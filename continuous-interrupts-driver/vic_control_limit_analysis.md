# VIC Control Limit Error Analysis (0x200000)

## Root Cause Analysis

The control limit error (0x200000 bit) indicates the VIC hardware is rejecting our configuration due to register mismatches with the reference driver. After comparing our implementation with the reference traces, I've identified several critical issues:

## Critical Register Mismatches

### 1. VIC Mode Configuration Issue
**Problem**: Our implementation sets VIC mode = 3 (MIPI), but reference driver uses mode = 2
```c
// Current (WRONG):
writel(3, vic_regs + 0xc);  // Mode 3 causes control limit error

// Reference driver (CORRECT):
writel(2, vic_regs + 0xc);  // Mode 2 for MIPI interface
```

### 2. Register 0x18 Timing Parameter Issue
**Problem**: We're overwriting register 0x18 with sensor width, but it's a timing parameter
```c
// Current (WRONG):
writel(sensor_width * 2, vic_regs + 0x18);  // Destroys timing parameter

// Reference driver (CORRECT):
writel(0xf00, vic_regs + 0x18);  // Timing parameter must stay 0xf00
```

### 3. Missing Critical VIC Configuration Registers
**Problem**: We're missing essential VIC configuration registers from reference trace:

```c
// Missing from our implementation:
writel(0x800800, vic_regs + 0x60);      // Control register
writel(0x9d09d0, vic_regs + 0x64);      // Control register  
writel(0x6002, vic_regs + 0x70);        // Control register
writel(0x7003, vic_regs + 0x74);        // Control register

// Color space configuration (missing):
writel(0xeb8080, vic_regs + 0xc0);      // Color space config
writel(0x108080, vic_regs + 0xc4);      // Color space config
writel(0x29f06e, vic_regs + 0xc8);      // Color space config
writel(0x913622, vic_regs + 0xcc);      // Color space config
writel(0x515af0, vic_regs + 0xd0);      // Processing config
writel(0xaaa610, vic_regs + 0xd4);      // Processing config
writel(0xd21092, vic_regs + 0xd8);      // Processing config
writel(0x6acade, vic_regs + 0xdc);      // Processing config
```

### 4. Register Sequence Timing Issue
**Problem**: We're applying VIC configuration in wrong order
- Reference driver applies VIC registers BEFORE sensor detection
- We're applying them AFTER sensor detection
- This timing mismatch triggers hardware protection

### 5. Buffer Management Register Issue
**Problem**: Our buffer management doesn't match reference driver formula
```c
// Current implementation:
u32 stream_ctrl = (buffer_count << 16) | 0x80000020;

// But reference driver shows different buffer management approach
// Need to analyze ispvic_frame_channel_qbuf decompiled code
```

## Hardware Protection Mechanism

The VIC hardware has built-in protection that triggers control limit errors when:
1. Register values don't match expected hardware configuration
2. Timing parameters are corrupted (like register 0x18)
3. Mode configuration is incorrect for the interface type
4. Buffer management registers have invalid values
5. Configuration sequence is applied in wrong order

## Proposed Fixes

### Fix 1: Correct VIC Mode Configuration
```c
// In tx_isp_vic_start(), change:
writel(2, vic_regs + 0xc);  // Use mode 2 for MIPI, not 3
```

### Fix 2: Preserve Timing Parameters
```c
// Never overwrite register 0x18 - it's a timing parameter
// Remove this line:
// writel(sensor_width * 2, vic_regs + 0x18);

// Keep the reference driver value:
writel(0xf00, vic_regs + 0x18);  // Timing parameter
```

### Fix 3: Add Missing VIC Configuration
```c
// Add complete VIC configuration matching reference driver:
void apply_reference_vic_config(void __iomem *vic_regs) {
    // Control registers
    writel(0x800800, vic_regs + 0x60);
    writel(0x9d09d0, vic_regs + 0x64);
    writel(0x6002, vic_regs + 0x70);
    writel(0x7003, vic_regs + 0x74);
    
    // Color space configuration
    writel(0xeb8080, vic_regs + 0xc0);
    writel(0x108080, vic_regs + 0xc4);
    writel(0x29f06e, vic_regs + 0xc8);
    writel(0x913622, vic_regs + 0xcc);
    
    // Processing configuration
    writel(0x515af0, vic_regs + 0xd0);
    writel(0xaaa610, vic_regs + 0xd4);
    writel(0xd21092, vic_regs + 0xd8);
    writel(0x6acade, vic_regs + 0xdc);
    
    // Additional processing config
    writel(0xeb8080, vic_regs + 0xe0);
    writel(0x108080, vic_regs + 0xe4);
    writel(0x29f06e, vic_regs + 0xe8);
    writel(0x913622, vic_regs + 0xec);
    writel(0x515af0, vic_regs + 0xf0);
    writel(0xaaa610, vic_regs + 0xf4);
    writel(0xd21092, vic_regs + 0xf8);
    writel(0x6acade, vic_regs + 0xfc);
    wmb();
}
```

### Fix 4: Correct Configuration Sequence
```c
// Apply VIC configuration BEFORE sensor detection, not after
// Move VIC register setup to early initialization phase
// Follow exact reference driver timing
```

### Fix 5: Fix Buffer Management
```c
// Analyze the decompiled ispvic_frame_channel_qbuf code:
// - Uses offset 0xd4 to get vic_dev structure
// - Buffer management at offset 0x1f4 (spinlock)
// - Buffer programming at offset (buffer_index + 0xc6) << 2
// - Active buffer count at offset 0x218

// Ensure our buffer management matches this exactly
```

## Implementation Priority

1. **CRITICAL**: Fix VIC mode (2 instead of 3) - This alone may resolve the issue
2. **CRITICAL**: Preserve register 0x18 timing parameter (0xf00)
3. **HIGH**: Add missing VIC configuration registers
4. **MEDIUM**: Fix configuration sequence timing
5. **LOW**: Optimize buffer management

## Expected Result

After applying these fixes, the control limit error (0x200000) should be eliminated because:
- VIC hardware will accept the correct mode configuration
- Timing parameters won't be corrupted
- All required registers will be properly configured
- Hardware protection mechanism won't trigger

The VIC should then generate interrupts normally and maintain stable operation.
