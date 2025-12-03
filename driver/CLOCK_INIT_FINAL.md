# ISP Clock Initialization - Final Implementation

## Summary
Simplified ISP clock initialization to use a single function that initializes all 3 clocks in the correct order, with `cgu_isp` explicitly set to 100MHz.

## The Solution

### Single Function Call (Multiple Call Sites)
```c
// In ispcore_core_ops_init() - Primary initialization
if (!isp_dev->cgu_isp || !isp_dev->isp_clk || !isp_dev->csi_clk) {
    ret = tx_isp_configure_clocks(isp_dev);
    if (ret != 0) {
        printk(KERN_ALERT "*** tx_isp_configure_clocks failed: %d ***\n", ret);
        return ret;
    }
}

// In vin_s_stream() - Fallback initialization
if (!ourISPdev->cgu_isp || !ourISPdev->isp_clk || !ourISPdev->csi_clk) {
    ret = tx_isp_configure_clocks(ourISPdev);
    if (ret != 0) {
        return ret;
    }
}
```

### Implementation
```c
int tx_isp_configure_clocks(struct tx_isp_dev *isp)
{
    // 1. Get all 3 clocks
    cgu_isp = clk_get(isp->dev, "cgu_isp");
    isp_clk = clk_get(isp->dev, "isp");
    csi_clk = clk_get(isp->dev, "csi");
    
    // 2. Set cgu_isp to 100MHz (REQUIRED!)
    clk_set_rate(cgu_isp, 100000000);
    
    // 3. Enable in order (parent first)
    clk_prepare_enable(cgu_isp);  // Parent clock
    clk_prepare_enable(isp_clk);  // Auto-rate
    clk_prepare_enable(csi_clk);  // Auto-rate
    
    // 4. Store in isp_dev
    isp->cgu_isp = cgu_isp;
    isp->isp_clk = isp_clk;
    isp->csi_clk = csi_clk;
    
    // 5. Stabilization delay
    msleep(10);
}
```

## The 3 Clocks

| Clock    | Rate      | Set Rate? | Purpose                    |
|----------|-----------|-----------|----------------------------|
| cgu_isp  | 100MHz    | ✅ YES    | Parent clock for ISP       |
| isp      | 200MHz    | ❌ NO     | ISP core processing        |
| csi      | 100MHz    | ❌ NO     | CSI-2 interface            |

## Why This Works

### Before (BROKEN)
```c
// Tried to set rates for all clocks
clk_set_rate(cgu_isp, 120000000);  // ❌ Wrong rate
clk_set_rate(isp_clk, 200000000);  // ❌ Failed with -EINVAL
clk_set_rate(ipu_clk, 200000000);  // ❌ Unnecessary clock
clk_set_rate(csi_clk, 100000000);  // ❌ Failed with -EINVAL
```

### After (WORKING)
```c
// Only set rate for cgu_isp
clk_set_rate(cgu_isp, 100000000);  // ✅ Correct rate, succeeds
// isp and csi use pre-configured rates  // ✅ Auto-rate (0xffff)
```

## Reference Driver Platform Data

From the reference driver:
```c
/* VIC clocks */
static struct tx_isp_device_clk vic_clks[] = {
    {"cgu_isp", 100000000},  // ← Explicit 100MHz
    {"isp", 0xffff},         // ← Auto-rate
};

/* CSI clocks */
static struct tx_isp_device_clk csi_clks[] = {
    {"csi", 0xffff},         // ← Auto-rate
};
```

**Key insight**: `0xffff` means "auto-rate - don't call clk_set_rate()"

## Expected Behavior

### Successful Initialization
```
[CLK] Configuring ISP system clocks
[CLK] Setting CGU ISP clock rate to 100MHz (current=75000000 Hz)
[CLK] CGU ISP clock rate set to 100000000 Hz
[CLK] Enabling CGU ISP clock (rate=100000000 Hz)
[CLK] Enabling ISP clock (rate=200000000 Hz)
[CLK] Enabling CSI clock (rate=100000000 Hz)
[CLK] All ISP clocks enabled successfully
[CLK]   cgu_isp: 100000000 Hz  ✅
[CLK]   isp:     200000000 Hz  ✅
[CLK]   csi:     100000000 Hz  ✅
```

### What Changed From Bootloader
- **cgu_isp**: 75MHz → 100MHz (corrected by driver)
- **isp**: 200MHz → 200MHz (unchanged, already correct)
- **csi**: 100MHz → 100MHz (unchanged, already correct)

## Files Modified

1. **tx_isp_core.c**
   - Simplified `tx_isp_configure_clocks()` to only set cgu_isp rate
   - Removed ipu_clk references
   - Call in `ispcore_core_ops_init()`

2. **tx_isp_vin.c**
   - Added clock initialization check in `vin_s_stream()`
   - Ensures clocks are initialized before VIN streaming starts

3. **tx_isp_csi.c**
   - Removed redundant CSI clock initialization

4. **tx_isp_subdev.c**
   - Marked `isp_subdev_init_clks()` as deprecated

5. **tx_isp_proc.c**
   - Removed ipu_clk from proc output

6. **include/tx_isp.h**
   - Removed ipu_clk from struct tx_isp_dev

## Benefits

✅ **Simple** - One function, one call site  
✅ **Correct** - cgu_isp at required 100MHz  
✅ **Efficient** - No unnecessary rate changes  
✅ **Reliable** - Uses pre-configured rates where possible  
✅ **Maintainable** - Clear, documented approach  

## Testing Checklist

- [x] cgu_isp clock set to 100MHz
- [x] isp clock enabled at 200MHz
- [x] csi clock enabled at 100MHz
- [x] No clock initialization errors
- [x] CSI PHY writes succeed
- [x] ISP streaming works

## Deprecated Functions

The following function is now deprecated:
- `isp_subdev_init_clks()` - Use `tx_isp_configure_clocks()` instead

## Future Improvements

If needed, we could:
1. Make the cgu_isp rate configurable via module parameter
2. Add validation that rates are within acceptable ranges
3. Add fallback if clk_set_rate() fails for cgu_isp

But for now, the simple approach works perfectly!

