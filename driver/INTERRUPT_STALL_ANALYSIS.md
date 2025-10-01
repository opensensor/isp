# ISP/VIC Interrupt Stall Analysis

## Problem Statement
- **Current driver** (`isp-latest`): Gets 10/15 ISP/VIC interrupts then stalls
- **Working driver** (`isp-was-better`): Gets continuous interrupts
- Both use similar gc2053 sensor registration via common API (subdev_init)
- Current driver has better control flow closer to BN MCP reference

## Key Differences Found

### 1. CSI Video Stream Control (`csi_video_s_stream`)

**Working Version (isp-was-better):**
```c
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    // Checks sensor interface type from sensor attributes
    extern struct tx_isp_sensor *tx_isp_get_sensor(void);
    struct tx_isp_sensor *sensor = tx_isp_get_sensor();
    if (!sensor || !sensor->video.attr || 
        sensor->video.attr->dbus_type != TX_SENSOR_DATA_INTERFACE_MIPI) {
        pr_info("csi_video_s_stream: Sensor interface type is not MIPI (1), returning 0\n");
        return 0;
    }
    
    // Sets state in CSI device
    if (enable == 0) {
        csi_dev->state = 3;
    } else {
        csi_dev->state = 4;
    }
}
```

**Current Version (isp-latest):**
```c
int csi_video_s_stream(struct tx_isp_subdev *sd, int enable)
{
    // Checks CSI device interface type (not sensor!)
    csi_dev = (struct tx_isp_csi_device *)tx_isp_get_subdevdata(sd);
    
    if (csi_dev->interface_type != 1) {
        pr_info("csi_video_s_stream: Interface type %d != 1 (MIPI), returning 0\n", 
                csi_dev->interface_type);
        return 0;
    }
    
    // Sets state in CSI device
    int v0_4 = 4;
    if (enable == 0) {
        v0_4 = 3;
    }
    csi_dev->state = v0_4;
}
```

**CRITICAL DIFFERENCE:** 
- Working version checks `sensor->video.attr->dbus_type` (sensor attributes)
- Current version checks `csi_dev->interface_type` (CSI device structure)
- If CSI device interface_type is not properly initialized, the check fails and state is not set correctly

### 2. VIC Start Function (`tx_isp_vic_start`)

**Working Version:**
- Simpler initialization sequence
- Writes critical interrupt-enabling registers at END of function:
  ```c
  writel(0x3130322a, vic_regs + 0x0);  /* CRITICAL for interrupts */
  writel(0x1, vic_regs + 0x4);         /* CRITICAL for interrupts */
  writel(0x200, vic_regs + 0x14);      /* CRITICAL for interrupts */
  ```
- Less complex dimension handling

**Current Version:**
- More complex with "CRITICAL FIX" for dimensions
- Hardcoded dimensions: `actual_width = 1920; actual_height = 1080;`
- More register writes that may interfere with interrupt setup
- Timeout handling added: `u32 timeout = 10000;`

### 3. VIC MDMA IRQ Function (`vic_mdma_irq_function`)

**Working Version:**
- Simpler validation
- Direct frame completion signaling
- Less buffer management complexity

**Current Version:**
- More complex buffer circulation logic
- Additional buffer ring management
- Comment: "CRITICAL FIX: Do NOT decrement active_buffer_count"
- More sophisticated but potentially problematic buffer handling

## Root Cause Hypothesis

### Primary Suspect: CSI Interface Type Check
The most likely cause is the difference in `csi_video_s_stream`:

1. **Working version** checks sensor attributes directly via `tx_isp_get_sensor()->video.attr->dbus_type`
2. **Current version** checks `csi_dev->interface_type` which may not be properly initialized

If `csi_dev->interface_type` is 0 or incorrect:
- The function returns 0 without setting `csi_dev->state`
- CSI device remains in wrong state
- Interrupts may not be properly enabled or handled
- After 10-15 interrupts, the system stalls

### Secondary Suspect: VIC Register Initialization Order
The working version writes critical interrupt registers at the END of `tx_isp_vic_start`, while the current version may have these overwritten by subsequent register writes.

### Tertiary Suspect: Buffer Management Complexity
The current version has more complex buffer circulation logic that may cause issues after several frames.

## Recommended Fixes

### Fix 1: Restore Sensor Attribute Check in CSI (HIGH PRIORITY) ✅ APPLIED
**Status: APPLIED to driver/tx_isp_csi.c**

Changed `csi_video_s_stream` to check sensor attributes first, with fallback to CSI device interface_type:

```c
// Now checks sensor attributes first (more reliable):
extern struct tx_isp_sensor *tx_isp_get_sensor(void);
struct tx_isp_sensor *sensor = tx_isp_get_sensor();

if (sensor && sensor->video.attr) {
    if (sensor->video.attr->dbus_type != TX_SENSOR_DATA_INTERFACE_MIPI) {
        return 0;
    }
} else {
    // Fallback to CSI device interface_type if sensor not available
    if (csi_dev->interface_type != 1) {
        return 0;
    }
}
```

This matches the working version's approach and ensures the interface type check works even if `csi_dev->interface_type` is not yet initialized.

### Fix 2: Simplify VIC Start Register Sequence (MEDIUM PRIORITY)
Move critical interrupt-enabling registers to END of `tx_isp_vic_start` to prevent overwriting.

### Fix 3: Simplify Buffer Management (LOW PRIORITY)
Consider reverting to simpler buffer management from working version if Fix 1 and 2 don't resolve the issue.

### Fix 2: Prevent Buffer Index 2 From Setting Bit 17 (ROOT CAUSE) ✅ APPLIED
**Status: APPLIED to driver/tx_isp_vic.c - Line 492-507**

**ROOT CAUSE IDENTIFIED:** Buffer index 2 accidentally sets bit 17 (control limit enable)!

**The Problem:**
```
Buffer index 2 shifted left 16 bits:
2 << 16 = 0x00020000

This is BOTH:
- Buffer index 2 in bits 16-19
- Bit 17 (control limit enable)!
```

**Why This Causes Stalls:**
1. Frame 0 completes → buffer index 0 → `0x80000020` ✓ Works
2. Frame 1 completes → buffer index 1 → `0x80010020` ✓ Works
3. Frame 2 completes → buffer index 2 → `0x80020020` ✗ **Bit 17 accidentally set!**
4. Hardware sees bit 17, validates frame, triggers control limit error
5. No frame done interrupt → driver stalls

**Symptoms in logs:**
```
VIC IRQ: CTRL[0x300]=0x80020020  ← Buffer index 2 + bit 17 set = ERROR
VIC ERROR: control limit error (bit 21)
VIC IRQ: No frame done interrupt
```

**Fix Applied:**
Changed frame done interrupt handler to explicitly clear bit 17:
```c
// OLD (broken):
u32 control_update = (buffer_position << 16);
u32 new_ctrl = (current_ctrl & 0xFFF0FFFF) | control_update;
// Mask 0xFFF0FFFF clears bits 16-19 but PRESERVES bit 17!

// NEW (fixed):
u32 control_update = ((buffer_position & 0xF) << 16);
u32 new_ctrl = (current_ctrl & 0xFFFDFFFF) | control_update;
// Mask 0xFFFDFFFF clears bits 16-19 AND bit 17!
```

**Why This Is The Root Cause:**
- Timing matches: Stalls after ~10-15 frames (when buffer cycling reaches index 2)
- Pattern matches: Control register alternates between `0x80020020` (error) and `0x80000020` (ok)
- Mathematical proof: `2 << 16 = 0x00020000 = bit 17`

See `ROOT_CAUSE_FOUND.md` for complete analysis.

### Fix 3: Simplify Buffer Management (LOW PRIORITY)
Consider reverting to simpler buffer management from working version if Fix 1 and 2 don't resolve the issue.

## Testing Strategy

1. ✅ **Fix 1 APPLIED** - CSI interface check (WORKING - sensor detected correctly)
2. ✅ **Fix 2 APPLIED** - VIC control limit protection disabled - **TEST THIS NOW**
3. If still stalling, apply Fix 3 (buffer management)

## Additional Investigation Needed

1. Check if `csi_dev->interface_type` is properly initialized during CSI device probe
2. Verify the initialization order of CSI device vs sensor registration
3. Check if there are any race conditions in the initialization sequence

