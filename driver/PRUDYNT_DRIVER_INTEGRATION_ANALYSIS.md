# Prudynt-to-Driver Integration Analysis

## Executive Summary

With access to both the reference driver (`tx-isp-t31.ko`) and userspace library (`libimp.so`) via Binary Ninja MCP, we can now trace the complete flow from prudynt's expectations through libimp.so to what our driver must provide.

**Current Status**: 5/5 interrupt stalls with control limit errors
**Root Cause**: Mismatch between what prudynt expects and what our driver provides

## Complete Call Flow Analysis

### 1. Prudynt Initialization Sequence (IMPSystem.cpp)

```cpp
// Line 58-59: Open ISP
ret = IMP_ISP_Open();

// Line 62-68: Add sensor
sinfo = create_sensor_info(cfg->sensor.model);
ret = IMP_ISP_AddSensor(&sinfo);

// Line 73-75: Enable sensor
ret = IMP_ISP_EnableSensor();

// Line 78-79: Initialize system
ret = IMP_System_Init();

// Line 81-82: Enable tuning
ret = IMP_ISP_EnableTuning();
```

### 2. libimp.so System Initialization (Binary Ninja)

**IMP_System_Init** (0x1dab0):
```c
// Calls modify_phyclk_strength()
// Then calls system_init()
```

**system_init** (0x1d2ac):
```c
// Initializes timestamp_base
// Calls 6 subsystem init functions in sequence:
for (i = 0; i < 6; i++) {
    result = (*subsystem[i].init)();
    if (result < 0) {
        // Cleanup on failure
        for (j = i - 1; j >= 0; j--) {
            (*subsystem[j].cleanup)();
        }
        return result;
    }
}
```

**Key Insight**: System initialization is a multi-stage process with proper error handling and cleanup.

### 3. Prudynt Frame Channel Setup (IMPFramesource.cpp)

```cpp
// Line 105-106: Create channel
ret = IMP_FrameSource_CreateChn(chnNr, &chnAttr);

// Line 108-109: Set channel attributes
ret = IMP_FrameSource_SetChnAttr(chnNr, &chnAttr);

// Line 135-136: Enable channel (in enable() method)
ret = IMP_FrameSource_EnableChn(chnNr);
```

### 4. libimp.so Frame Channel Operations (Binary Ninja)

**IMP_FrameSource_CreateChn** (0x9d5d4):
- Validates channel number (< 5)
- Validates channel attributes
- Creates pthread mutex and condition variables
- Calls `create_group()` to set up channel group
- Initializes channel state to 1 (created)
- **Does NOT open /dev/framechan device yet**

**IMP_FrameSource_EnableChn** (0x9ecf8):
```c
// CRITICAL: This is where /dev/framechan0 is opened!
sprintf(&str, "/dev/framechan%d", channel_num);

// Retry loop for device open (up to 256 times with 10ms sleep)
for (retry = 0; retry < 256; retry++) {
    fd = open(&str, O_RDWR | O_NONBLOCK, 0);
    if (fd >= 0) break;
    usleep(10000);  // 10ms
}

// Store file descriptor
channel_state->fd = fd;

// Configure channel format via ioctl
ioctl(fd, 0x407056c4, &format_struct);  // Get format
ioctl(fd, 0xc07056c3, &format_struct);  // Set format

// Create VBM pool
VBMCreatePool(channel_num, &pool_config, &pool_callbacks, gFramesource);

// Request buffers via ioctl
ioctl(fd, 0xc0145608, &reqbuf_struct);  // VIDIOC_REQBUFS

// Fill VBM pool with buffers
VBMFillPool(channel_num);

// Start streaming via ioctl
ioctl(fd, 0x80045612, &stream_on);  // VIDIOC_STREAMON

// Create frame polling thread
pthread_create(&channel_state->thread, 0, frame_poll_thread, &channel_num);
```

**Key Insight**: The device `/dev/framechan0` is created dynamically when `IMP_FrameSource_EnableChn` is called, NOT during system init!

### 5. What Our Driver Must Provide

#### A. Device Creation Timing

**CRITICAL**: `/dev/framechan0` must be created BEFORE prudynt calls `IMP_FrameSource_EnableChn`.

Current implementation creates it during module init, which is correct.

#### B. IOCTL Support Required

Our `frame_channel_unlocked_ioctl` must handle:

1. **0x407056c4** - Get format (VIDIOC_G_FMT)
2. **0xc07056c3** - Set format (VIDIOC_S_FMT)
3. **0xc0145608** - Request buffers (VIDIOC_REQBUFS)
4. **0x80045612** - Stream on (VIDIOC_STREAMON)
5. **0x80045613** - Stream off (VIDIOC_STREAMOFF)
6. **0xc044560f** - Queue buffer (VIDIOC_QBUF)
7. **0xc0445611** - Dequeue buffer (VIDIOC_DQBUF)

**Status**: All implemented in `tx-isp-module.c` lines 2495-3100

#### C. VBM Buffer Management

**VBMCreatePool** expects:
- Pool configuration with buffer count, size, format
- Callback functions for buffer allocation/deallocation
- Pool state tracking

**VBMFillPool** expects:
- Ability to queue buffers via QBUF ioctl
- Buffers allocated from VBM pool
- Buffer addresses written to VIC registers

**Status**: Partially implemented, needs verification

### 6. Reference Driver VIC Start Sequence (Binary Ninja)

**vic_core_s_stream** (0x15b4):
```c
if (enable) {
    if (current_state != 4) {
        // Disable IRQ
        tx_vic_disable_irq();
        
        // Start VIC hardware
        tx_isp_vic_start(vic_dev);
        
        // Update state to 4 (streaming)
        vic_dev->state = 4;
        
        // Enable IRQ
        tx_vic_enable_irq();
    }
}
```

**tx_isp_vic_start** (0x260):
- Configures VIC registers based on sensor type (MIPI/DVP/BT656/BT601/BT1120)
- For MIPI (our case):
  - Sets VIC mode register (0xc) = 2
  - Sets VIC dimensions register (0x4)
  - Sets VIC control register (0x10c)
  - Sets VIC stride register (0x100)
  - Configures interrupt routing
  - Writes control register (0x0) = 1 to start

**Status**: Implemented in `tx_isp_vic.c` lines 1028-1580

### 7. Interrupt Flow (Reference Driver)

**isp_irq_handle** (0xa060):
```c
// Top-half handler (fast path)
// Checks if VIC has interrupt pending
if (vic_dev && vic_dev->ops && vic_dev->ops->irq_handler) {
    result = vic_dev->ops->irq_handler();
    if (result == 2) {
        return IRQ_WAKE_THREAD;  // Wake threaded handler
    }
}
return IRQ_HANDLED;
```

**isp_irq_thread_handle** (0xa130):
```c
// Bottom-half handler (threaded)
// Processes VIC frame completion
if (vic_dev && vic_dev->ops && vic_dev->ops->irq_thread_handler) {
    vic_dev->ops->irq_thread_handler();
}
```

**Status**: Implemented in `tx_isp_core.c` lines 1838-2050

## Critical Gaps Identified

### Gap 1: VIC Interrupt Handler Registration

**Reference driver** registers VIC interrupt handlers during `vic_core_s_stream`:
```c
vic_dev->ops->irq_handler = vic_irq_top_half;
vic_dev->ops->irq_thread_handler = vic_irq_bottom_half;
```

**Our driver** registers during probe, which may be too early.

**Fix**: Move VIC interrupt handler registration to `vic_core_s_stream` enable path.

### Gap 2: VIC Frame Done Interrupt Routing

**Reference driver** uses VIC registers 0x1e0-0x1f4 for interrupt configuration.

**Our driver** uses registers 0x04/0x0c which may be incorrect.

**Fix**: Update VIC interrupt configuration to use correct register range.

### Gap 3: Buffer Address Programming

**Reference driver** programs buffer addresses during QBUF:
```c
reg_offset = (buffer_index + 0xc6) << 2;
writel(buffer_addr, vic_regs + reg_offset);
```

**Our driver** programs during MDMA enable, which may be too late.

**Fix**: Ensure buffer addresses are programmed during QBUF before streaming starts.

### Gap 4: Control Limit Error (Bit 21)

**Symptom**: VIC control register shows 0x80020020 (bit 17 set) causing control limit error.

**Root Cause**: Buffer index 2 shifted left 16 bits = 0x00020000 = bit 17 set!

**Fix**: Already applied in `tx_isp_vic.c` lines 492-507 (mask 0xFFFDFFFF to clear bit 17).

**Verification Needed**: Check if fix is actually being executed during interrupt handling.

## Action Plan

### Priority 1: Verify Control Limit Fix is Active

1. Add logging to VIC interrupt handler to show control register updates
2. Verify mask 0xFFFDFFFF is being used to clear bit 17
3. Check if buffer index is being properly masked to 4 bits

### Priority 2: Fix VIC Interrupt Configuration

1. Update `tx_isp_vic_hw_init` to use registers 0x1e0-0x1f4
2. Remove incorrect register writes to 0x04/0x0c
3. Verify interrupt routing matches reference driver

### Priority 3: Verify Buffer Programming Sequence

1. Add logging to `ispvic_frame_channel_qbuf` to show buffer address writes
2. Verify buffer addresses are written before STREAMON
3. Check VIC registers 0x318-0x32c for buffer addresses

### Priority 4: Test with Minimal Configuration

1. Start with single buffer (active_buffer_count = 1)
2. Verify frame done interrupt fires
3. Gradually increase buffer count to 4-5

## Expected Behavior After Fixes

1. Prudynt calls `IMP_FrameSource_EnableChn(0)`
2. libimp.so opens `/dev/framechan0`
3. libimp.so calls REQBUFS for N buffers
4. Our driver allocates VBM buffers
5. libimp.so calls QBUF to queue buffers
6. Our driver writes buffer addresses to VIC registers
7. libimp.so calls STREAMON
8. Our driver enables VIC MDMA and interrupts
9. VIC generates frame done interrupts
10. Our driver processes interrupts and updates buffer ring
11. libimp.so calls DQBUF to get completed frames
12. Prudynt receives frames and streams video

## Next Steps

1. Review current interrupt handler logs to identify which gap is causing stalls
2. Apply fixes in priority order
3. Test after each fix to isolate the root cause
4. Document successful configuration for future reference

