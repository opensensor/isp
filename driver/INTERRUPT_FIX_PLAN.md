# ISP Interrupt Fix Plan: Enable Continuous Interrupts

## Problem Statement

Current driver stalls at 4/4 frames with VIC control errors because:
1. AE/AWB/AF interrupt handlers are stubbed (skip processing)
2. Only legacy interrupt bank is read (missing new bank)
3. VIC interrupt enable/disable has complex kernel IRQ manipulation
4. vic_core_s_stream has extensive register manipulation

Working driver has continuous interrupts because:
1. Full AE interrupt processing with DMA sync and statistics
2. Reads and clears BOTH interrupt banks
3. Simple VIC interrupt enable/disable (just flag setting)
4. Simple vic_core_s_stream (minimal register manipulation)

## Priority 1: Restore AE Interrupt Processing (CRITICAL)

### File: `tx_isp_tuning.c`

#### ae0_interrupt_static
**Current (BROKEN)**:
```c
int ae0_interrupt_static(void) {
    if (data_b2f3c == 0) {
        return 1;  // Skip processing
    }
    uint32_t ae0_status = system_reg_read(0xa050);
    return 1;
}
```

**Fix (WORKING)**:
```c
int ae0_interrupt_static(void) {
    uint32_t ae0_status = system_reg_read(0xa050);
    uint32_t buffer_offset = (ae0_status & 3) << 11;
    void *buffer_addr = (void *)(buffer_offset + data_b2f3c);
    
    tisp_dma_cache_sync_helper(0, buffer_addr, 0x1000, 0);
    tisp_ae0_get_statistics(buffer_addr, 0xf001f001);
    data_b0df8 = 1;
    
    return 1;
}
```

#### ae0_interrupt_hist
**Current (BROKEN)**:
```c
int ae0_interrupt_hist(void) {
    if (data_b2f3c == 0) {
        return 1;
    }
    uint32_t ae0_hist_status = system_reg_read(0xa054);
    return 1;
}
```

**Fix (WORKING)**:
```c
int ae0_interrupt_hist(void) {
    uint32_t ae0_status = system_reg_read(0xa050);
    uint32_t buffer_offset = (ae0_status & 3) << 11;
    void *buffer_addr = (void *)(buffer_offset + data_b2f48);
    
    tisp_dma_cache_sync_helper(0, buffer_addr, 0x800, 0);
    
    void *hist_base = (void *)data_b2f48;
    int hist_flag = (data_b0e10 != 1) ? 1 : 0;
    
    tisp_ae0_get_hist(buffer_offset + hist_base, 1, hist_flag);
    
    struct {
        uint32_t pad1[2];
        uint32_t event_id;
        uint32_t pad2[8];
    } event_data = {0};
    event_data.event_id = 1;
    tisp_event_push(&event_data);
    
    return 2;
}
```

#### ae1_interrupt_static
**Current (BROKEN)**:
```c
int ae1_interrupt_static(void) {
    if (data_b2f3c == 0) {
        return 1;
    }
    uint32_t ae1_status = system_reg_read(0xa850);
    return 1;
}
```

**Fix (WORKING)**:
```c
int ae1_interrupt_static(void) {
    uint32_t ae1_status = system_reg_read(0xa850);
    void *buffer_addr = (void *)((ae1_status << 8) & 0x3000) + data_b2f54;
    
    tisp_dma_cache_sync_helper(0, buffer_addr, 0x1000, 0);
    tisp_ae1_get_statistics(buffer_addr, 0xf001f001);
    data_b0dfc = 1;
    
    return 1;
}
```

#### ae1_interrupt_hist
**Current (BROKEN)**:
```c
int ae1_interrupt_hist(void) {
    if (data_b2f3c == 0) {
        return 1;
    }
    uint32_t ae1_hist_status = system_reg_read(0xa854);
    return 1;
}
```

**Fix (WORKING)**:
```c
int ae1_interrupt_hist(void) {
    uint32_t ae1_status = system_reg_read(0xa850);
    uint32_t buffer_offset = (ae1_status & 3) << 11;
    void *buffer_addr = (void *)(buffer_offset + data_b2f60);
    
    private_dma_cache_sync(0, buffer_addr, 0x800, 0);
    tisp_ae1_get_hist(buffer_addr);
    
    struct {
        uint32_t pad1[2];
        uint32_t event_id;
        uint32_t pad2[8];
    } event_data = {0};
    event_data.event_id = 6;
    tisp_event_push(&event_data);
    
    return 2;
}
```

## Priority 2: Add Dual Interrupt Bank Support

### File: `tx_isp_core.c`

#### ispcore_interrupt_service_routine

**Current (BROKEN)**:
```c
interrupt_status = readl(isp_regs + 0xb4);
writel(interrupt_status, isp_regs + 0xb8);
wmb();
```

**Fix (WORKING)**:
```c
u32 status_legacy, status_new;

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

## Priority 3: Simplify VIC Interrupt Management

### File: `tx_isp_vic.c`

#### tx_vic_enable_irq

**Current (COMPLEX)**:
```c
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev) {
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 1;
    
    if (vic_dev->irq > 0) {
        enable_irq(vic_dev->irq);  // Kernel-level manipulation
    }
    
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

**Fix (SIMPLE)**:
```c
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev) {
    unsigned long flags;
    
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        return;
    }
    
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 1;
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

#### tx_vic_disable_irq

**Current (COMPLEX)**:
```c
void tx_vic_disable_irq(struct tx_isp_vic_device *vic_dev) {
    spin_lock_irqsave(&vic_dev->lock, flags);
    
    if (vic_dev->irq_enabled != 0) {
        vic_dev->irq_enabled = 0;
        
        if (vic_dev->irq > 0) {
            disable_irq(vic_dev->irq);  // Kernel-level manipulation
        }
    }
    
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

**Fix (SIMPLE)**:
```c
void tx_vic_disable_irq(struct tx_isp_vic_device *vic_dev) {
    unsigned long flags;
    
    if (!vic_dev || (unsigned long)vic_dev >= 0xfffff001) {
        return;
    }
    
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 0;
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

## Priority 4: Simplify vic_core_ops_init

### File: `tx_isp_vic.c`

**Current (COMPLEX)**:
```c
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable) {
    // ... validation ...
    
    if (enable == 0) {
        if (current_state != 2) {
            tx_vic_disable_irq(vic_dev);  // Extra call
            vic_dev->state = 2;
        }
    } else {
        if (current_state != 3) {
            tx_vic_enable_irq(vic_dev);  // Extra call
            vic_dev->state = 3;
        }
    }
    
    return result;
}
```

**Fix (SIMPLE)**:
```c
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable) {
    struct tx_isp_vic_device *vic_dev;
    int old_state;
    
    if (!sd)
        return -EINVAL;
    
    vic_dev = tx_isp_get_subdevdata(sd);
    if (!vic_dev)
        return -EINVAL;
    
    old_state = vic_dev->state;
    
    if (enable) {
        if (old_state != 3) {
            vic_dev->state = 3; /* READY -> ACTIVE */
        }
    } else {
        if (old_state != 2) {
            vic_dev->state = 2; /* ACTIVE -> READY */
        }
    }
    
    return 0;
}
```

## Implementation Order

1. **First**: Add dual interrupt bank support (Priority 2)
   - This is the safest change and may fix some issues immediately
   
2. **Second**: Simplify VIC interrupt management (Priority 3)
   - Remove kernel IRQ manipulation that may interfere
   
3. **Third**: Simplify vic_core_ops_init (Priority 4)
   - Remove redundant IRQ enable/disable calls
   
4. **Fourth**: Restore AE interrupt processing (Priority 1)
   - This is the most complex change but most critical
   - Requires implementing missing functions:
     - `tisp_dma_cache_sync_helper`
     - `tisp_ae0_get_statistics`
     - `tisp_ae0_get_hist`
     - `tisp_ae1_get_statistics`
     - `tisp_ae1_get_hist`
     - `tisp_event_push`

## Testing Strategy

After each change:
1. Build and load the driver
2. Start streaming with prudynt
3. Check for continuous interrupts in dmesg
4. Monitor for VIC control errors
5. Check frame count progression

Expected results after all fixes:
- Continuous ISP core interrupts (0x100, 0x400, etc.)
- No VIC control errors
- Frame count progresses beyond 4
- Streaming continues indefinitely

