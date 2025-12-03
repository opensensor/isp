# Frame Sync Workqueue Crash Fix

## Problem

The driver was crashing with a "Reserved instruction in kernel code" error when processing frame sync interrupts:

```
[   68.102071] Reserved instruction in kernel code[#1]:
[   68.116264] Workqueue: events 0xc05d2154
[   68.174963] epc   : c05d223c 0xc05d223c
```

### Root Cause

The crash occurred because:

1. **`fs_workqueue` was NULL** - The workqueue creation in `tx_isp_core_probe()` was happening late in the probe function (after VIN device creation)
2. **Early returns prevented workqueue creation** - If any of these failed, the workqueue was never created:
   - Tuning initialization (line 3616)
   - Memory mappings (line 3620)
   - VIN device creation (line 3628)
3. **Dangerous fallback code** - When `fs_workqueue` was NULL, the interrupt handler fell back to:
   ```c
   schedule_work_on(0, &fs_work);  // Using system workqueue
   ```
4. **Uninitialized work structure** - The `fs_work` structure was only initialized with `INIT_WORK()` when the dedicated workqueue was created, so when using the system workqueue, `fs_work` contained garbage data
5. **Crash on execution** - The kernel tried to execute the work function pointer from the uninitialized structure, leading to the "Reserved instruction" crash

## Solution

### 1. Move Workqueue Creation Early

Moved the workqueue creation to the very beginning of `tx_isp_core_probe()`, right after basic device initialization (line 3484-3496):

```c
/* CRITICAL: Initialize frame sync work queue EARLY - MUST be done before ANY interrupts can occur */
printk(KERN_ALERT before any interrupt setup) ***\n");
fs_workqueue = create_singlethread_workqueue("isp_frame_sync");
if (!fs_workqueue) {
    printk(KERN_ALERT "*** tx_isp_core_probe: CRITICAL - Failed to create frame sync workqueue ***\n");
    return -ENOMEM;
}
printk(KERN_ALERT "*** tx_isp_core_probe: Frame sync workqueue created successfully at %p ***\n", fs_workqueue);

/* Initialize the work structure */
INIT_WORK(&fs_work, ispcore_irq_fs_work);
printk(KERN_ALERT "*** tx_isp_core_probe: Frame sync work initialized at %p ***\n", &fs_work);
printk(KERN_ALERT "*** tx_isp_core_probe: Frame sync work queue READY - safe to enable interrupts ***\n");
```

**Benefits:**
- Workqueue is created before any code path that might enable interrupts
- No early returns can prevent workqueue creation
- Work structure is always properly initialized

### 2. Remove Dangerous System Workqueue Fallback

Replaced the dangerous fallback code in the interrupt handler (line 867-883):

**Before:**
```c
if (fs_workqueue) {
    queue_work_on(0, fs_workqueue, &fs_work);
} else {
    printk(KERN_ALERT "*** ISP CORE: fs_workqueue is NULL - using system workqueue ***\n");
    schedule_work_on(0, &fs_work);  // DANGEROUS - fs_work not initialized!
}
```

**After:**
```c
/* CRITICAL: Only queue work if workqueue is properly initialized */
if (!fs_workqueue) {
    printk(KERN_ALERT "*** ISP CORE: CRITICAL ERROR - fs_workqueue is NULL! Cannot process frame sync interrupt! ***\n");
    printk(KERN_ALERT "*** ISP CORE: This indicates the workqueue was not created during probe or init ***\n");
    /* Still acknowledge the interrupt to prevent interrupt storm */
    goto acknowledge_interrupt;
}

printk(KERN_ALERT "*** ISP CORE: fs_workqueue=%p, fs_work=%p ***\n", fs_workqueue, &fs_work);
queue_work_on(0, fs_workqueue, &fs_work);

acknowledge_interrupt:
```

**Benefits:**
- No dangerous fallback to system workqueue with uninitialized work structure
- Clear error message if workqueue is NULL (should never happen now)
- Interrupt is still acknowledged to prevent interrupt storms

### 3. Add Verification in Core Init

Added verification in `ispcore_core_ops_init()` (line 1818-1825):

```c
/* NOTE: Frame sync work structure already initialized early in probe function */
/* Verify it's initialized */
if (!fs_workqueue) {
    printk(KERN_ALERT "*** ispcore_core_ops_init: CRITICAL - fs_workqueue is NULL! ***\n");
    printk(KERN_ALERT "*** This should never happen - workqueue should be created in probe ***\n");
    return -EINVAL;
}
printk(KERN_ALERT "*** ispcore_core_ops_init: Frame sync workqueue verified: %p ***", fs_workqueue);
```

**Benefits:**
- Early detection if workqueue wasn't created
- Prevents enabling interrupts without a working workqueue

### 4. Remove Duplicate Initialization

Removed duplicate workqueue creation code that was later in the probe function (previously at line 3647-3663).

## Testing

After this fix:
1. The workqueue should be created early and always be available
2. No crashes should occur from uninitialized work structures
3. Frame sync interrupts should be processed correctly
4. Clear error messages if something goes wrong

## Files Modified

- `driver/tx_isp_core.c`:
  - Line 3484-3496: Early workqueue creation
  - Line 867-883: Removed dangerous fallback
  - Line 1818-1825: Added verification
  - Removed duplicate initialization code

