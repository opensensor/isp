# CRITICAL FIX: VIC State Reset Causing 10/15 Stall

## Root Cause Found!

The 10/15 interrupt stall was caused by **`tx_isp_vic_start` being called repeatedly on every interrupt**, not just once during initialization.

## The Problem Sequence

1. **Initial Setup (Correct)**:
   - VIC state: 1 → 2 → 3 → 4 (STREAMING)
   - `tx_isp_vic_start` is called once
   - VIC interrupts start working

2. **State 3 → 4 Transition (BUG TRIGGER)**:
   - `vic_core_s_stream` transitions VIC from state 3 → 4
   - Calls `ispcore_slake_module` to configure silicon bits
   - `ispcore_slake_module` calls `tx_isp_vic_slake_subdev`
   - `tx_isp_vic_slake_subdev` calls `vic_core_s_stream(sd, 0)` to stop streaming
   - This resets VIC state from 4 → 1 ❌

3. **Next Interrupt (BUG MANIFESTS)**:
   - `vic_core_s_stream` is called again
   - Checks: `if (current_state != 4)` → TRUE (state is 1!)
   - Calls `tx_isp_vic_start` again ❌
   - VIC unlock sequence times out (40ms delay)
   - VIC state goes 1 → 2 → 3 → 4
   - Calls `ispcore_slake_module` again
   - State resets to 1 again ❌

4. **Repeat 10 Times**:
   - Each interrupt: state reset → `tx_isp_vic_start` → 40ms delay
   - After 10 interrupts: 400ms of delays
   - System appears to stall

## Evidence from Logs

```
[172.215838] *** tx_isp_vic_start: Following EXACT Binary Ninja flow ***  ← 1st call
[172.278667] *** VIC start completed - vic_start_ok = 1 ***
[173.028837] *** tx_isp_vic_start: Following EXACT Binary Ninja flow ***  ← 2nd call (0.8s later)
[173.062013] *** VIC start completed - vic_start_ok = 1 ***
[174.534914] *** tx_isp_vic_start: Following EXACT Binary Ninja flow ***  ← 3rd call (1.5s later)
...
[179.207090] *** VIC IRQ: vic_start_ok=1, v1_7=0x200000, v1_10=0x0 ***  ← 10th interrupt
[After 179.xxx: NO MORE VIC IRQs!]  ← STALL!
```

**`tx_isp_vic_start` was called 10 times, once per interrupt!**

## The State Reset Chain

```
vic_core_s_stream(enable=1)
  └─> vic_dev->state = 4 (STREAMING)
      └─> ispcore_slake_module(isp_dev)
          └─> tx_isp_vic_slake_subdev(vic_sd)
              └─> vic_core_s_stream(sd, 0)  ← Stops streaming!
                  └─> vic_dev->state = 3 → 2 → 1  ← Resets state!
```

## The Fix

### Part 1: Set `stream_state` Flag Before Calling Slake Module

**File: `driver/tx_isp_vic.c` - Lines 3498-3557**

```c
/* Save VIC state before slake module */
int saved_vic_state = vic_dev->state;

/* CRITICAL FIX: Set stream_state flag to prevent state reset */
vic_dev->stream_state = 1;  /* Signal that we're actively streaming */

/* Call slake module (needed for silicon bits and clocks) */
ispcore_slake_module(ourISPdev);

/* CRITICAL FIX: Restore VIC state after slake module */
if (vic_dev->state != saved_vic_state) {
    pr_info("*** Slake module changed state from %d to %d - RESTORING to %d ***\n",
            saved_vic_state, vic_dev->state, saved_vic_state);
    vic_dev->state = saved_vic_state;
}
```

### Part 2: Check `stream_state` in Slake Subdev

**File: `driver/tx_isp_vic.c` - Lines 3608-3618**

```c
/* CRITICAL FIX: Check if VIC is actively streaming */
if (vic_dev->stream_state == 1) {
    pr_info("*** VIC actively streaming - PRESERVING state %d (NOT resetting to 1) ***\n", 
            vic_dev->state);
    return 0;  /* Return early without resetting state */
}
```

## Why This Works

1. **Before the fix**:
   - `ispcore_slake_module` → `tx_isp_vic_slake_subdev` → resets state to 1
   - Next interrupt: state != 4 → calls `tx_isp_vic_start` again
   - Repeat 10 times → stall

2. **After the fix**:
   - Set `stream_state = 1` before calling `ispcore_slake_module`
   - `tx_isp_vic_slake_subdev` sees `stream_state == 1` → returns early
   - VIC state remains 4 (STREAMING)
   - Next interrupt: state == 4 → no action needed
   - Interrupts continue indefinitely ✓

## Why We Need `ispcore_slake_module`

The slake module is **essential** for:
- Configuring silicon bits for proper hardware operation
- Managing clock transitions
- Setting up ISP core registers
- Coordinating CSI/VIC/ISP pipeline

**We cannot skip it!** We just need to prevent it from resetting VIC state.

## Expected Behavior After Fix

### Before Fix (Broken)
```
[172.xxx] VIC IRQ 1 → tx_isp_vic_start (40ms)
[173.xxx] VIC IRQ 2 → tx_isp_vic_start (40ms)
[174.xxx] VIC IRQ 3 → tx_isp_vic_start (40ms)
...
[179.xxx] VIC IRQ 10 → tx_isp_vic_start (40ms)
[After 179.xxx: NO MORE VIC IRQs!]  ← STALL!
```

### After Fix (Working)
```
[172.xxx] VIC IRQ 1 → tx_isp_vic_start (once, 40ms)
[172.xxx] VIC state 3 → 4 (STREAMING)
[172.xxx] ispcore_slake_module called
[172.xxx] tx_isp_vic_slake_subdev: stream_state=1, PRESERVING state 4
[172.xxx] VIC state remains 4 ✓
[173.xxx] VIC IRQ 2 → state=4, no action needed ✓
[174.xxx] VIC IRQ 3 → state=4, no action needed ✓
[175.xxx] VIC IRQ 4 → state=4, no action needed ✓
...
[Continuous interrupts forever!] ✓
```

## Testing

After rebuilding and loading the module:

```bash
cd /home/matteius/isp-latest/driver
make clean && make
sudo rmmod tx-isp-module 2>/dev/null
sudo insmod tx-isp-module.ko
dmesg | tail -100
```

**Look for in logs:**
```
VIC actively streaming - PRESERVING state 4 (NOT resetting to 1)
vic_core_s_stream: EXACT Binary Ninja - State=4, no action needed
```

**Should NOT see:**
```
tx_isp_vic_start: Following EXACT Binary Ninja flow  ← Should only appear ONCE!
VIC UNLOCK TIMEOUT  ← Should only appear ONCE during initial setup!
```

## Files Modified

- `driver/tx_isp_vic.c` - Lines 3483-3557 (vic_core_s_stream state 4 handling)
- `driver/tx_isp_vic.c` - Lines 3608-3618 (tx_isp_vic_slake_subdev stream_state check)
- `driver/tx-isp-module.c` - Lines 1668-1675 (added rate limiter variables for error handler)

## Summary

The 10/15 stall was caused by `ispcore_slake_module` resetting VIC state from 4 → 1 on every interrupt, causing `tx_isp_vic_start` to be called repeatedly. The fix uses a `stream_state` flag to signal when VIC is actively streaming, preventing the slake module from resetting the state while preserving its essential silicon configuration functionality.

