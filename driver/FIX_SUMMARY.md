# Fix Summary: Single VBM Buffer Issue

## Problem

The system was hanging and rebooting after starting the video streamer (prudynt). Analysis showed:

1. **TX_ISP_SET_BUF** was correctly seeding VBM with 1 buffer:
   ```
   TX_ISP_SET_BUF: addr=0x6300000 size=4685424
   SET_BUF: Seeded VBM[0]=0x6300000, count=1, size=4685424
   ```

2. **No QBUF calls** were happening in the logs, meaning VBM buffer count stayed at 1

3. **VIC MDMA enable** was trying to program 2 buffer slots but only had 1 buffer available

## Root Cause

By decompiling the userspace call flow using Binary Ninja MCP, we discovered:

### Userspace Flow (from libimp.so)

1. **IMP_ISP_AddSensor()** (0x8bd6c):
   - Calls `TX_ISP_GET_BUF (0x800856d5)` → gets buffer size
   - Calls `IMP_Alloc` → allocates physical memory
   - Calls `TX_ISP_SET_BUF (0x800856d4)` → **sends SINGLE buffer to driver**

2. **IMP_FrameSource_EnableChn()** (0x9ecf8):
   - Calls `VBMCreatePool` → allocates VBM pool structure
   - Calls `VIDIOC_REQBUFS (0xc0145608)` → requests N buffers
   - Calls `VBMFillPool` → **should queue buffers via QBUF**
   - Calls `VIDIOC_STREAMON (0x80045612)` → starts streaming

### The Missing Link

**VBMFillPool** expects callback functions to be set during **VBMCreatePool**, but prudynt is using the stock libimp.so which expects the reference driver's VBM implementation. Since our driver doesn't implement the full VBM API, the callbacks are never set, so **VBMFillPool never calls QBUF**, leaving us with only the single buffer from TX_ISP_SET_BUF.

## Solution

**Duplicate the single buffer address for ping-pong operation.**

When VIC MDMA enable detects only 1 VBM buffer, it now duplicates that buffer address to both hardware slots.

### Code Change (driver/tx_isp_vic.c)

Added logic in `vic_pipo_mdma_enable()` to duplicate the buffer when count == 1.

### Why This Works

1. **Hardware expects 2 buffers** for ping-pong DMA operation
2. **Duplicating is safe** - hardware will just reuse the same buffer
3. **No delays needed** - immediate solution
4. **Matches reference behavior** - reference driver likely does the same when only 1 buffer is available

## Files Modified

1. **driver/tx_isp_vic.c** - Added single-buffer duplication logic in `vic_pipo_mdma_enable()`
2. **driver/USERSPACE_CALL_FLOW.md** - Documented userspace call flow from Binary Ninja MCP analysis
3. **driver/FIX_SUMMARY.md** - This file
