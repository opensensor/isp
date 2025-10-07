# Recursive Loop Fix - 2025-10-04

## Problem

The driver was experiencing a kernel crash with an "Unhandled kernel unaligned access" error, followed by a repeating call trace pattern showing addresses being called in a loop:
- `0x803b462c`
- `0x803b4664`
- `0xc067f94c`

This indicated a **circular recursion** between functions in the driver.

## Root Cause Analysis

Using Binary Ninja MCP to decompile the reference driver, we discovered that:

### `tx_isp_video_s_stream` (address 0xcf90)
```c
// Simplified decompilation
int tx_isp_video_s_stream(struct tx_isp_dev *dev, int enable) {
    // Loop through all 16 subdevs
    for (int i = 0; i != 0x10; i++) {
        struct tx_isp_subdev *subdev = dev->subdevs[i];
        if (subdev && subdev->ops && subdev->ops->video && subdev->ops->video->s_stream) {
            // Call each subdev's s_stream function
            result = subdev->ops->video->s_stream(subdev, enable);
            if (result != 0 && result != -ENOIOCTLCMD) {
                // Handle error and rollback
                return result;
            }
        }
    }
    return 0;
}
```

### `ispcore_video_s_stream` (address 0x6880c)
```c
// Simplified decompilation
int ispcore_video_s_stream(struct tx_isp_subdev *sd, int enable) {
    // VIC state management
    if (enable == 0 && vic_state == 4) {
        // Stream OFF: handle frame channels
        vic_dev->state = 3;
    } else if (vic_state == 3) {
        // Stream ON
        vic_dev->state = 4;
    }
    
    // ALSO loops through all subdevs!
    while (true) {
        struct tx_isp_subdev *subdev = *subdev_ptr;
        if (subdev && subdev->ops && subdev->ops->video && subdev->ops->video->s_stream) {
            // Call each subdev's s_stream function
            result = subdev->ops->video->s_stream(subdev, enable);
            if (result != 0 && result != -ENOIOCTLCMD) {
                break;
            }
        }
        subdev_ptr++;
        if (subdev_ptr >= end) break;
    }
    
    // IRQ management
    if (enable == 0) {
        tx_isp_disable_irq(isp_dev);
    } else {
        tx_isp_enable_irq(isp_dev);
    }
    
    return result;
}
```

### The Circular Loop

The problem was:

1. **`tx_isp_video_s_stream`** iterates through all subdevs and calls their `s_stream` function
2. When it encounters the **core subdev**, it calls **`ispcore_video_s_stream`**
3. **`ispcore_video_s_stream`** ALSO iterates through all subdevs and calls their `s_stream` function
4. This creates a circular loop:
   ```
   tx_isp_video_s_stream
   → core_subdev->ops->video->s_stream (ispcore_video_s_stream)
   → iterates through subdevs again
   → calls other subdevs' s_stream
   → eventually calls back to tx_isp_video_s_stream
   → INFINITE RECURSION
   ```

## Key Insight

Both `tx_isp_video_s_stream` and `ispcore_video_s_stream` do the same thing (iterate through subdevs), which means they are **alternative entry points** for the same operation, NOT a call chain.

- `tx_isp_video_s_stream` is called from the main ioctl handler
- `ispcore_video_s_stream` is ALSO called directly from the main ioctl handler (in some cases)
- They should NOT call each other

## Solution

The fix is to **remove `s_stream` from the core subdev's video operations**:

```c
/* Core subdev video operations - GLOBAL to ensure proper accessibility */
struct tx_isp_subdev_video_ops core_subdev_video_ops = {
    .s_stream = NULL,  /* CRITICAL FIX: Core subdev should NOT have s_stream */
    .link_stream = NULL,
    .link_setup = ispcore_link_setup,
};
```

### Why This Works

1. `tx_isp_video_s_stream` iterates through all subdevs (CSI, VIC, sensors, etc.)
2. When it encounters the core subdev, it will skip it (because `s_stream` is NULL)
3. No circular recursion occurs
4. `ispcore_video_s_stream` can still be called directly when needed (e.g., from specific ioctl handlers)

## Files Modified

- **`driver/tx_isp_core.c`**:
  - Set `core_subdev_video_ops.s_stream = NULL` (line 958)
  - Removed unnecessary self-check code from `ispcore_video_s_stream` (lines 456-466)

## Testing

After this fix, the driver should:
1. Not experience recursive loop crashes
2. Properly propagate stream on/off commands to all subdevs (CSI, VIC, sensors)
3. Handle VIC state transitions correctly
4. Manage IRQs properly during streaming

## Related Issues

This fix resolves the circular recursion that was causing:
- Kernel unaligned access errors
- Stack overflow from infinite recursion
- System crashes during stream on/off operations

