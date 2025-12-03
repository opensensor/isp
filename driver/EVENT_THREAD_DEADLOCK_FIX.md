# Event Thread Deadlock Fix

## Problem

The system would hang for approximately 22 seconds during ISP STREAMON and then reboot. The hang occurred when userspace called `VIDIOC_STREAMON` on the ISP device.

### Symptoms

```
[IMP_ISP] EnableSensor: calling ioctl 0x80045612 (ISP STREAMON)
<system hangs for ~22 seconds, then reboots>
```

## Root Cause

The ISP tuning subsystem has an event processing thread (`tisp_event_thread`) that waits for events using a completion variable (`tevent_info`). The thread was deadlocking because:

1. **Event thread starts** in `tisp_init()` via `kthread_run(tisp_event_process_thread, ...)`
2. **Thread waits** on `wait_for_completion_interruptible(&tevent_info)` in `tisp_event_process()`
3. **Completion is consumed** when `complete(&tevent_info)` is called from `tisp_event_push()`
4. **Next iteration blocks forever** because the completion was not reinitialized

### The Completion API Issue

Linux completion API behavior:
- `init_completion()` - Initialize completion to "not completed" state
- `complete()` - Mark completion as "completed", wake up one waiter
- `wait_for_completion_interruptible()` - Wait until completed, **consumes the completion**
- `reinit_completion()` - Reset completion back to "not completed" state

**The bug**: After the first `wait_for_completion_interruptible()` returns, the completion is consumed. The next call to `wait_for_completion_interruptible()` will block forever unless someone calls `complete()` again OR the completion is reinitialized.

### Why It Caused a 22-Second Hang

The watchdog timer was likely set to ~22 seconds, causing the system to reboot when the event thread blocked indefinitely during STREAMON.

## Solution

1. Initialize the completion **once** in `tisp_event_init()` during system initialization
2. Add `INIT_COMPLETION(tevent_info)` before each `wait_for_completion_interruptible()` call to reset the completion to the "not completed" state
3. Remove duplicate initialization code from `tisp_event_push()` and `tisp_event_process()`

**Note**: Kernel 3.10 uses `INIT_COMPLETION()` macro instead of `reinit_completion()` which was introduced in kernel 3.13.

**Critical Bug Fixed**: The original code had TWO separate static `tevent_initialized` variables in both `tisp_event_push()` and `tisp_event_process()`, creating a race condition. The completion is now initialized exactly once in `tisp_event_init()`.

### Code Changes

**File: `driver/tx_isp_tuning.c`**

**Change 1: Initialize completion in tisp_event_init() (line 12463-12483)**
```c
int tisp_event_init(void)
{
    printk(KERN_ALERT "tisp_event_init: Initializing ISP event system\n");

    /* Clear all callback arrays */
    memset(isp_event_func_cb, 0, sizeof(isp_event_func_cb));
    memset(cb, 0, sizeof(cb));

    if (!isp_irq_initialized) {
        spin_lock_init(&isp_irq_lock);
        isp_irq_initialized = true;
    }

    /* CRITICAL: Initialize event completion ONCE at system init */
    init_completion(&tevent_info);
    printk(KERN_ALERT "tisp_event_init: Event completion initialized\n");

    printk(KERN_ALERT "tisp_event_init: Event system initialized\n");
    return 0;
}
```

**Change 2: Remove duplicate init from tisp_event_push() (line 2148-2159)**
```c
static int tisp_event_push(void *event)
{
    /* Event push implementation - adds event to ISP event queue */
    if (!event) {
        return -EINVAL;
    }

    /* Signal event completion - completion is initialized in tisp_event_init() */
    printk(KERN_ALERT "Event pushed\n");
    complete(&tevent_info);
    return 0;
}
```

**Change 3: Simplify tisp_event_process() (line 12492-12500)**
```c
printk(KERN_ALERT "tisp_event_process: Starting event processing loop\n");

/* CRITICAL FIX: Reinitialize completion before waiting to prevent deadlock
 * Kernel 3.10 uses INIT_COMPLETION instead of reinit_completion
 * Completion is initialized once in tisp_event_init() */
INIT_COMPLETION(tevent_info);

/* Wait for event notification */
ret = wait_for_completion_interruptible(&tevent_info);
```

## Event Thread Flow

### Normal Operation (After Fix)

1. **System initialization**: `tisp_event_init()` called during module init
   - Calls `init_completion(&tevent_info)` exactly once
   - Completion is now ready for use

2. **Thread starts**: `tisp_event_process_thread()` runs in a loop

3. **Each iteration**:
   - Calls `tisp_event_process()`
   - Calls `INIT_COMPLETION(tevent_info)` to reset completion state (kernel 3.10)
   - Waits on `wait_for_completion_interruptible(&tevent_info)`
   - When event arrives, `tisp_event_push()` calls `complete(&tevent_info)`
   - Thread wakes up, processes event
   - Loop continues

### Event Sources

Events are pushed to the thread via `tisp_event_push()` which is called from:
- ISP interrupt handlers
- Sensor attribute sync operations
- Frame processing callbacks
- AE/AWB algorithm updates

## Related Code

### Event Thread Creation
**File: `driver/tx_isp_tuning.c`, line 2530**
```c
tisp_event_thread = kthread_run(tisp_event_process_thread, NULL, "tisp_events");
```

### Event Push (Completion Signal)
**File: `driver/tx_isp_tuning.c`, line 2163**
```c
complete(&tevent_info);
```

### Event Thread Loop
**File: `driver/tx_isp_tuning.c`, line 12527**
```c
while (!kthread_should_stop()) {
    int ret = tisp_event_process();
    
    if (ret < 0) {
        if (ret == -ERESTARTSYS) {
            printk(KERN_ALERT "tisp_event_process_thread: Thread interrupted, continuing\n");
            continue;
        } else {
            printk(KERN_ALERT "tisp_event_process_thread: Event processing error: %d\n", ret);
            msleep(100); /* Brief delay before retry */
            continue;
        }
    }
}
```

## Testing

After this fix:
1. Module should load without errors
2. STREAMON should complete without hanging
3. Event thread should process events continuously
4. No watchdog reboots during normal operation

## Technical Notes

### Kernel 3.10 Compatibility

In kernel 3.10, the completion API uses:
- `INIT_COMPLETION(x)` - Macro to reinitialize completion (removed in kernel 3.13+)
- `reinit_completion(&x)` - Function introduced in kernel 3.13 to replace `INIT_COMPLETION`

The difference:
- `INIT_COMPLETION(x)` - Takes completion by value (macro)
- `reinit_completion(&x)` - Takes completion by pointer (function)

Both do the same thing: reset the completion's `done` field to 0.

### Why Not Use a Semaphore?

Completions are preferred over semaphores for this use case because:
- Completions are designed for "event notification" patterns
- They have clearer semantics for one-shot events
- They integrate better with kernel wait queues
- The kernel documentation recommends completions for this pattern

### Alternative Solutions Considered

1. **Use `wait_for_completion_timeout()`**: Would prevent infinite hang but wouldn't fix the root cause
2. **Use a semaphore**: Would work but completions are more idiomatic for this pattern
3. **Use `complete_all()` instead of `complete()`**: Would allow multiple waiters but doesn't solve the reinit issue

## References

- Linux kernel completion API: `include/linux/completion.h`
- Kernel 3.10 documentation: `Documentation/scheduler/completion.txt`
- Similar pattern in kernel: `drivers/media/` subsystem event handling
- `INIT_COMPLETION` to `reinit_completion` migration: kernel commit 6d1d51e (v3.13)

