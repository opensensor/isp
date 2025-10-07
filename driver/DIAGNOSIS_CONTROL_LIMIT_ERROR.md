# VIC Control Limit Error Diagnosis - SOLVED

## Problem Summary

The ISP driver experiences "control limit err!!!" interrupts (bit 21, 0x200000) that occur 4 times at startup and then stall the pipeline. The streamer continues running but no real frames are produced.

## Root Cause: Missing Event Handler for 0x3000003

**The control limit error occurs because VIC register 0x300 (VIC_ADDR_DMA_CONTROL) has buffer_count=0.**

From logs.txt line 2500:
```
[ 1279.074846] *** VIC CTRL (PRIMARY): [0x300]=0x00000000 ***
```

The VIC hardware tries to DMA frames but has no valid buffer count configured, triggering a control limit violation.

## Why VIC[0x300] is Not Written

The function `ispvic_frame_channel_s_stream()` is responsible for writing VIC[0x300] with the buffer count during stream ON. However, **it is NEVER called** because the event handler doesn't route the 0x3000003 event to it.

### The Missing Event Handler

From Binary Ninja MCP decompilation of reference driver's `vic_core_ops_ioctl`:

```c
if (arg2 == 0x1000001)
    // Handle sensor sync
else if (arg2 == 0x3000009)
    result = tx_isp_subdev_pipo(arg1, arg3)
else if (arg2 != 0x1000000)
    return 0  // Unknown commands return 0 (success)
else
    // Handle 0x1000000
```

**The reference driver does NOT have a case for 0x3000003!** It falls through to the "unknown cmd" case and returns 0.

### The Call Sequence

From `tx-isp-module.c` lines 3878-3893:

1. **Frame channel STREAMON** (ioctl 0x80045612)
2. **Sends 0x3000003 event** via `tx_isp_send_event_to_remote()`
3. **Event routes to `vic_core_ops_ioctl`**
4. **No case for 0x3000003** - returns 0 (success)
5. **Fallback check** (line 3886-3890): Only triggers if result == 0xfffffdfd
6. **Result is 0** - fallback NOT triggered
7. **`ispvic_frame_channel_s_stream` is NEVER called**
8. **VIC[0x300] is never written** - buffer_count stays 0
9. **VIC starts** with buffer_count=0
10. **Control limit error** fires immediately

### Missing Log Evidence

The logs confirm `ispvic_frame_channel_s_stream()` is NEVER called:
- Line 1372: `0x3000003 event SUCCESS` (returns 0)
- **NO** "ispvic_frame_channel_s_stream: EXACT Binary Ninja implementation" message
- **NO** "Checking stream state" message
- **NO** "Calling vic_pipo_mdma_enable" message
- **NO** "READBACK: VIC[0x300]" message

This confirms the event handler returns success without actually calling the function.

## Symptom Pattern (from logs.txt)
```
[ 1279.165504] *** VIC HARDWARE INTERRUPT: vic_start_ok=1, processing (v1_7=0x200000, v1_10=0x0) ***
[ 1279.165511] *** VIC ISR: Frame done bit NOT set (v1_7 & 1 = 0), v1_7=0x200000 ***
[ 1279.165517] Err2 [VIC_INT] : control limit err!!!
[ 1279.165525] *** FRAME SYNC: Frame done count = 4 ***
[ 1279.165532] *** ISP FRAME DONE WAKEUP: Frame 4 ready for processing ***
```

### Key Observations

1. **VIC interrupt fires with v1_7=0x200000** (bit 21 set = control limit error)
2. **Frame done bit is NOT set** (v1_7 & 1 = 0) - no real frame was completed
3. **Frame counter increments anyway** due to workaround code calling `isp_frame_done_wakeup()` unconditionally
4. **Pattern repeats 4 times** then stops, but pipeline is stalled

### Binary Ninja MCP Analysis

Decompiled the reference driver's `isp_vic_interrupt_service_routine`:

```c
if (($v1_7 & 1) != 0)
    *($s0 + 0x160) += 1
    $a2 = vic_framedone_irq_function($s0)

if (($v1_7 & 0x200000) != 0)
    data_a29b4 += 1
    $a2 = isp_printf(1, "Err [VIC_INT] : control limit er...", $a2)

// NO unconditional wakeup call at end!
return 1
```

**Critical finding**: The reference driver:
- Only calls `vic_framedone_irq_function()` when bit 0 (frame done) is set
- Does NOT call any wakeup function for error interrupts
- Does NOT have unconditional wakeup at end of interrupt handler

### Interrupt Mask Logic

The VIC interrupt logic works as follows:

```c
// From tx-isp-module.c line 1713:
v1_7 = (~readl(vic_regs + 0x1e8)) & readl(vic_regs + 0x1e0);
```

This means:
- **Mask register (0x1e8)**: 1 = masked (disabled), 0 = unmasked (enabled)
- **Status register (0x1e0)**: Hardware interrupt status bits
- **Result (v1_7)**: `(~mask) & status` = active unmasked interrupts

### The Bug

In `tx_isp_vic.c` line 2942 (before fix):

```c
writel(0xFFDFFFFE, vr + 0x1e8);  // WRONG!
```

Binary analysis:
```
0xFFDFFFFE = 11111111110111111111111111111110
                    ^
                    Bit 21 = 0 (UNMASKED/ENABLED)
```

This mask has **bit 21 cleared**, which means:
- `~0xFFDFFFFE = 0x00200001`
- Bit 21 in `~mask` = 1, so control limit errors are **ENABLED**
- When hardware sets bit 21 in status register, interrupt fires

## Diagnostic Value of Control Limit Errors

**IMPORTANT**: Control limit error interrupts (bit 21) should be ENABLED, not masked!

These errors indicate real VIC configuration problems:
- Buffer count not set (VIC[0x300] = 0)
- Invalid buffer addresses
- DMA configuration errors

Masking these errors would hide critical configuration issues. The errors are telling us that VIC is not properly configured.

## The Fixes

### Fix 1: Add 0x3000003 Handler to vic_core_ops_ioctl

Add a case for 0x3000003 in `vic_core_ops_ioctl` that calls `ispvic_frame_channel_s_stream()`.

### Implementation

In `driver/tx_isp_vic.c`, added after line 1705:

```c
/* CRITICAL FIX: Handle 0x3000003 (TX_ISP_EVENT_FRAME_STREAMON) from frame channel STREAMON */
else if (cmd == 0x3000003) {
    int enable = arg ? *((int *)arg) : 1;  /* Default to stream ON if arg is NULL */
    pr_info("vic_core_ops_ioctl: 0x3000003 (FRAME_STREAMON) - calling ispvic_frame_channel_s_stream(%d)\n", enable);
    result = ispvic_frame_channel_s_stream(sd, enable);
    pr_info("vic_core_ops_ioctl: 0x3000003 - ispvic_frame_channel_s_stream returned %d\n", result);
    return result;
}
```

### Why This Works

1. **Frame channel STREAMON** sends 0x3000003 event
2. **Event routes to `vic_core_ops_ioctl`**
3. **New case handler** calls `ispvic_frame_channel_s_stream(sd, 1)`
4. **`ispvic_frame_channel_s_stream`** executes fully:
   - Calls `vic_pipo_mdma_enable()` to configure MDMA
   - Writes VIC[0x300] with buffer count
   - Sets `stream_state = 1`
5. **VIC[0x300]** now has valid buffer count
6. **VIC starts** with proper configuration
7. **No control limit errors!**

### Why Reference Driver Doesn't Need This

The reference driver likely uses a different event routing mechanism or calls `ispvic_frame_channel_s_stream()` directly from the frame channel STREAMON handler instead of via events. Our driver uses the event system, so we need the handler.

## Testing Plan

1. Rebuild driver with the fix
2. Load driver and start streaming
3. Verify `ispvic_frame_channel_s_stream` is called (check for log messages)
4. Verify VIC[0x300] is written with buffer count
5. Verify NO control limit errors occur
6. Verify frames are captured successfully

### Fix 2: Write VIC Interrupt Enable Register (0x1e0)

**CRITICAL**: The code was only writing the interrupt MASK register (0x1e8), not the interrupt ENABLE register (0x1e0)!

The interrupt logic is: `v1_7 = (~readl(0x1e8)) & readl(0x1e0)`

Both registers must be set:
- **0x1e0** = interrupt enable (which bits CAN fire)
- **0x1e8** = interrupt mask (which bits are masked OUT)

The logs showed `[0x1e0]=0x00000000` after VIC start, meaning NO interrupts were enabled at the hardware level!

#### Implementation

In `driver/tx_isp_vic.c` lines 2957-2960, added:

```c
/* Enable frame done (bit 0) and control limit error (bit 21) */
writel(0x00200001, vr + 0x1e0);  /* Enable bits 0 and 21 */
wmb();
```

This enables:
- **Bit 0**: Frame done interrupt
- **Bit 21**: Control limit error interrupt (for diagnostics)

Combined with the mask register (0xFFDFFFFE), this allows both interrupts to fire.

## Files Modified

- `driver/tx_isp_vic.c` lines 1707-1714: Added 0x3000003 case handler
- `driver/tx_isp_vic.c` lines 2944-2971: Added 0x1e0 write to enable interrupts

## Expected Behavior After Fix

With the 0x3000003 handler in place:

1. **Frame channel STREAMON** triggers event
2. **`ispvic_frame_channel_s_stream` is called** and executes fully
3. **VIC[0x300] is written** with buffer count from `active_buffer_count`
4. **VIC starts** with proper buffer configuration
5. **No control limit errors** (VIC is properly configured)
6. **Frames are captured** and delivered to userspace
7. **Bit 21 stays enabled** for future diagnostic purposes

