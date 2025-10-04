# Buffer Calculation Analysis - 2025-10-04

## Problem: VIC Needs Multiple Buffers But VBM Only Has Space for 1

### Current Situation

**From logs:**
- TX_ISP_GET_BUF returns: size=4,685,424 bytes, paddr=0x06300000
- This is the VBM buffer allocated by prudynt for channel 0

**Frame size calculation (1920x1080 NV12):**
- Y plane: 1920 × 1080 = 2,073,600 bytes
- UV plane: 1920 × 1080 / 2 = 1,036,800 bytes
- Overhead: 4,416 bytes (from Binary Ninja formula)
- **Total per frame**: 3,114,816 bytes

**Number of frames that fit:**
- 4,685,424 ÷ 3,114,816 = **1.504 frames**
- **We can only fit 1 complete frame!**

### The Root Cause

**VIC hardware requires at least 2 buffers for ping-pong operation:**
1. Buffer 0: VIC writes frame data
2. Buffer 1: VIC switches to this while CPU processes buffer 0

**With only 1 buffer:**
- VIC captures frame 0
- VIC tries to start frame 1
- **No buffer 1 available** → control limit error (bit 21)
- VIC stops generating interrupts

### Why v1_7 = 0x200000 (No Frame Done Bit)

From the logs:
```
*** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x200000, v1_10=0x0) ***
*** VIC ISR: Frame done bit NOT set (v1_7 & 1 = 0), v1_7=0x200000 ***
Err2 [VIC_INT] : control limit err!!!
```

**v1_7 = 0x200000 = bit 21 only**
- Bit 21 = control limit error
- Bit 0 (frame done) NOT set

**This means:**
- VIC started capturing frame 0
- VIC looked for buffer 1 address in VIC[0x31c]
- **VIC[0x31c] = 0x00000000** (no buffer 1!)
- VIC generated control limit error
- VIC did NOT complete frame 0
- No frame done interrupt

### Available Memory

**Total rmem**: 29M = 30,408,704 bytes (from device tree)
**Base address**: 0x06300000

**We have plenty of space!**
- Channel 0 (1920x1080): 3 × 3,114,816 = 9,344,448 bytes
- Channel 1 (640x360): 3 × 345,600 = 1,036,800 bytes
- **Total needed**: 10,381,248 bytes
- **Available**: 30,408,704 bytes
- **Remaining**: 20,027,456 bytes ✅

### The Solution

**Option 1: Subdivide the VBM buffer (WRONG)**
- VBM buffer is only 4,685,424 bytes
- Can't fit 2 complete frames
- Won't work

**Option 2: Use rmem directly for multiple buffers (CORRECT)**
- Ignore the VBM buffer size from TX_ISP_GET_BUF
- Calculate buffer addresses directly from rmem
- Program 3 buffers per channel into VIC registers

### Buffer Layout in rmem

**Base address**: 0x06300000
**Frame size**: 3,114,816 bytes (rounded up to 3,145,728 for alignment)

**Channel 0 (1920x1080) - 3 buffers:**
- Buffer 0: 0x06300000
- Buffer 1: 0x06600000 (+ 3M)
- Buffer 2: 0x06900000 (+ 6M)

**Channel 1 (640x360) - 3 buffers:**
- Buffer 0: 0x06c00000 (+ 9M)
- Buffer 1: 0x06d00000 (+ 10M)
- Buffer 2: 0x06e00000 (+ 11M)

**Total used**: 12M out of 29M available

### Implementation Plan

1. **Modify vic_core_s_stream()** to calculate 3 buffer addresses per channel
2. **Write all 3 addresses** to VIC[0x318], VIC[0x31c], VIC[0x320]
3. **Set buffer_count = 3** in VIC[0x300]
4. **Initialize buffer management** with 3 buffers in free queue

### Code Changes Needed

**File: driver/tx_isp_vic.c**

**Location**: vic_core_s_stream() around line 2720

**Current code** (WRONG - only 1 buffer):
```c
vic_dev->active_buffer_count = 1;
u32 buffer_count = 1;
```

**New code** (CORRECT - 3 buffers):
```c
/* Calculate 3 buffer addresses from rmem */
u32 base_addr = isp_dev->rmem_addr;  // 0x06300000
u32 frame_size = 3145728;  // 3M per buffer (aligned)

/* Channel 0 buffers */
writel(base_addr, vic_regs + 0x318);              // Buffer 0
writel(base_addr + frame_size, vic_regs + 0x31c);     // Buffer 1
writel(base_addr + (frame_size * 2), vic_regs + 0x320);  // Buffer 2

/* Set buffer count to 3 */
vic_dev->active_buffer_count = 3;
u32 buffer_count = 3;
u32 stream_ctrl = (buffer_count << 16) | 0x80000020;
writel(stream_ctrl, vic_regs + 0x300);
```

### Expected Results

**After fix:**
```
[  X.XXX] *** VIC BUFS (PRIMARY): [0x318]=0x06300000 [0x31c]=0x06600000 [0x320]=0x06900000 ***
[  X.XXX] *** VIC CTRL (PRIMARY): [0x300]=0x80030020 (3 buffers) ***
[  X.XXX] *** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x1, v1_10=0x0) ***
[  X.XXX] *** VIC ISR: Frame done bit SET (v1_7 & 1 = 1) ***
[  X.XXX] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=1) ***
[  X.XXX] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=2) ***
[  X.XXX] *** VIC FRAME DONE INTERRUPT: Frame completion detected (count=3) ***
... (continuous interrupts!)
```

**Key indicators:**
- ✅ Multiple buffer addresses programmed (not all zeros)
- ✅ Buffer count = 3 in VIC[0x300]
- ✅ Frame done bit (bit 0) SET in v1_7
- ✅ Continuous interrupts at ~30 FPS
- ✅ No control limit errors

### Why This Will Work

1. **VIC has buffers to rotate**: 3 buffers available
2. **Ping-pong operation works**: While processing buffer 0, VIC writes to buffer 1
3. **No control limit error**: VIC always has a free buffer
4. **Frame done bit sets**: VIC completes frames successfully
5. **Continuous interrupts**: Buffer rotation keeps VIC fed

---

**This is the REAL fix for the 5/5 stall!** 🎯

