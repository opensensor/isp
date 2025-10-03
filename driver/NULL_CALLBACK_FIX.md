# NULL Callback Fix - Kernel Panic Resolution

## Problem
Kernel panic with NULL pointer dereference:
```
CPU 0 Unable to handle kernel paging request at virtual address 00000000, epc == 00000000, ra == 00000000
```

The system was trying to execute code at address 0x00000000, indicating a NULL function pointer being called.

## Root Cause
The `raw_pipe[1]` callback function pointer was NULL. The MDMA IRQ handler (`vic_mdma_irq_function`) calls this callback when a buffer DMA completes:

```c
if (raw_pipe && raw_pipe[1]) {
    void (*callback)(void *, struct vic_buffer_entry *) = raw_pipe[1];
    void *callback_arg = raw_pipe[4];
    callback(callback_arg, completed_buffer);
}
```

But we never set `raw_pipe[1]` to a valid function pointer!

## Binary Ninja Analysis

The Binary Ninja decompilation of `vic_mdma_irq_function` shows:

```c
uint32_t raw_pipe_1 = raw_pipe
(*(raw_pipe_1 + 4))(*(raw_pipe_1 + 0x14), $v0_2)
```

This translates to:
- Call function at `raw_pipe[1]` (offset 0x4 / 4 = index 1)
- With arguments: `raw_pipe[5]` (offset 0x14 / 4 = index 5) and buffer entry

The Binary Ninja decompilation of `tx_isp_subdev_pipo` shows:

```c
*raw_pipe = 0xbd4                          // ispvic_frame_channel_qbuf
*(raw_pipe + 8) = 0xd04                    // ispvic_frame_channel_clearbuf
*(raw_pipe + 0xc) = ispvic_frame_channel_s_stream
*(raw_pipe + 0x10) = arg1                  // subdev pointer
```

But it doesn't explicitly show setting offset 0x4 (raw_pipe[1]), which means it might be set elsewhere or we need to provide a callback.

## Solution

### 1. Created Buffer Completion Callback

**File**: `driver/tx_isp_vic.c` (lines 3771-3780)

```c
/* VIC buffer completion callback - called by MDMA IRQ handler */
static void vic_buffer_done_callback(void *arg, struct vic_buffer_entry *buffer)
{
    pr_info("*** vic_buffer_done_callback: Buffer completed - addr=0x%x, index=%d ***\n",
            buffer ? buffer->buffer_addr : 0,
            buffer ? buffer->buffer_index : -1);
    
    /* This callback is called when a buffer DMA completes */
    /* In the full implementation, this would signal userspace that a frame is ready */
    /* For now, just log it */
}
```

### 2. Set raw_pipe[1] in tx_isp_subdev_pipo

**File**: `driver/tx_isp_vic.c` (lines 3838-3854)

```c
if (raw_pipe) {
    raw_pipe[0] = (void *)ispvic_frame_channel_qbuf;      /* offset 0x0 */
    raw_pipe[1] = (void *)vic_buffer_done_callback;       /* offset 0x4 - CRITICAL! */
    raw_pipe[2] = (void *)ispvic_frame_channel_clearbuf;  /* offset 0x8 */
    raw_pipe[3] = (void *)ispvic_frame_channel_s_stream;  /* offset 0xc */
    raw_pipe[4] = (void *)sd;                             /* offset 0x10 */
    raw_pipe[5] = (void *)sd;                             /* offset 0x14 - callback arg */
    pr_info("*** CRITICAL: Set vic_buffer_done_callback at raw_pipe[1] (offset 0x4) ***\n");
    pr_info("*** CRITICAL: Set ispvic_frame_channel_s_stream at raw_pipe[3] (offset 0xc) ***\n");
}
```

### 3. Fixed Buffer Entry Reuse

**File**: `driver/tx_isp_vic.c` (lines 3721-3756)

Changed from:
- Pop from done queue
- Pop from free queue
- Copy data
- Free done entry
- Add free entry to busy queue

To:
- Pop from done queue
- Reuse the same entry (no need to pop from free queue)
- Add the entry directly to busy queue

This is more efficient and matches the Binary Ninja logic better.

## raw_pipe Array Layout

After the fix, the `raw_pipe` array is properly initialized:

| Index | Offset | Function Pointer | Purpose |
|-------|--------|------------------|---------|
| 0 | 0x00 | `ispvic_frame_channel_qbuf` | Queue buffer for DMA |
| 1 | 0x04 | `vic_buffer_done_callback` | **Buffer completion callback** |
| 2 | 0x08 | `ispvic_frame_channel_clearbuf` | Clear buffer |
| 3 | 0x0c | `ispvic_frame_channel_s_stream` | Start/stop streaming |
| 4 | 0x10 | `sd` (subdev pointer) | Subdev context |
| 5 | 0x14 | `sd` (subdev pointer) | **Callback argument** |

## Expected Behavior After Fix

1. VIC MDMA interrupt fires when buffer DMA completes
2. `vic_mdma_irq_function` is called
3. It pops the completed buffer from busy queue
4. It calls `raw_pipe[1]` (vic_buffer_done_callback) with:
   - arg = `raw_pipe[5]` (subdev pointer)
   - buffer = completed buffer entry
5. Callback logs the completion
6. Buffer is moved to free queue
7. Next buffer is queued from done queue
8. No more NULL pointer dereferences!

## Build Status
✅ **Build completed successfully with no compilation errors**

## Testing Checklist
- [ ] No kernel panics
- [ ] No NULL pointer dereferences
- [ ] MDMA interrupts fire
- [ ] `vic_buffer_done_callback` is called
- [ ] Buffer completion messages in dmesg
- [ ] Buffers rotate through queues correctly
- [ ] No "dma chid overflow" errors
- [ ] Frame streaming works

## Next Steps
1. Deploy to device
2. Monitor dmesg for:
   - "vic_buffer_done_callback: Buffer completed" messages
   - No kernel panics
   - No NULL pointer errors
3. Verify frame streaming works
4. Implement full buffer delivery to userspace in callback

