# Frame Sync Interrupt Callback Fix - Prevent Soft Lockup

## Date: 2025-10-03

## Problem Statement

The system was experiencing soft lockups after ISP driver initialization:

```
[  383.574940] *** system_irq_func_set: Registered handler c0676668 at index 13 ***
[  383.622961] *** VIC IRQ: CORE GATES [9ac0]=0x0 [9ac8]=0x0 ***
[22 second soft lockup, then reboot]
```

The logs showed:
- ISP Core interrupt handler registered at index 13 (0xd)
- VIC interrupts being processed
- Then soft lockup after 22 seconds

## Root Cause Analysis

### The Problem: Unhandled Frame Sync Interrupt

1. **Bit 12 (0x1000) is unmasked** at ISP Core initialization
2. **No callback registered** for bit 12 in the `irq_func_cb[]` array
3. **Frame sync interrupt fires continuously** (every frame at 25 FPS)
4. **Interrupt handler processes it** but finds NULL callback
5. **Interrupt fires again immediately** after being cleared
6. **CPU spends all time in interrupt handler** (thousands of interrupts per second)
7. **Other threads can't run** → soft lockup after 22 seconds

### Why We Need Frame Sync Interrupts

Frame sync interrupts (bit 12) are **essential** for:
- Triggering AE/AWB calculations
- Synchronizing sensor operations with frame timing
- Queuing work for frame processing
- Maintaining frame counters

We **cannot** simply mask this interrupt - we need it to work!

### The Missing Piece

The reference driver registers a callback for bit 12, but our driver was missing this registration:

**Reference driver (from Binary Ninja MCP):**
```c
// tisp_init registers callbacks:
system_irq_func_set(0xc, frame_sync_callback)    // Bit 12 - Frame sync
system_irq_func_set(0xd, ip_done_interrupt_static) // Bit 13 - IP done
system_irq_func_set(0x1b, ae0_interrupt_hist)     // Bit 27 - AE0 histogram
// ... etc
```

**Our driver (before fix):**
```c
// Only registered index 0xd:
system_irq_func_set(0xd, ip_done_interrupt_static)
// Missing: index 0xc for frame sync!
```

## The Fix

### 1. Create Frame Sync Interrupt Callback

Added a new callback function that matches the required signature: `int (*handler)(void)`

**File: driver/tx_isp_core.c** (after line 1751)

```c
/* Frame sync interrupt callback - registered via system_irq_func_set for bit 12 */
/* CRITICAL: This callback is called from interrupt context, must be FAST */
/* Signature MUST match: int (*handler)(void) - NO PARAMETERS */
static int frame_sync_interrupt_callback(void)
{
    /* Queue work to handle frame sync in process context */
    /* This is the same work that's queued in ispcore_interrupt_service_routine */
    if (fs_workqueue) {
        queue_work_on(0, fs_workqueue, &fs_work);
    } else {
        schedule_work_on(0, &fs_work);
    }
    
    /* Return 1 to indicate interrupt was handled */
    return 1;
}
```

**Key points:**
- **Fast execution**: Just queues work and returns
- **No parameters**: Matches `system_irq_func_set` signature
- **Returns 1**: Indicates interrupt was handled
- **Reuses existing work**: Uses the same `fs_work` that was already defined

### 2. Register the Callback

Added callback registration in `tisp_init()` **before** the index 0xd registration.

**File: driver/tx_isp_tuning.c** (lines 2205-2214)

```c
/* CRITICAL: Register frame sync interrupt callback for bit 12 (0x1000) */
/* This prevents soft lockup from unhandled frame sync interrupts */
extern int frame_sync_interrupt_callback(void);

int fs_ret = system_irq_func_set(0xc, frame_sync_interrupt_callback);
if (fs_ret == 0) {
    pr_info("*** tisp_init: Frame sync callback registered (index=0xc, bit 12) ***\n");
} else {
    pr_err("*** tisp_init: Failed to register frame sync callback: %d ***\n", fs_ret);
}
```

### 3. Reduce Logging in Work Function

Changed the frame sync work function logging from `pr_info` to `pr_debug` to reduce console spam:

**File: driver/tx_isp_core.c** (line 1750)

```c
// BEFORE:
pr_info("*** ispcore_irq_fs_work: Frame sync work completed safely ***\n");

// AFTER:
pr_debug("*** ispcore_irq_fs_work: Frame sync work completed safely ***\n");
```

This prevents 25 messages per second (one per frame) from flooding the console.

## How It Works

### Before Fix

```
[Frame sync interrupt fires (bit 12, 0x1000)]
  ↓
[ISP Core interrupt handler called]
  ↓
[Looks up irq_func_cb[12]]
  ↓
[Finds NULL - no callback registered]
  ↓
[Logs "Callback[12] for bit 12 is NULL - skipping"]
  ↓
[Clears interrupt and returns]
  ↓
[Interrupt fires again immediately]
  ↓
[Repeat thousands of times per second]
  ↓
[CPU overloaded → soft lockup]
```

### After Fix

```
[Frame sync interrupt fires (bit 12, 0x1000)]
  ↓
[ISP Core interrupt handler called]
  ↓
[Looks up irq_func_cb[12]]
  ↓
[Finds frame_sync_interrupt_callback]
  ↓
[Calls callback]
  ↓
[Callback queues fs_work to workqueue]
  ↓
[Callback returns 1 (handled)]
  ↓
[Interrupt cleared and returns]
  ↓
[Work processed in process context (not interrupt)]
  ↓
[Next frame: repeat, but work is rate-limited by workqueue]
  ↓
[No soft lockup - system runs normally]
```

## Expected Behavior After Fix

### Kernel Messages

You should see:
```
*** tisp_init: Frame sync callback registered (index=0xc, bit 12) ***
*** tisp_init: ISP processing completion callback registered (index=0xd) ***
```

### No More Soft Lockups

The system should:
- Process frame sync interrupts properly
- Queue work to handle frame processing
- Not overload the CPU with interrupt handling
- Run normally without soft lockups

### Debug Logging

If you need to see frame sync work execution:
```bash
echo 'module tx_isp_t31 +p' > /sys/kernel/debug/dynamic_debug/control
```

This will show:
```
*** ispcore_irq_fs_work: Frame sync work completed safely ***
```

## Files Modified

1. **driver/tx_isp_core.c**
   - Added `frame_sync_interrupt_callback()` function (lines 1753-1767)
   - Changed work function logging to `pr_debug` (line 1750)

2. **driver/tx_isp_tuning.c**
   - Added frame sync callback registration (lines 2205-2214)

## Testing

To rebuild and test:

```bash
# Rebuild the driver
cd /path/to/isp-latest/driver
# (Use your build system)

# On target device:
rmmod tx_isp_t31 2>/dev/null || true
insmod /path/to/tx-isp-t31.ko

# Check that callback is registered:
dmesg | grep "Frame sync callback registered"
# Should see: "*** tisp_init: Frame sync callback registered (index=0xc, bit 12) ***"

# Start streamer
prudynt &

# Monitor for soft lockups (should not occur):
dmesg -w | grep -i "lockup\|hung"

# Check frame sync interrupts are being handled:
dmesg | grep "ISP CORE: ABOUT TO CALL callback\[12\]"
# Should see callbacks being called (if KERN_ALERT logging is enabled)
```

## Why This Is The Correct Fix

1. **Matches reference driver**: Binary Ninja MCP shows reference driver registers callback for bit 12
2. **Handles interrupts properly**: Callback processes the interrupt instead of ignoring it
3. **Fast interrupt handler**: Callback just queues work and returns (no blocking)
4. **Reuses existing infrastructure**: Uses the same `fs_work` that was already defined
5. **Prevents soft lockup**: Interrupt is handled, not ignored
6. **Maintains functionality**: Frame sync interrupts work as intended

## Alternative Approaches Considered

### 1. Mask Bit 12 (REJECTED)
- Set 0xbc/0x98bc to 0x0000 to mask all interrupts
- **Problem**: Loses frame sync functionality
- **Problem**: AE/AWB won't work properly
- **Problem**: Frame timing is lost

### 2. Reduce Logging Only (REJECTED)
- Convert all `printk(KERN_ALERT ...)` to `pr_debug(...)`
- **Problem**: Doesn't fix root cause (unhandled interrupt)
- **Problem**: Interrupt still fires continuously
- **Problem**: CPU still overloaded, just less visible

### 3. Register Callback for Bit 12 (CHOSEN)
- Create proper callback that queues work
- **Advantages**: Matches reference driver, handles interrupt properly
- **Advantages**: Maintains frame sync functionality
- **Advantages**: Fast interrupt handler, work in process context
- **Advantages**: No soft lockup, system runs normally

## Conclusion

The soft lockup was caused by **unhandled frame sync interrupts** (bit 12, 0x1000). The interrupt was unmasked but no callback was registered, causing it to fire continuously and overload the CPU.

The fix is to **register a proper callback** for bit 12 that queues work to handle frame sync in process context. This matches the reference driver behavior and prevents soft lockups while maintaining full frame sync functionality.

This is a **critical correctness fix** that enables proper ISP operation.

