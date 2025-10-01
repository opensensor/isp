# Porting Critical ISP Core Initialization from isp-was-better

## Date: 2025-10-01

## Problem Statement
The isp-latest driver only gets 4/4 control errors and no continuous VIC/ISP frame done interrupts, whereas the isp-was-better version had continuous working VIC and ISP interrupts.

## Root Cause Analysis
Using smart-diff MCP to compare the two versions revealed that isp-was-better had:

1. **Proper ISP core register initialization** at 0x13300000 (main ISP base)
2. **Skipped interrupt-killing registers** (0xb018-0xb024) to preserve VIC interrupts
3. **Proper VIC routing register configuration** (0x9a00-0x9ac8 range)
4. **Detailed CSI PHY configuration** sequence
5. **Called `ispcore_slake_module`** when VIC state reached 4

## Changes Made

### 1. ISP Core Register Initialization in `tx_isp_vic_start()`

Added comprehensive ISP core register initialization at the end of `tx_isp_vic_start()` function:

#### Step 1: Main ISP Core Registers (0x13300000 base)
- Configured core control registers (0x0-0x114)
- Set up ISP pipeline registers (0x9804-0x98a8)
- Used actual sensor dimensions (1920x1080) instead of total dimensions

#### Step 2: VIC Routing Registers (0x9a00-0x9ac8) - CRITICAL
These registers are **essential** for VIC frame done interrupts:
- `0x9a00`: Width configuration (0x50002d0)
- `0x9a04`: Height configuration (0x3000300)
- `0x9a2c`: Stride configuration (0x50002d0)
- `0x9a34`: Enable bit (0x1)
- `0x9a70`: Frame done status (W1C clear)
- `0x9a7c`: Frame done status (W1C clear)
- `0x9a80`: Stride value (0x500)
- `0x9a88`: Route/enable latch (0x1)
- `0x9a94`: Enable bit (0x1)
- `0x9a98`: Width-related (0x500)
- **`0x9ac0`: VIC IRQ gate (0x200 then 0x1) - CRITICAL**
- **`0x9ac8`: VIC IRQ gate (0x200 then 0x0) - CRITICAL**

#### Step 3: Core Control Registers (0xb000-0xb088) - INTERRUPT PRESERVING
Configured core control registers BUT **explicitly skipped** these interrupt-killing registers:
- **SKIPPED: 0xb018** (would write 0x40404040 then 0x24242424)
- **SKIPPED: 0xb01c** (would write 0x40404040 then 0x24242424)
- **SKIPPED: 0xb020** (would write 0x40404040 then 0x24242424)
- **SKIPPED: 0xb024** (would write 0x404040 then 0x242424)

These registers appear to control interrupt routing/masking, and writing to them kills VIC interrupts.

#### Step 4: 280ms Delta Register Changes
Applied post-sensor-detection register updates:
- Cleared VIC IRQ gates (0x9ac0/0x9ac8 → 0x0)
- Updated core control timing registers
- **Still skipped 0xb018-0xb024** during delta update

#### Step 5: CSI PHY Configuration
Applied detailed CSI PHY register sequence at 0x13310000 base:
- Configured PHY control registers (0x0-0x3c)
- Matches working trace from was-better version

#### Step 6: Re-assert VIC IRQ Gates
Final step to enable frame done interrupts:
- W1C clear any latched frame done status (0x9a70/0x9a7c)
- Re-assert VIC IRQ gates (0x9ac0=0x1, 0x9ac8=0x0)

### 2. Call `ispcore_slake_module` When VIC Reaches State 4

Modified `vic_core_s_stream()` to call `ispcore_slake_module` during 3→4 state transition:
- The was-better version that had working interrupts DID call this
- The ISP core initialization from `tx_isp_vic_start` should have already configured critical registers
- This initializes the ISP core pipeline properly

## Key Insights from Binary Ninja MCP

The reference driver's `vic_core_s_stream` is very simple:
```c
if (enable) {
    if (state != 4) {
        tx_vic_disable_irq();
        tx_isp_vic_start(vic_dev);
        state = 4;
        tx_vic_enable_irq();
    }
}
```

The complexity is in `tx_isp_vic_start()`, which should configure all the VIC and ISP core registers. Our implementation now matches this pattern.

## Expected Results

After these changes, the driver should:
1. ✅ Generate continuous VIC frame done interrupts (not just 4/4 control errors)
2. ✅ Have proper ISP core pipeline initialization
3. ✅ Preserve interrupt routing through careful register handling
4. ✅ Match the working behavior of isp-was-better version

## Testing Instructions

1. Build and load the driver:
   ```bash
   cd /home/matteius/isp-latest/driver
   make clean && make
   sudo rmmod tx_isp_t31 2>/dev/null
   sudo insmod tx-isp-t31.ko
   ```

2. Check for continuous interrupts in dmesg:
   ```bash
   dmesg | grep -E "VIC FRAME DONE|isp_vic_interrupt|framedone"
   ```

3. Look for successful ISP core initialization:
   ```bash
   dmesg | grep -E "ISP CORE INIT|VIC IRQ GATES|ispcore_slake_module"
   ```

4. Verify no control limit errors:
   ```bash
   dmesg | grep -i "control limit"
   ```

## Files Modified

- `driver/tx_isp_vic.c`:
  - Added ISP core register initialization in `tx_isp_vic_start()` (lines ~1406-1572)
  - Modified `vic_core_s_stream()` to call `ispcore_slake_module` (lines ~3168-3193)

## References

- Smart-diff comparison ID: `0633842f-0788-4e94-bafe-30b040152bf9`
- Source: `/home/matteius/isp-was-better/driver`
- Target: `/home/matteius/isp-latest/driver`
- Binary Ninja MCP decompilation of reference driver functions

## Critical Registers Summary

| Register | Value | Purpose | Notes |
|----------|-------|---------|-------|
| 0x9ac0 | 0x200→0x1 | VIC IRQ gate | CRITICAL for frame done IRQ |
| 0x9ac8 | 0x200→0x0 | VIC IRQ gate | CRITICAL for frame done IRQ |
| 0xb018-0xb024 | SKIPPED | Interrupt routing | Writing kills VIC interrupts |
| 0x9a70/0x9a7c | 0x1 (W1C) | Frame done status | Clear before re-asserting gates |

## Next Steps

If interrupts still don't work after this patch:
1. Check if CSI PHY is properly initialized before VIC start
2. Verify sensor is actually streaming before VIC enable
3. Check if there are additional clock/power domain requirements
4. Use Binary Ninja MCP to decompile more reference driver functions

