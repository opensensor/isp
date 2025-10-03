# VIC MDMA IRQ Callback Fix - Critical Use-After-Free and Callback Argument Fixes

## Problem 1: Use-After-Free in Buffer Management

### Root Cause
The frame channel code allocates a `vic_buffer_entry`, passes it to the VIC driver via `vic_core_ops_ioctl`, and then **immediately frees it**:

```c
// tx-isp-module.c line 3385-3414
struct vic_buffer_entry *node = VIC_BUFFER_ALLOC();
if (node) {
    node->buffer_addr = buffer_phys_addr;
    node->buffer_index = slot;
    node->buffer_status = VIC_BUFFER_STATUS_QUEUED;
    vic_core_ops_ioctl(&vic->sd, 0x3000005, node);
    kfree(node);  // <-- FREED HERE!
}
```

But our `ispvic_frame_channel_qbuf` was storing the pointer directly in the done queue:

```c
// OLD CODE - WRONG!
if (new_buffer) {
    list_add_tail(&new_buffer->list, &vic_dev->done_head);  // Storing freed pointer!
}
```

Later, when `vic_mdma_irq_function` tried to pop from the done queue and use the buffer, it was accessing **freed memory**, causing the `BadVA: 20555043` ("CPUS") slab allocator corruption crash.

### Solution
**Copy the buffer data** instead of storing the pointer:

```c
// NEW CODE - CORRECT!
if (new_buffer) {
    struct vic_buffer_entry *done_entry;
    
    /* Allocate a new entry to store the buffer data */
    done_entry = VIC_BUFFER_ALLOC_ATOMIC();  // GFP_ATOMIC since we're in spinlock
    if (!done_entry) {
        pr_err("ispvic_frame_channel_qbuf: Failed to allocate done_entry\n");
        spin_unlock_irqrestore(&vic_dev->buffer_mgmt_lock, irq_flags);
        return 0;
    }
    
    /* Copy buffer data from new_buffer to done_entry */
    INIT_LIST_HEAD(&done_entry->list);
    done_entry->buffer_addr = new_buffer->buffer_addr;
    done_entry->buffer_index = new_buffer->buffer_index;
    done_entry->buffer_status = VIC_BUFFER_STATUS_QUEUED;
    
    /* Add the COPY to done queue, not the original pointer! */
    list_add_tail(&done_entry->list, &vic_dev->done_head);
}
```

This way, when the caller frees `new_buffer`, we still have our own copy in the done queue.

## Problem 2: Memory Leak in vic_mdma_irq_function

### Root Cause
In `vic_mdma_irq_function`, we were popping `next_buffer` from the done queue, copying its data to `free_buf`, but **never freeing `next_buffer`**:

```c
// OLD CODE - MEMORY LEAK!
next_buffer = list_first_entry(&vic_dev->done_head, struct vic_buffer_entry, list);
list_del(&next_buffer->list);

struct vic_buffer_entry *free_buf = list_first_entry(&vic_dev->free_head, ...);
list_del(&free_buf->list);

free_buf->buffer_addr = next_buffer->buffer_addr;  // Copy data
// ... but never kfree(next_buffer)!
```

### Solution
Free the done entry after copying its data:

```c
// NEW CODE - NO LEAK!
next_buffer = list_first_entry(&vic_dev->done_head, struct vic_buffer_entry, list);
list_del(&next_buffer->list);

struct vic_buffer_entry *free_buf = list_first_entry(&vic_dev->free_head, ...);
list_del(&free_buf->list);

/* Copy buffer address and index */
free_buf->buffer_addr = next_buffer->buffer_addr;
free_buf->buffer_index = next_buffer->buffer_index;
free_buf->buffer_status = VIC_BUFFER_STATUS_ACTIVE;

/* Add to busy queue */
list_add_tail(&free_buf->list, &vic_dev->busy_head);
vic_dev->active_buffer_count++;

/* Write buffer address to VIC register */
writel(free_buf->buffer_addr, vic_regs + ((free_buf->buffer_index + 0xc6) << 2));

/* CRITICAL: Free the done entry since we copied its data */
kfree(next_buffer);
```

## Problem 3: Wrong Callback Argument Index

### Root Cause
Binary Ninja decompilation shows:

```c
/* Binary Ninja: (*(raw_pipe_1 + 4))(*(raw_pipe_1 + 0x14), $v0_2) */
```

This means:
- `raw_pipe[1]` is the callback function (offset 0x4 / 4 = 1)
- `raw_pipe[5]` is the callback argument (offset 0x14 / 4 = 5)

But we were using `raw_pipe[4]` as the argument:

```c
// OLD CODE - WRONG INDEX!
if (pipe && pipe[1]) {
    void (*callback)(void *, struct vic_buffer_entry *) = pipe[1];
    void *callback_arg = pipe[4];  // WRONG! Should be pipe[5]
    callback(callback_arg, completed_buffer);
}
```

### Solution
Use the correct index:

```c
// NEW CODE - CORRECT INDEX!
if (pipe && pipe[1]) {
    void (*callback)(void *, struct vic_buffer_entry *) = pipe[1];
    void *callback_arg = pipe[5];  // Binary Ninja: offset 0x14 = index 5
    callback(callback_arg, completed_buffer);
}
```

## Summary of Changes

### File: `driver/tx_isp_vic.c`

1. **ispvic_frame_channel_qbuf** (lines 3695-3727):
   - Changed to allocate a new `done_entry` with `VIC_BUFFER_ALLOC_ATOMIC()`
   - Copy buffer data from `new_buffer` to `done_entry`
   - Add `done_entry` to done queue instead of `new_buffer`
   - This prevents use-after-free when caller frees `new_buffer`

2. **vic_mdma_irq_function** (lines 730-739, 764-769):
   - Changed callback argument from `pipe[4]` to `pipe[5]`
   - Matches Binary Ninja decompilation: `(*(raw_pipe_1 + 4))(*(raw_pipe_1 + 0x14), $v0_2)`

3. **vic_mdma_irq_function** (lines 785-812):
   - Added `kfree(next_buffer)` after copying its data to `free_buf`
   - Also copy `buffer_index` and `buffer_status` fields
   - Prevents memory leak

## Buffer Lifecycle

### Correct Flow:
1. **Frame channel** allocates `node` with `VIC_BUFFER_ALLOC()`
2. **Frame channel** calls `vic_core_ops_ioctl(&vic->sd, 0x3000005, node)`
3. **VIC driver** (`ispvic_frame_channel_qbuf`) allocates `done_entry` with `VIC_BUFFER_ALLOC_ATOMIC()`
4. **VIC driver** copies data from `node` to `done_entry`
5. **VIC driver** adds `done_entry` to done queue
6. **Frame channel** frees `node` with `kfree(node)` ✅ Safe - we have our own copy!
7. **MDMA IRQ** pops `done_entry` from done queue
8. **MDMA IRQ** copies data from `done_entry` to `free_buf` (from free queue)
9. **MDMA IRQ** frees `done_entry` with `kfree(done_entry)` ✅ No leak!
10. **MDMA IRQ** adds `free_buf` to busy queue
11. **MDMA IRQ** (next frame) pops `free_buf` from busy queue
12. **MDMA IRQ** moves `free_buf` back to free queue for reuse

### Key Points:
- **Frame channel** allocates temporary nodes that are freed immediately
- **VIC driver** maintains its own pool of buffer entries in the free queue
- **Done queue** contains copies of buffer data, not the original pointers
- **Busy queue** contains entries from the free queue that are being DMA'd
- **Free queue** contains reusable entries that persist across frames

## Build Status
✅ **Build completed successfully with no compilation errors**

## Expected Behavior After Fix
1. No more use-after-free crashes (`BadVA: 20555043`)
2. No more memory leaks in buffer management
3. Correct callback argument passed to `raw_pipe[1]`
4. Proper buffer rotation through free → done → busy → free queues
5. Frame streaming should work without kernel panics

## Testing Checklist
- [ ] No kernel panics
- [ ] No use-after-free errors
- [ ] No memory leaks (check `/proc/meminfo` over time)
- [ ] VIC MDMA interrupts fire correctly
- [ ] Buffers rotate through queues properly
- [ ] Frame streaming works
- [ ] No slab allocator corruption errors

