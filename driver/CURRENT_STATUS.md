# ISP Driver Current Status

## Test Results (2025-10-03)

### Automated Test Run
- **Test Directory**: `/tmp/isp_test_current_20251003_150812`
- **ISP Core Interrupts (IRQ 37)**: 5
- **VIC Interrupts (IRQ 38)**: 5
- **Status**: 5/5 stall (same as before)

### Key Observations

1. **Interrupts ARE happening** - `/proc/interrupts` shows 5 counts for both IRQ 37 and 38
2. **No VIC interrupt handler logs** - `dmesg` has NO "VIC IRQ" or "VIC INTERRUPT" messages
3. **QBUF is working** - Userspace is successfully queuing buffers via VBM
4. **Buffer addresses being written** - Logs show `VIC QBUF: Wrote buffer addr=0x76d3000 to reg 0x318`

### Problem Analysis

The lack of VIC interrupt handler logs despite interrupts firing suggests:

**Option A: Log Level Suppression**
- `printk(KERN_ALERT ...)` messages might be suppressed by kernel log level
- The camera's console log level might be set too low
- Solution: Check `/proc/sys/kernel/printk` and increase log level

**Option B: Handler Not Registered**
- The VIC interrupt handler might not be properly registered
- Interrupts are being counted but not calling our handler
- Solution: Verify `request_irq()` succeeded during module load

**Option C: Handler Crashing Silently**
- Handler might be crashing before first printk
- No oops because it's in interrupt context
- Solution: Add very early printk or check for oops in dmesg

## Fixes Applied (Not Yet Tested with Full Logs)

### Fix 1: Force active_buffer_count = 1
**File**: `driver/tx_isp_vic.c` line 2712
```c
vic_dev->active_buffer_count = 1;  /* FORCE to 1 for simplification */
```

### Fix 2: VIC RE-ARM Enabled
**File**: `driver/tx-isp-module.c` line 2148
```c
if (vic_dev && vic_dev->vic_regs) {  /* ENABLED: Was disabled with "if (0 &&" */
```

### Fix 3: VIC RE-ARM Detects Wrong Buffer Count
**File**: `driver/tx-isp-module.c` lines 2152-2158
```c
u32 ctrl_buffer_count = (ctrl >> 16) & 0xF;
u32 expected_count = vic_dev->active_buffer_count;
int wrong_count = (ctrl_buffer_count != expected_count);
if (lost_ctrl || wrong_count) {
    /* Re-write correct buffer count */
}
```

### Fix 4: VIC RE-ARM Skips VIC[0x0] Write
**File**: `driver/tx-isp-module.c` lines 2184-2189
```c
/* CRITICAL FIX: DO NOT write to VIC[0x0] - it clears the buffer count field! */
/* writel(2, vr + 0x0); */  /* DISABLED - clears buffer count */
```

### Fix 5: VIC RE-ARM Writes to Both Register Spaces
**File**: `driver/tx-isp-module.c` lines 2180-2182
```c
writel(new_ctrl, vr + 0x300);
if (vc) writel(new_ctrl, vc + 0x300);  /* Also write to control space */
```

### Fix 6: VIC RE-ARM Readback Verification
**File**: `driver/tx-isp-module.c` lines 2190-2194
```c
u32 readback_ctrl = readl(vr + 0x300);
u32 readback_count = (readback_ctrl >> 16) & 0xF;
printk(KERN_ALERT "*** VIC RE-ARM: Wrote 0x%x to VIC[0x300], readback=0x%x (count=%u) ***\n", 
       new_ctrl, readback_ctrl, readback_count);
```

### Fix 7: Gate Enable Prerequisites
**File**: `driver/tx-isp-module.c` lines 1878-1882
```c
/* CRITICAL: Write prerequisite registers BEFORE gate enable! */
writel(0x1, core + 0x9a34);   /* enable bit */
writel(0x1, core + 0x9a88);   /* enable/route latch bit */
wmb();
```

## Next Steps for User

### Step 1: Check Log Level
```bash
ssh root@192.168.50.211
cat /proc/sys/kernel/printk
# Should show something like: 7 4 1 7
# If first number < 7, increase it:
echo "7 4 1 7" > /proc/sys/kernel/printk
```

### Step 2: Check Handler Registration
Look in dmesg during module load for:
```
*** system_irq_func_set: Registered handler c0660f68 at index 12 ***
*** system_irq_func_set: Registered handler c0674ec0 at index 13 ***
```

If these appear, handlers are registered correctly.

### Step 3: Manual Test with Increased Log Level
```bash
# On camera:
echo "7 4 1 7" > /proc/sys/kernel/printk
dmesg -c  # Clear dmesg
prudynt &
sleep 5
dmesg | grep -i "VIC IRQ\|VIC RE-ARM\|control limit"
```

### Step 4: Check for Kernel Oops
```bash
dmesg | grep -i "oops\|panic\|bug\|unable to handle"
```

## Expected Behavior After Fixes

If the VIC RE-ARM code is working correctly, we should see:

1. **First VIC interrupt**:
   ```
   *** VIC IRQ: CTRL[0x300]=0x80000020 ***  (buffer_count=0)
   *** VIC RE-ARM: reason=wrong_buffer_count ctrl=0x80000020 (buf_count=0, expected=1) -> reassert, stream_ctrl=0x80010020 (count=1) ***
   *** VIC RE-ARM: Wrote 0x80010020 to VIC[0x300], readback=0x80010020 (count=1) ***
   ```

2. **Subsequent VIC interrupts**:
   ```
   *** VIC IRQ: CTRL[0x300]=0x80010020 ***  (buffer_count=1, correct!)
   *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
   ```

3. **No more control limit errors** after the first RE-ARM

4. **Continuous interrupts** beyond 5/5

## Files Modified

- `driver/tx_isp_vic.c` - Force active_buffer_count=1, skip VIC[0x0] write
- `driver/tx-isp-module.c` - Enable VIC RE-ARM, detect wrong buffer count, skip VIC[0x0] write, add readback
- `driver/test_current.sh` - Automated test script
- `driver/GATE_ENABLE_FIX.md` - Documentation of gate enable fix
- `driver/SINGLE_BUFFER_SIMPLIFICATION.md` - Documentation of single-buffer mode

## Build Status

✅ **Build completed successfully** (2025-10-03 15:06:28)

All kernel modules compiled without errors and uploaded to camera at `/opt/`.

