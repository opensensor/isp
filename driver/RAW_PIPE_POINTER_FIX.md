# Raw Pipe Pointer Fix - Critical Buffer Management Correction

## Problem
Kernel panic with NULL pointer dereference when calling `raw_pipe[1]` callback in MDMA IRQ handler.

## Root Cause Analysis

### Assembly Code Evidence
From Binary Ninja assembly listing:

```assembly
# tx_isp_subdev_pipo:
000030c8  sw $a1, 0x295c($s3)     # raw_pipe = arg2 (CRITICAL!)
0000310c  sw $v0, ($v1)           # *raw_pipe = 0xbd4
00003110  lw $v0, 0x295c($s3)     # reload raw_pipe
0000311c  sw $v1, 8($v0)          # *(raw_pipe + 8) = 0xd04
00003128  sw $v1, 0xc($v0)        # *(raw_pipe + 0xc) = ispvic_frame_channel_s_stream
0000312c  sw $s1, 0x10($v0)       # *(raw_pipe + 0x10) = arg1

# vic_mdma_irq_function:
00002580  lw $v0, 0x295c($s3)     # load raw_pipe
00002594  jalr $a2                # call function from raw_pipe
```

### Key Insight
The assembly shows `sw $a1, 0x295c($s3)` at address 0x000030c8, which means:
- **`raw_pipe` is a GLOBAL POINTER, not a global array!**
- `tx_isp_subdev_pipo` stores the `arg2` parameter into the global `raw_pipe` pointer
- Then it writes to the array that `arg2` points to

### Binary Ninja Decompilation
```c
raw_pipe = arg2                                    // Store pointer!
*raw_pipe = 0xbd4                                  // offset 0x0
*(raw_pipe + 8) = 0xd04                            // offset 0x8 (NOT 0x4!)
*(raw_pipe + 0xc) = ispvic_frame_channel_s_stream  // offset 0xc
*(raw_pipe + 0x10) = arg1                          // offset 0x10
```

**CRITICAL**: Notice that offset 0x4 (`raw_pipe[1]`) is **NOT SET**! The reference driver does not set this value in `tx_isp_subdev_pipo`.

## Our Incorrect Implementation

### Before Fix
```c
// tx-isp-module.c
void *raw_pipe[8] = {NULL};  // WRONG: This is an array

// tx_isp_subdev_pipo
void **raw_pipe = (void **)arg;  // WRONG: Local variable shadows global
if (raw_pipe) {
    raw_pipe[0] = ispvic_frame_channel_qbuf;
    raw_pipe[1] = vic_buffer_done_callback;  // WRONG: BN doesn't set this!
    raw_pipe[2] = ispvic_frame_channel_clearbuf;
    ...
}
```

Problems:
1. `raw_pipe` was declared as a global array instead of a pointer
2. We were setting `raw_pipe[1]` which the reference driver doesn't set
3. We weren't storing the `arg2` pointer in the global `raw_pipe`

## Correct Implementation

### After Fix

**File**: `driver/tx-isp-module.c`
```c
/* Binary Ninja raw_pipe - CRITICAL: This is a POINTER, not an array! */
/* Assembly shows: sw $a1, 0x295c($s3) - stores arg2 pointer to this global */
void *raw_pipe = NULL;  /* Global pointer to the pipe function array */
EXPORT_SYMBOL(raw_pipe);
```

**File**: `driver/tx_isp_vic.c` - `tx_isp_subdev_pipo`
```c
int tx_isp_subdev_pipo(struct tx_isp_subdev *sd, void *arg)
{
    void **pipe_array = (void **)arg;  /* The array passed in as arg2 */
    extern void *raw_pipe;  /* Global pointer */
    
    /* Binary Ninja EXACT: raw_pipe = arg2 */
    /* Assembly: sw $a1, 0x295c($s3) */
    raw_pipe = pipe_array;
    
    if (pipe_array) {
        pipe_array[0] = (void *)ispvic_frame_channel_qbuf;      /* offset 0x0 */
        /* pipe_array[1] is NOT set - Binary Ninja does not set offset 0x4! */
        pipe_array[2] = (void *)ispvic_frame_channel_clearbuf;  /* offset 0x8 */
        pipe_array[3] = (void *)ispvic_frame_channel_s_stream;  /* offset 0xc */
        pipe_array[4] = (void *)sd;                             /* offset 0x10 */
    }
}
```

**File**: `driver/tx_isp_tuning.c` - Caller
```c
/* CRITICAL: Create pipe function array to pass to tx_isp_subdev_pipo */
static void *pipe_functions[8] = {NULL};  /* Static so it persists */

/* tx_isp_subdev_pipo will set global raw_pipe = pipe_functions */
int pipo_ret = tx_isp_subdev_pipo(ourISPdev->vic_dev, pipe_functions);
```

**File**: `driver/tx_isp_vic.c` - MDMA IRQ handler
```c
/* Binary Ninja: Call raw_pipe callback */
extern void *raw_pipe;  /* Global pointer */
void **pipe = (void **)raw_pipe;
if (pipe && pipe[1]) {  /* Check for NULL - BN doesn't set this! */
    void (*callback)(void *, struct vic_buffer_entry *) = pipe[1];
    void *callback_arg = pipe[4];
    callback(callback_arg, completed_buffer);
}
```

## Raw Pipe Array Layout

After `tx_isp_subdev_pipo` completes:

| Index | Offset | Value | Set By |
|-------|--------|-------|--------|
| 0 | 0x00 | `ispvic_frame_channel_qbuf` | pipo |
| 1 | 0x04 | **NULL** | **Not set!** |
| 2 | 0x08 | `ispvic_frame_channel_clearbuf` | pipo |
| 3 | 0x0c | `ispvic_frame_channel_s_stream` | pipo |
| 4 | 0x10 | `sd` (subdev pointer) | pipo |
| 5 | 0x14 | **NULL** | **Not set in pipo** |
| 6 | 0x18 | NULL | - |
| 7 | 0x1c | NULL | - |

**CRITICAL**: `pipe[1]` remains NULL because the reference driver doesn't set it in `tx_isp_subdev_pipo`. The MDMA IRQ handler checks for NULL before calling it, so this is safe.

## Why This Matters

1. **Global Pointer vs Array**: The reference driver uses a global pointer that gets set to point to a caller-provided array. This allows different callers to provide different arrays.

2. **Offset 0x4 Not Set**: The reference driver does NOT set `raw_pipe[1]` (offset 0x4). Our code was setting it to `vic_buffer_done_callback`, which was causing crashes because that function might not exist or might have the wrong signature.

3. **Proper Indirection**: The MDMA IRQ handler loads the global `raw_pipe` pointer, then accesses the array it points to. This is a two-level indirection.

## Expected Behavior After Fix

1. `tisp_init` creates a static `pipe_functions[8]` array
2. Calls `tx_isp_subdev_pipo(vic_dev, pipe_functions)`
3. `tx_isp_subdev_pipo` sets `raw_pipe = pipe_functions` (global pointer)
4. `tx_isp_subdev_pipo` writes function pointers to `pipe_functions[0,2,3,4]`
5. `pipe_functions[1]` remains NULL
6. MDMA IRQ handler loads `raw_pipe`, casts to `void**`, checks `pipe[1]` for NULL
7. Since `pipe[1]` is NULL, the callback is not called
8. No more NULL pointer crashes!

## Build Status
✅ **Build completed successfully with no compilation errors**

## Files Modified
1. `driver/tx-isp-module.c` - Changed `raw_pipe` from array to pointer
2. `driver/tx_isp_vic.c` - Updated `tx_isp_subdev_pipo` to store pointer and write to array
3. `driver/tx_isp_vic.c` - Updated MDMA IRQ handler to use pointer correctly
4. `driver/tx_isp_tuning.c` - Created static array to pass to pipo

## Testing Checklist
- [ ] No kernel panics
- [ ] No NULL pointer dereferences
- [ ] MDMA interrupts fire
- [ ] `pipe[1]` is NULL (as expected)
- [ ] No crashes when checking `pipe[1]`
- [ ] Frame streaming works
- [ ] VIC buffer management works correctly

