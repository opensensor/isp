# Final Buffer Count Fix - Complete Solution

## Problem Summary

**Control limit errors and 5/5 interrupt stalls** caused by VIC being configured for 1 buffer but receiving 5 buffers.

## Root Causes Found

### 1. Buffer Allocation from Wrong Memory Region
- **Problem**: VBM pool was using `dma_alloc_coherent()` which allocates from regular system memory (0x2000000 range)
- **VIC Requirement**: VIC hardware can only access ISP reserved memory (rmem) at 0x6000000-0x8000000
- **Result**: Buffer addresses like 0x25eec00 were rejected as invalid
- **Fix**: Changed VBM pool allocation to use ISP rmem with `ioremap_nocache()`

### 2. Buffer Count Mismatch
- **Problem**: VIC configured for 1 buffer but prudynt queues 5 buffers
- **Cause**: Prudynt uses newer allocation strategy:
  - Does NOT call REQBUFS
  - Calls TX_ISP_SET_BUF once (sets vbm_buffer_count=1)
  - Calls QBUF 5 times (queues 5 buffers)
  - STREAMON used vbm_buffer_count=1 instead of actual queued count=5
- **Result**: VIC[0x300] = 0x80010020 (1 buffer) but 5 buffers queued → control limit error
- **Fix**: STREAMON now uses VIC's active_buffer_count (incremented during QBUF) instead of vbm_buffer_count

## Fixes Applied

### Fix 1: VBM Pool Allocation from rmem

**File: `driver/tx-isp-module.c` (lines 2856-2900)**

```c
/* CRITICAL FIX: Allocate from ISP reserved memory (rmem), not regular DMA memory! */
/* VIC hardware can only access rmem region (0x6000000-0x8000000) */
if (ourISPdev && ourISPdev->rmem_addr && ourISPdev->rmem_size) {
    /* Calculate offset in rmem for this channel */
    size_t rmem_offset = 0;
    if (channel == 1) {
        /* Channel 1 starts after channel 0's buffers */
        rmem_offset = 12 * 1024 * 1024;
    }
    
    /* Check if we have enough rmem space */
    if (rmem_offset + total_dma_size > ourISPdev->rmem_size) {
        pr_err("*** Channel %d: Not enough rmem space ***\n", channel);
        return -ENOMEM;
    }
    
    /* Use rmem region - map physical address to virtual */
    dma_paddr = ourISPdev->rmem_addr + rmem_offset;
    dma_vaddr = ioremap_nocache(dma_paddr, total_dma_size);
    
    pr_info("*** VBM Pool: Using ISP rmem: vaddr=%p, paddr=0x%x, size=%zu ***\n",
            dma_vaddr, (u32)dma_paddr, total_dma_size);
}
```

**What Changed:**
- ❌ OLD: `dma_alloc_coherent(NULL, ...)` → allocates from system memory (0x2000000)
- ✅ NEW: `ioremap_nocache(rmem_addr + offset, ...)` → uses ISP rmem (0x6000000)

### Fix 2: VBM Pool Cleanup

**File: `driver/tx-isp-module.c` (lines 1402-1417)**

```c
/* CRITICAL: Free VBM pool resources */
if (fcd->vbm_pool) {
    /* NOTE: Do NOT free rmem memory - it's managed by ISP device */
    /* We're using ISP's reserved memory region, not dma_alloc_coherent */
    pr_info("*** Channel %d: VBM pool used rmem (not freeing, managed by ISP) ***\n", fcd->channel_num);
    
    /* Free VBM pool structure */
    kfree(fcd->vbm_pool);
    fcd->vbm_pool = NULL;
}
```

**What Changed:**
- ❌ OLD: `dma_free_coherent()` to free allocated memory
- ✅ NEW: No memory free (rmem is managed by ISP device, just `iounmap` on module unload)

### Fix 3: Use Actual Queued Buffer Count in STREAMON

**File: `driver/tx-isp-module.c` (lines 3836-3874)**

```c
/* CRITICAL FIX: Use VIC's active_buffer_count (incremented during QBUF), not vbm_buffer_count! */
/* VIC already knows how many buffers were queued via ispvic_frame_channel_qbuf */
int actual_queued = ourISPdev->vic_dev->active_buffer_count;

/* If no buffers queued yet, use vbm_buffer_count as fallback */
if (actual_queued == 0) {
    actual_queued = (st->vbm_buffer_count > 5) ? 5 : st->vbm_buffer_count;
}

/* Ensure we have at least 2 buffers for ping-pong */
if (actual_queued == 0) actual_queued = 2;

/* Cap at 5 buffers (VIC hardware limit) */
if (actual_queued > 5) actual_queued = 5;

ourISPdev->vic_dev->active_buffer_count = actual_queued;

pr_info("STREAMON: VIC ring active_count=%u (from QBUF), last_idx=%d\n",
        ourISPdev->vic_dev->active_buffer_count,
        ourISPdev->vic_dev->last_idx);

/* CRITICAL: Send 0x3000008 event to VIC with buffer count (Binary Ninja EXACT) */
int32_t buffer_count_for_vic = ourISPdev->vic_dev->active_buffer_count;
pr_info("*** STREAMON: Sending 0x3000008 event to VIC with buffer_count=%d ***\n", 
        buffer_count_for_vic);

int event_result = tx_isp_send_event_to_remote(&ourISPdev->vic_dev->sd, 0x3000008, &buffer_count_for_vic);
```

**What Changed:**
- ❌ OLD: `active_buffer_count = vbm_buffer_count` (from SET_BUF = 1)
- ✅ NEW: `active_buffer_count = vic_dev->active_buffer_count` (from QBUF calls = 5)
- ✅ NEW: Send 0x3000008 event to VIC with correct buffer count

### Fix 4: Use Active Buffer Count in VIC STREAMON

**File: `driver/tx_isp_vic.c` (lines 2702-2718)**

```c
/* CRITICAL FIX: Use active_buffer_count from REQBUFS, not hardcoded value! */
/* This was set during REQBUFS via 0x3000008 event */
u32 buffer_count = vic_dev->active_buffer_count;
if (buffer_count == 0) {
    /* Fallback: Use default if not set */
    buffer_count = 3;
    vic_dev->active_buffer_count = 3;
    pr_warn("*** VIC MDMA: active_buffer_count was 0, using default 3 ***\n");
}
u32 stream_ctrl = (buffer_count << 16) | 0x80000020;  /* Binary Ninja EXACT formula */
pr_info("*** VIC MDMA: Using active_buffer_count=%d from REQBUFS (Binary Ninja EXACT) ***\n", buffer_count);
```

**What Changed:**
- ❌ OLD: `buffer_count = 3` (hardcoded)
- ✅ NEW: `buffer_count = vic_dev->active_buffer_count` (from 0x3000008 event)

### Fix 5: Use VBM Pool Buffers in VIC STREAMON

**File: `driver/tx_isp_vic.c` (lines 2727-2777)**

```c
/* Try to get buffer addresses from VBM pool first */
if (fcd->vbm_pool && fcd->vbm_pool->buffer_count >= buffer_count) {
    int i;
    pr_info("*** VIC BUFFER INIT: Using VBM pool buffers (Binary Ninja EXACT) ***\n");
    
    for (i = 0; i < buffer_count && i < 5; i++) {
        u32 buf_addr = (u32)fcd->vbm_pool->buffers[i].paddr;
        u32 reg_offset = 0x318 + (i * 4);
        
        writel(buf_addr, vic_base + reg_offset);
        if (vic_ctrl)
            writel(buf_addr, vic_ctrl + reg_offset);
        
        pr_info("*** VIC BUFFER %d: 0x%x (from VBM pool) ***\n", i, buf_addr);
    }
    wmb();
}
```

**What Changed:**
- ❌ OLD: Hardcoded 3 buffers from rmem with fixed addresses
- ✅ NEW: Use actual buffer_count buffers from VBM pool with allocated addresses

## Event Flow (Corrected)

### Prudynt's Newer Allocation Strategy:
```
prudynt: IMP_System_Init()
    ↓
libimp.so: Allocates buffers internally
    ↓
prudynt: TX_ISP_SET_BUF(buffer[0])
    ↓
Driver: vbm_buffer_addresses[0] = 0x6300000, vbm_buffer_count = 1
    ↓
prudynt: QBUF(buffer[0])
    ↓
VIC: active_buffer_count++ = 1
    ↓
prudynt: QBUF(buffer[1])
    ↓
VIC: active_buffer_count++ = 2
    ↓
prudynt: QBUF(buffer[2])
    ↓
VIC: active_buffer_count++ = 3
    ↓
prudynt: QBUF(buffer[3])
    ↓
VIC: active_buffer_count++ = 4
    ↓
prudynt: QBUF(buffer[4])
    ↓
VIC: active_buffer_count++ = 5
    ↓
prudynt: STREAMON
    ↓
Driver: Use VIC's active_buffer_count = 5 ✅
    ↓
Driver: Send 0x3000008 event to VIC with buffer_count=5 ✅
    ↓
VIC: vic_core_ops_ioctl(0x3000008, &buffer_count=5)
    ↓
VIC: active_buffer_count = 5 (confirmed)
    ↓
Driver: ispvic_frame_channel_s_stream(1)
    ↓
VIC: Writes (5 << 16) | 0x80000020 = 0x80050020 to register 0x300 ✅
    ↓
VIC: Programs 5 buffer addresses from VBM pool ✅
    ↓
VIC: Starts streaming with 5 buffers ✅
    ↓
VIC: No control limit errors! ✅
```

## Expected Log Output After Fix

```
TX_ISP_SET_BUF: addr=0x6300000 size=4685424
*** SET_BUF: Added VBM[0]=0x6300000, new_count=1 ***

*** Channel 0: QBUF - Buffer 0: phys_addr=0x6300000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 1

*** Channel 0: QBUF - Buffer 1: phys_addr=0x6600000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 2

*** Channel 0: QBUF - Buffer 2: phys_addr=0x6900000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 3

*** Channel 0: QBUF - Buffer 3: phys_addr=0x6c00000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 4

*** Channel 0: QBUF - Buffer 4: phys_addr=0x6f00000 (VALIDATED) ***
ispvic_frame_channel_qbuf: Incremented active_buffer_count to 5

*** Channel 0: VIDIOC_STREAMON ***
STREAMON: VIC ring active_count=5 (from QBUF), last_idx=4
*** STREAMON: Sending 0x3000008 event to VIC with buffer_count=5 (Binary Ninja EXACT) ***
*** vic_core_ops_ioctl: VIC active_buffer_count set to 5 (Binary Ninja EXACT) ***

*** VIC MDMA: Using active_buffer_count=5 from REQBUFS (Binary Ninja EXACT) ***
*** Binary Ninja EXACT: Wrote 0x80050020 to reg 0x300 (5 buffers) ***
*** VIC BUFFER INIT: Using VBM pool buffers (Binary Ninja EXACT) ***
*** VIC BUFFER 0: 0x6300000 (from VBM pool) ***
*** VIC BUFFER 1: 0x6600000 (from VBM pool) ***
*** VIC BUFFER 2: 0x6900000 (from VBM pool) ***
*** VIC BUFFER 3: 0x6c00000 (from VBM pool) ***
*** VIC BUFFER 4: 0x6f00000 (from VBM pool) ***
*** READBACK: VIC[0x300]=0x80050020, buffer_count_field=5 ***

*** VIC: Frame done interrupt ***
*** VIC: Frame done interrupt ***
*** VIC: Frame done interrupt ***
... (continuous interrupts, no stalls!)
```

## Success Criteria

✅ **Buffer addresses in valid rmem range** (0x6000000-0x8000000)
✅ **VIC configured for correct buffer count** (5 buffers, not 1)
✅ **No control limit errors**
✅ **Continuous frame done interrupts** (no 5/5 stalls)
✅ **Real frame data** from sensor (not green/black)

## Files Modified

1. **driver/tx-isp-module.c**:
   - Lines 2856-2900: VBM pool allocation from rmem instead of dma_alloc_coherent
   - Lines 1402-1417: VBM pool cleanup (no dma_free_coherent)
   - Lines 3836-3874: STREAMON uses VIC's active_buffer_count and sends 0x3000008 event

2. **driver/tx_isp_vic.c**:
   - Lines 2702-2718: Use active_buffer_count instead of hardcoded 3
   - Lines 2727-2777: Use VBM pool buffers instead of hardcoded rmem addresses

## Testing

After this fix:
1. Build driver: `make clean && make`
2. Install driver: `sudo insmod tx-isp-t31.ko`
3. Start prudynt: `./prudynt`
4. Check logs: `dmesg -w | grep -E "VIC|buffer_count|0x300|control limit"`
5. Verify: 
   - Buffer addresses in 0x6000000 range
   - VIC[0x300] = 0x80050020 (5 buffers)
   - No control limit errors
   - Continuous interrupts

The buffer count should now be consistent throughout the entire pipeline!

