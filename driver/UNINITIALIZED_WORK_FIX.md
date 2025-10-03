# Software Lockup Fix - Uninitialized Work Structure

## Date: 2025-10-03

## Summary

**CRITICAL BUG FOUND AND FIXED**: The soft lockup 22 seconds after VIC interrupt was caused by queuing an uninitialized work structure (`fs_work`) instead of the properly initialized one (`ispcore_fs_work`).

## Problem

The system was experiencing a software lockup 22 seconds after the VIC interrupt fired successfully. The logs showed:

```
[26349.242447] *** isp_irq_handle: IRQ 37, dev_id=81160000 ***
[26349.248194] *** ISP CORE INTERRUPT HANDLER: IRQ 37 called, dev_id=81160000 ***
[26349.255643] *** ISP CORE: Read interrupt status - interrupt_status=0x00001000 ***
[26349.263361] *** ISP CORE: Clearing legacy interrupt 0x00001000 to reg +0xb8 ***
[26349.270898] *** ISP CORE: After clearing - legacy=0x00000000 ***
[26349.277099] *** ISP CORE: About to process IRQ callbacks - interrupt_status=0x1000 ***
[26349.285260] *** ISP CORE: ABOUT TO CALL callback[12] for bit 12 - callback=c0661c88 ***
[26349.293513] *** ISP CORE: CALLING CALLBACK NOW - IF SYSTEM HANGS, THIS CALLBACK IS THE PROBLEM ***
[26349.302764] *** ISP CORE: CALLBACK RETURNED SUCCESSFULLY - callback[12] returned 1 ***
```

The interrupt was firing and the callback was returning successfully, but the system would lock up 22 seconds later.

## Root Cause

There were **two different work structures** declared in `tx_isp_core.c`:

1. **`fs_work`** (line 68) - declared but **never initialized**
2. **`ispcore_fs_work`** (line 1652) - properly initialized with `INIT_WORK`

The `frame_sync_interrupt_callback()` function (registered for bit 12 interrupts) was queuing `fs_work`:

```c
int frame_sync_interrupt_callback(void)
{
    if (fs_workqueue) {
        queue_work_on(0, fs_workqueue, &fs_work);  // ← WRONG! fs_work is uninitialized
    } else {
        schedule_work_on(0, &fs_work);  // ← WRONG! fs_work is uninitialized
    }
    return 1;
}
```

However, the initialization code was initializing `ispcore_fs_work`:

```c
INIT_WORK(&ispcore_fs_work, ispcore_irq_fs_work);  // ← Initializing the WRONG structure
```

When the kernel tried to execute an uninitialized work structure, it would cause undefined behavior leading to a kernel panic or soft lockup.

## The Fix

Changed `frame_sync_interrupt_callback()` to use the properly initialized `ispcore_fs_work`:

```c
int frame_sync_interrupt_callback(void)
{
    /* CRITICAL FIX: Use ispcore_fs_work (which is initialized) instead of fs_work (uninitialized) */
    if (fs_workqueue) {
        queue_work_on(0, fs_workqueue, &ispcore_fs_work);  // ← FIXED
    } else {
        schedule_work_on(0, &ispcore_fs_work);  // ← FIXED
    }
    return 1;
}
```

Also fixed other references to `&fs_work` in the ISP core interrupt handler (lines 1935, 1937, 1945).

Removed the unused `fs_work` declaration to prevent future confusion:

```c
/* Frame sync work queue - CRITICAL for sensor I2C communication */
static struct workqueue_struct *fs_workqueue = NULL;
/* REMOVED: static struct work_struct fs_work; - was duplicate/uninitialized, use ispcore_fs_work instead */
static void ispcore_irq_fs_work(struct work_struct *work);
```

## Files Modified

- `driver/tx_isp_core.c`:
  - Line 68: Removed `fs_work` declaration (replaced with comment)
  - Line 1758: Changed `frame_sync_interrupt_callback()` to use `&ispcore_fs_work`
  - Line 1935: Changed debug print to reference `&ispcore_fs_work`
  - Line 1937: Changed `queue_work_on()` to use `&ispcore_fs_work`
  - Line 1945: Changed `schedule_work_on()` to use `&ispcore_fs_work`

## Testing

Build the driver and test on the device. The soft lockup should no longer occur after the VIC interrupt fires.

## Why This Happened

This was a classic case of having duplicate variable names with similar purposes:
- `fs_work` was likely an early declaration that was never properly initialized
- `ispcore_fs_work` was added later as the "correct" implementation
- The callback registration was using the old, uninitialized variable
- The initialization code was initializing the new variable

The mismatch between what was initialized and what was used caused the lockup.

## Lesson Learned

When working with kernel work queues:
1. Always ensure work structures are initialized with `INIT_WORK()` before use
2. Avoid duplicate variable names for similar purposes
3. Grep for all references to a work structure when making changes
4. Uninitialized work structures cause undefined behavior and hard-to-debug lockups

