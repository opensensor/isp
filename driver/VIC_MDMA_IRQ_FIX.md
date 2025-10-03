# VIC MDMA IRQ Function Fix

## Problem
The VIC IRQ was firing continuously but reporting "dma chid overflow (bit 3)" error. The error `v1_10 = 0x8` indicated that bit 3 of the MDMA interrupt status register was set, which means DMA channel ID overflow.

## Root Cause
The `vic_mdma_irq_function` implementation was **completely wrong**. It only handled the saveraw mode (when `stream_state == 0`) and returned early, completely missing the streaming mode logic that handles buffer circulation and MDMA interrupts.

## Binary Ninja Analysis
Using Binary Ninja MCP to decompile the reference driver's `vic_mdma_irq_function` revealed that it has **two completely different code paths**:

### Path 1: Saveraw Mode (`stream_state == 0`)
- Handles buffer rotation using global variables:
  - `vic_mdma_ch0_set_buff_index`
  - `vic_mdma_ch1_set_buff_index`
  - `vic_mdma_ch0_sub_get_num`
  - `vic_mdma_ch1_sub_get_num`
- Rotates buffer addresses in VIC registers (0x318-0x328)
- Signals completion when all buffers are processed

### Path 2: Streaming Mode (`stream_state != 0`)
- Reads current DMA buffer address from register 0x380
- Pops completed buffer from busy queue
- Calls `raw_pipe` callback to deliver completed buffer
- Moves completed buffer to free queue
- Searches for matching buffer if address doesn't match
- Queues next buffer from done queue if available
- Writes new buffer address to VIC register

## Changes Made

### 1. Rewrote `vic_mdma_irq_function` in `driver/tx_isp_vic.c`
- Implemented both saveraw and streaming mode paths
- Added proper buffer queue management (busy_head, free_head, done_head)
- Added raw_pipe callback invocation for buffer delivery
- Added buffer address matching logic
- Added proper spinlock protection

### 2. Made Global Variables Accessible
In `driver/tx-isp-module.c`:
- Changed MDMA channel variables from `static` to global
- Exported symbols:
  - `vic_mdma_ch0_sub_get_num`
  - `vic_mdma_ch1_sub_get_num`
  - `vic_mdma_ch0_set_buff_index`
  - `vic_mdma_ch1_set_buff_index`
- Added global `raw_pipe[8]` array and exported it

### 3. Updated Structure Definitions
In `driver/include/tx_isp_vic.h`:
- Added `busy_head` list to `tx_isp_vic_device` structure
- This is the queue of buffers currently being DMA'd by hardware

### 4. Updated External Declarations
In `driver/tx_isp_vic.c`:
- Changed local static variables to extern declarations
- Changed `extern void *raw_pipe` to `extern void *raw_pipe[]`

In `driver/tx_isp_tuning.c`:
- Changed from local `raw_pipe` array to using global `extern void *raw_pipe[]`

## Key Insights from Binary Ninja

### Buffer Address Calculation
The reference driver writes buffer addresses at:
```c
*(*($s0 + 0xb8) + (($v1_1 + 0xc6) << 2)) = $a1_2
```

Where:
- `$s0 + 0xb8` = `vic_dev->vic_regs`
- `$v1_1 + 0xc6` = buffer_index + 0xc6 (198 decimal)
- `198 << 2 = 792 = 0x318` (first buffer register)

So buffer registers are:
- Buffer 0: (0 + 0xc6) << 2 = 0x318
- Buffer 1: (1 + 0xc6) << 2 = 0x31c
- Buffer 2: (2 + 0xc6) << 2 = 0x320
- Buffer 3: (3 + 0xc6) << 2 = 0x324
- Buffer 4: (4 + 0xc6) << 2 = 0x328

### Raw Pipe Callback Structure
The `raw_pipe` is a function pointer array:
- `raw_pipe[0]` = `ispvic_frame_channel_qbuf`
- `raw_pipe[1]` = callback function for buffer delivery
- `raw_pipe[2]` = `ispvic_frame_channel_clearbuf`
- `raw_pipe[3]` = `ispvic_frame_channel_s_stream`
- `raw_pipe[4]` = callback argument (usually `sd`)

## Expected Behavior After Fix

With the correct implementation:
1. When MDMA channel 0 or 1 interrupt fires (bits 0 or 1 of v1_10)
2. The handler will pop the completed buffer from busy queue
3. Call the raw_pipe callback to deliver the buffer to userspace
4. Move the buffer to free queue
5. Queue the next buffer from done queue
6. Write the new buffer address to VIC hardware

This should eliminate the "dma chid overflow" error and allow proper frame streaming.

## Testing
After deploying this fix:
1. Load the new driver module
2. Start prudynt or other streaming application
3. Monitor dmesg for VIC IRQ messages
4. Should see "VIC SUCCESS: MDMA channel X interrupt" messages
5. Should NOT see "dma chid overflow" errors
6. Frames should be delivered to userspace via raw_pipe callbacks

## Files Modified
- `driver/tx_isp_vic.c` - Rewrote `vic_mdma_irq_function`
- `driver/tx-isp-module.c` - Made global variables accessible
- `driver/include/tx_isp_vic.h` - Added `busy_head` to structure
- `driver/tx_isp_tuning.c` - Use global raw_pipe

## Compilation
Build completed successfully with no errors.

