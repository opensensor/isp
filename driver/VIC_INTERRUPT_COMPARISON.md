# VIC Interrupt Handling Comparison: Working vs Current Driver

## Executive Summary

The **working driver** (isp-was-better) has **continuous interrupts** because it:
1. **Fully processes AE/AWB/AF interrupt callbacks** with DMA sync and statistics gathering
2. **Reads and clears BOTH interrupt banks** (legacy 0xb4/0xb8 AND new 0x98b4/0x98b8)
3. **Simpler VIC interrupt enable/disable** without complex kernel IRQ manipulation
4. **Simpler vic_core_ops_init** that just sets state flags

The **current driver** (isp-latest) **stalls at 4/4 frames** with VIC control errors because:
1. **Stubbed AE interrupt handlers** that skip all processing
2. **Only reads legacy interrupt bank** (missing new bank 0x98b4/0x98b8)
3. **Complex VIC IRQ enable/disable** with kernel-level enable_irq()/disable_irq() calls
4. **Complex vic_core_s_stream** with extensive register manipulation

## Critical Difference #1: AE Interrupt Processing

### Working Driver (isp-was-better)
```c
int ae0_interrupt_static(void) {
    uint32_t ae0_status = system_reg_read(0xa050);
    uint32_t buffer_offset = (ae0_status & 3) << 11;
    void *buffer_addr = (void *)(buffer_offset + data_b2f3c);
    
    // CRITICAL: DMA cache sync
    tisp_dma_cache_sync_helper(0, buffer_addr, 0x1000, 0);
    
    // CRITICAL: Get statistics
    tisp_ae0_get_statistics(buffer_addr, 0xf001f001);
    
    // CRITICAL: Set completion flag
    data_b0df8 = 1;
    
    return 1;
}
```

### Current Driver (isp-latest) - BROKEN
```c
int ae0_interrupt_static(void) {
    // CRITICAL SAFETY: Skip processing if data buffers not initialized
    if (data_b2f3c == 0) {
        return 1;  // Return success to prevent repeated calls
    }
    
    uint32_t ae0_status = system_reg_read(0xa050);
    // NO DMA sync, NO statistics gathering, NO completion flag!
    return 1;
}
```

**Impact**: The ISP hardware waits for acknowledgment that never comes, causing pipeline stalls.

## Critical Difference #2: Interrupt Bank Handling

### Working Driver (isp-was-better)
```c
// Support both legacy (+0xb*) and new (+0x98b*) interrupt banks
status_legacy = readl(isp_regs + 0xb4);
status_new    = readl(isp_regs + 0x98b4);
interrupt_status = status_legacy ? status_legacy : status_new;

// Clear BOTH banks
if (status_legacy)
    writel(status_legacy, isp_regs + 0xb8);
if (status_new)
    writel(status_new, isp_regs + 0x98b8);
wmb();
```

### Current Driver (isp-latest) - BROKEN
```c
// Only reads legacy bank
interrupt_status = readl(isp_regs + 0xb4);
writel(interrupt_status, isp_regs + 0xb8);
wmb();
// NO new bank handling!
```

**Impact**: Interrupts in the new bank (0x98b4) are never acknowledged, causing interrupt storms or stalls.

## Critical Difference #3: VIC Interrupt Enable/Disable

### Working Driver (isp-was-better) - SIMPLE
```c
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev) {
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 1;
    pr_info("VIC: Enabling interrupts (enable=%d)\n", enable);
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

### Current Driver (isp-latest) - COMPLEX
```c
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev) {
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 1;
    
    // CRITICAL FIX: Enable VIC interrupt at kernel level
    if (vic_dev->irq > 0) {
        enable_irq(vic_dev->irq);  // Kernel-level IRQ manipulation
    }
    
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

**Impact**: The kernel-level IRQ manipulation may interfere with the hardware interrupt flow, especially if called multiple times.

## Critical Difference #4: vic_core_ops_init Complexity

### Working Driver (isp-was-better) - SIMPLE
```c
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable) {
    vic_dev = tx_isp_get_subdevdata(sd);
    old_state = vic_dev->state;
    
    if (enable) {
        if (old_state != 3) {
            pr_info("VIC: Enabling interrupts\n");
            vic_dev->state = 3; /* READY -> ACTIVE */
        }
    } else {
        if (old_state != 2) {
            pr_info("VIC: Disabling interrupts\n");
            vic_dev->state = 2; /* ACTIVE -> READY */
        }
    }
    
    return 0;
}
```

### Current Driver (isp-latest) - COMPLEX
```c
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable) {
    vic_dev = tx_isp_get_subdevdata(sd);
    current_state = vic_dev->state;
    
    if (enable == 0) {
        if (current_state != 2) {
            tx_vic_disable_irq(vic_dev);  // Kernel IRQ manipulation
            vic_dev->state = 2;
        }
    } else {
        if (current_state != 3) {
            tx_vic_enable_irq(vic_dev);  // Kernel IRQ manipulation
            vic_dev->state = 3;
        }
    }
    
    return result;
}
```

**Impact**: The additional IRQ enable/disable calls may cause race conditions or interrupt masking issues.

## Critical Difference #5: vic_core_s_stream Complexity

### Working Driver (isp-was-better) - SIMPLE
```c
int vic_core_s_stream(struct tx_isp_subdev *sd, int enable) {
    vic_dev = tx_isp_get_subdev_hostdata(sd);
    current_state = vic_dev->state;
    
    if (enable == 0) {
        ret = 0;
        if (current_state == 4) {
            vic_dev->state = 3;
        }
        return ret;
    } else {
        ret = 0;
        if (current_state != 4) {
            ret = tx_isp_vic_start(vic_dev);
            if (ret != 0) return ret;
            
            vic_dev->state = 4;
        }
        return ret;
    }
}
```

### Current Driver (isp-latest) - EXTREMELY COMPLEX
```c
int vic_core_s_stream(struct tx_isp_subdev *sd, int enable) {
    // ... 300+ lines of complex register manipulation ...
    
    // Disable IRQ before VIC start
    // Call tx_isp_vic_start
    // Force QBUF writes
    // Call ispvic_frame_channel_s_stream
    // Clear ISP core VIC status
    // Re-assert gate
    // Re-write buffer addresses
    // Re-assert interrupt mask
    // Clear pending in BOTH banks
    // Enable VIC IRQ after final re-assert
    // Sample status for 200ms
    // Complex state transitions
    
    return ret;
}
```

**Impact**: The extensive register manipulation may interfere with the hardware state machine, causing control errors.

## Recommended Fixes

To enable continuous interrupts in the current driver:

### 1. Restore Full AE Interrupt Processing
Remove the early returns in `ae0_interrupt_static`, `ae0_interrupt_hist`, `ae1_interrupt_static`, `ae1_interrupt_hist` and implement:
- DMA cache sync (`tisp_dma_cache_sync_helper`)
- Statistics gathering (`tisp_ae0_get_statistics`, `tisp_ae1_get_statistics`)
- Event pushing (`tisp_event_push`)
- Completion flags (`data_b0df8 = 1`, etc.)

### 2. Add Dual Interrupt Bank Support
In `ispcore_interrupt_service_routine`:
```c
status_legacy = readl(isp_regs + 0xb4);
status_new    = readl(isp_regs + 0x98b4);
interrupt_status = status_legacy ? status_legacy : status_new;

if (status_legacy)
    writel(status_legacy, isp_regs + 0xb8);
if (status_new)
    writel(status_new, isp_regs + 0x98b8);
wmb();
```

### 3. Simplify VIC Interrupt Enable/Disable
Remove kernel-level `enable_irq()`/`disable_irq()` calls from `tx_vic_enable_irq` and `tx_vic_disable_irq`. Just set the flag.

### 4. Simplify vic_core_ops_init
Remove the `tx_vic_enable_irq`/`tx_vic_disable_irq` calls. Just manage state transitions.

### 5. Simplify vic_core_s_stream
Remove the extensive register manipulation. Follow the simple pattern from the working driver.

### 6. Initialize Data Buffers
Ensure `data_b2f3c`, `data_b2f48`, `data_b2f54`, `data_b2f60` are allocated before interrupts are enabled.

## Root Cause Analysis

The current driver has **defensive programming** that prevents interrupt processing:
- Safety checks that skip processing if buffers aren't initialized
- Complex IRQ management that may mask interrupts
- Extensive register manipulation that may interfere with hardware state

The working driver has **minimal, direct interrupt processing**:
- Always processes interrupts (no safety checks)
- Simple flag-based IRQ management
- Minimal register manipulation

**The ISP hardware expects immediate acknowledgment of interrupts. Any delay or skipped processing causes pipeline stalls.**

