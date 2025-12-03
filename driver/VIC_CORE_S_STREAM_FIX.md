# vic_core_s_stream - Simplified to Match Binary Ninja Reference

## Problem

Our implementation of `vic_core_s_stream` was extremely complex with hundreds of lines of register writes, manual ISP configuration, and complex initialization sequences. The Binary Ninja decompilation shows the reference driver is MUCH simpler.

## Binary Ninja Reference (0x15b4)

```c
int32_t $v0 = 0xffffffea

if (arg1 != 0)
    if (arg1 u>= 0xfffff001)
        return 0xffffffea
    
    void* $s1_1 = *(arg1 + 0xd4)  // Get VIC device
    $v0 = 0xffffffea
    
    if ($s1_1 != 0 && $s1_1 u< 0xfffff001)
        int32_t $v1_3 = *($s1_1 + 0x128)  // Read state
        
        if (arg2 == 0)  // Stream OFF
            $v0 = 0
            if ($v1_3 == 4)  // If streaming
                *($s1_1 + 0x128) = 3  // Set state to 3
        else  // Stream ON
            $v0 = 0
            if ($v1_3 != 4)  // If not streaming
                tx_vic_disable_irq()
                int32_t $v0_1 = tx_isp_vic_start($s1_1)
                *($s1_1 + 0x128) = 4  // Set state to 4
                tx_vic_enable_irq()
                return $v0_1

return $v0
```

## Our Original Implementation (WRONG)

Our implementation had:
- ~400 lines of code
- Manual register writes to VIC, ISP, CSI
- Complex initialization sequences
- Multiple ioremap calls
- Hardcoded register values
- Complex state management

This was completely wrong! The reference driver delegates all the complex work to `tx_isp_vic_start()`.

## Fixed Implementation (CORRECT)

```c
int vic_core_s_stream(struct tx_isp_subdev *sd, int enable)
{
    struct tx_isp_vic_device *vic_dev;
    int state;
    int ret = -EINVAL;

    /* Binary Ninja: if (arg1 == 0 || arg1 u>= 0xfffff001) return 0xffffffea */
    if (!sd || (unsigned long)sd >= 0xfffff001) {
        return -EINVAL;
    }

    /* Binary Ninja: void* $s1_1 = *(arg1 + 0xd4) */
    vic_dev = (struct tx_isp_vic_device *)tx_isp_get_subdevdata(sd);
    ret = -EINVAL;

    /* Binary Ninja: if ($s1_1 != 0 && $s1_1 u< 0xfffff001) */
    if (vic_dev != NULL && (unsigned long)vic_dev < 0xfffff001) {
        /* Binary Ninja: int32_t $v1_3 = *($s1_1 + 0x128) */
        state = vic_dev->state;

        /* Binary Ninja: if (arg2 == 0) - Stream OFF */
        if (enable == 0) {
            ret = 0;
            /* Binary Ninja: if ($v1_3 == 4) */
            if (state == 4) {
                /* Binary Ninja: *($s1_1 + 0x128) = 3 */
                vic_dev->state = 3;
            }
        } else {
            /* Binary Ninja: else - Stream ON */
            ret = 0;
            /* Binary Ninja: if ($v1_3 != 4) */
            if (state != 4) {
                /* Binary Ninja: tx_vic_disable_irq() */
                tx_vic_disable_irq(vic_dev);
                
                /* Binary Ninja: int32_t $v0_1 = tx_isp_vic_start($s1_1) */
                ret = tx_isp_vic_start(vic_dev);
                
                /* Binary Ninja: *($s1_1 + 0x128) = 4 */
                vic_dev->state = 4;
                
                /* Binary Ninja: tx_vic_enable_irq() */
                tx_vic_enable_irq(vic_dev);
                
                /* Binary Ninja: return $v0_1 */
                return ret;
            }
        }
    }

    /* Binary Ninja: return $v0 */
    return ret;
}
```

## Key Changes

### 1. Removed Complex Initialization
- **Before**: ~400 lines of manual register writes
- **After**: Simple delegation to `tx_isp_vic_start()`

### 2. Simplified Control Flow
- **Before**: Complex nested conditions and multiple paths
- **After**: Simple if/else matching Binary Ninja exactly

### 3. Proper IRQ Management
- **Before**: Manual `vic_start_ok` flag manipulation
- **After**: Calls `tx_vic_disable_irq()` and `tx_vic_enable_irq()`

### 4. State Transitions
- **Stream OFF**: If state == 4, set state to 3
- **Stream ON**: If state != 4, disable IRQ, call `tx_isp_vic_start()`, set state to 4, enable IRQ

## Delegation to tx_isp_vic_start()

The reference driver delegates all complex VIC initialization to `tx_isp_vic_start()`. This function handles:
- VIC register configuration
- Sensor interface setup
- Clock management
- MIPI/DVP configuration
- Buffer setup

By delegating to this function, `vic_core_s_stream` remains simple and focused on:
1. State validation
2. IRQ management
3. State transitions

## Impact

### Before
- Complex, hard to maintain
- Hundreds of lines of code
- Difficult to debug
- Didn't match reference driver

### After
- Simple, easy to understand
- ~60 lines of code
- Matches Binary Ninja reference exactly
- Proper separation of concerns

## Verification

✅ **Control flow**: Matches Binary Ninja exactly
✅ **State transitions**: Correct (3 ↔ 4)
✅ **IRQ management**: Proper disable/enable around `tx_isp_vic_start()`
✅ **Delegation**: All complex work delegated to `tx_isp_vic_start()`

## Related Functions

- `tx_isp_vic_start()`: Handles all VIC hardware initialization
- `tx_vic_disable_irq()`: Disables VIC interrupts
- `tx_vic_enable_irq()`: Enables VIC interrupts

## File

- `driver/tx_isp_vic.c`, lines 2650-2709

## Lesson Learned

When reverse engineering, always check the Binary Ninja decompilation FIRST before implementing complex logic. The reference driver is often much simpler than we think!

