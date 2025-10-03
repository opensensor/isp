# VIC Buffer Management Fix - DMA CHID Overflow Root Cause

## Problem
Still getting "dma chid overflow (bit 3)" error even after implementing the correct `vic_mdma_irq_function`. The logs show:
- `v1_7 = 0x0` (no frame done)
- `v1_10 = 0x8` (DMA channel ID overflow)
- VIC control = 0x80000020 (only 0 buffers configured!)

## Root Cause Analysis

### Issue 1: `vic_pipo_mdma_enable` Writing Buffer Addresses
**Binary Ninja shows:**
```c
int32_t $v1 = *(arg1 + 0xdc)
*(*(arg1 + 0xb8) + 0x308) = 1
int32_t $v1_1 = $v1 << 1
*(*(arg1 + 0xb8) + 0x304) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0)
*(*(arg1 + 0xb8) + 0x310) = $v1_1
void* result = *(arg1 + 0xb8)
*(result + 0x314) = $v1_1
return result
```

The reference `vic_pipo_mdma_enable` **ONLY** writes:
1. 0x308 = 1 (MDMA enable)
2. 0x304 = dimensions
3. 0x310 = stride
4. 0x314 = stride

**It does NOT write buffer addresses (0x318-0x328)!**

Our implementation (lines 2717-2776 in tx_isp_vic.c) incorrectly writes buffer addresses in `vic_pipo_mdma_enable`. This is wrong!

### Issue 2: `ispvic_frame_channel_qbuf` Not Managing Queues
**Binary Ninja shows:**
```c
$a1_1, $a2_1 = pop_buffer_fifo($s0 + 0x1f4)  // Pop from free queue
void** $v0_5, void* $a3_1 = $a1_1($a2_1)
int32_t $a1_2 = *($a3_1 + 8)                 // Get buffer address
int32_t $v1_1 = $v0_5[4]                     // Get buffer index
$v0_5[2] = $a1_2                             // Store buffer address in entry
*(*($s0 + 0xb8) + (($v1_1 + 0xc6) << 2)) = $a1_2  // Write to VIC register
void** $v1_5 = *($s0 + 0x208)
*($s0 + 0x208) = $v0_5                       // Add to busy queue
*$v0_5 = $s0 + 0x204
$v0_5[1] = $v1_5
*$v1_5 = $v0_5
*($s0 + 0x218) += 1                          // Increment active_buffer_count
```

The reference `ispvic_frame_channel_qbuf`:
1. Pops a buffer entry from the free queue (offset 0x1f4)
2. Gets the buffer address from the entry
3. Writes the buffer address to VIC register at `(index + 0xc6) << 2`
4. Adds the buffer entry to the busy queue (offset 0x204)
5. Increments `active_buffer_count` (offset 0x218)

Our implementation (lines 3721-3767) is a stub that:
- Doesn't pop from free queue
- Doesn't add to busy queue
- Doesn't increment active_buffer_count properly
- Just writes a buffer address if one is provided

### Issue 3: Buffer Queue Initialization
The buffer queues need to be properly initialized with buffer entries. Looking at the Binary Ninja decompilation of `tx_isp_subdev_pipo`, it should:
1. Initialize the list heads (done_head, free_head, busy_head)
2. Allocate buffer entries and add them to the free queue
3. Set up the raw_pipe function pointers

## Required Fixes

### Fix 1: Remove Buffer Address Writes from `vic_pipo_mdma_enable`
Remove lines 2717-2776 in `tx_isp_vic.c` that write buffer addresses. The function should ONLY write:
- 0x308 = 1
- 0x304 = dimensions
- 0x310 = stride
- 0x314 = stride

### Fix 2: Implement Proper `ispvic_frame_channel_qbuf`
Rewrite `ispvic_frame_channel_qbuf` to match Binary Ninja:
1. Check if free queue is empty - if so, print "bank no free" and return
2. Check if done queue is empty - if so, print "qbuffer null" and return
3. Pop buffer entry from free queue
4. Get buffer address from done queue entry
5. Write buffer address to VIC register at `(index + 0xc6) << 2`
6. Add buffer entry to busy queue
7. Increment active_buffer_count

### Fix 3: Initialize Buffer Queues in `tx_isp_subdev_pipo`
The `tx_isp_subdev_pipo` function should:
1. Initialize list heads (already done)
2. Allocate 5 buffer entries
3. Add them to the free queue
4. Initialize buffer indices

### Fix 4: Populate Done Queue with VBM Buffers
When VBM buffers are available, they need to be added to the done queue so that `ispvic_frame_channel_qbuf` can pop them and write their addresses to VIC registers.

## Expected Behavior After Fix

1. `tx_isp_subdev_pipo` initializes buffer queues and allocates buffer entries
2. VBM buffer addresses are added to the done queue
3. `ispvic_frame_channel_qbuf` is called
4. It pops from free queue, gets address from done queue, writes to VIC register
5. Adds entry to busy queue and increments active_buffer_count
6. VIC control register shows correct buffer count (e.g., 0x80040020 for 4 buffers)
7. VIC hardware can now DMA to buffers and generate MDMA interrupts
8. `vic_mdma_irq_function` handles the interrupts and rotates buffers

## Key Insight
The "dma chid overflow" error occurs because:
1. `vic_pipo_mdma_enable` doesn't write buffer addresses (correct per Binary Ninja)
2. `ispvic_frame_channel_qbuf` is supposed to write them, but our stub doesn't
3. VIC hardware has no buffer addresses configured
4. When it tries to DMA, it has no valid channel/buffer to use
5. This causes the DMA channel ID to overflow

The fix is to properly implement the buffer queue management so that buffer addresses get written to VIC registers through the correct code path (`ispvic_frame_channel_qbuf`), not through `vic_pipo_mdma_enable`.

## Implementation Summary

### Changes Made

1. **Removed buffer address writes from `vic_pipo_mdma_enable`** (lines 2715-2720)
   - Function now only writes: 0x308, 0x304, 0x310, 0x314 (matching Binary Ninja)
   - Does NOT write buffer addresses to 0x318-0x328

2. **Rewrote `ispvic_frame_channel_qbuf`** (lines 3661-3750)
   - Implements proper buffer queue management
   - Adds new buffer to done queue if provided
   - Checks if free queue is empty ("bank no free")
   - Checks if done queue is empty ("qbuffer null")
   - Pops buffer entry from done queue
   - Pops free entry from free queue
   - Copies buffer info to free entry
   - Writes buffer address to VIC register at `(index + 0xc6) << 2`
   - Adds free entry to busy queue
   - Increments active_buffer_count

3. **Allocated buffer entries in `tx_isp_subdev_pipo`** (lines 3832-3874)
   - Allocates 5 buffer entries using `VIC_BUFFER_ALLOC()`
   - Initializes each entry with buffer_index
   - Adds all entries to free queue
   - Clears VIC registers for each buffer slot

4. **Populated done queue with VBM buffers** (lines 3876-3951)
   - Gets VBM buffer addresses from frame_channels[0].state
   - Allocates done entries for each VBM buffer
   - Adds done entries to done queue with actual physical addresses
   - Falls back to reserved memory addresses if VBM not available

5. **Called qbuf multiple times to program all buffers** (lines 3953-3964)
   - Calls `ispvic_frame_channel_qbuf` 5 times
   - Each call pops from done queue and programs one buffer
   - Increments active_buffer_count for each successful call

6. **Added busy_head initialization** (line 3806)
   - Added `INIT_LIST_HEAD(&vic_dev->busy_head)` to initialize busy queue

### Expected Behavior After Fix

1. `tx_isp_subdev_pipo` allocates 5 free buffer entries
2. VBM buffer addresses are added to done queue (5 entries)
3. `ispvic_frame_channel_qbuf` is called 5 times
4. Each call:
   - Pops from done queue (gets VBM address)
   - Pops from free queue (gets free entry)
   - Writes VBM address to VIC register
   - Adds entry to busy queue
   - Increments active_buffer_count
5. After 5 calls:
   - active_buffer_count = 5
   - VIC registers 0x318-0x328 contain VBM buffer addresses
   - VIC control register shows 0x80050020 (5 buffers)
   - VIC hardware can now DMA to buffers
6. VIC MDMA interrupts should fire when frames complete
7. `vic_mdma_irq_function` handles interrupts and rotates buffers

### Build Status
✅ Build completed successfully with no compilation errors

### Next Steps
1. Deploy to device and test
2. Monitor dmesg for:
   - "bank no free" or "qbuffer null" errors
   - Buffer address writes to VIC registers
   - active_buffer_count increments
   - VIC control register value (should be 0x80050020)
   - MDMA interrupts firing
   - Frame done interrupts (v1_7 & 1)
3. Verify no "dma chid overflow" errors

