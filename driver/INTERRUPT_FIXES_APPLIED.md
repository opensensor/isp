# ISP Interrupt Fixes Applied - 2025-10-03

## Summary

Applied critical fixes to enable continuous interrupts based on comparison with working "isp-was-better" driver.
The current driver was stalling at 4/4 frames with VIC control errors due to incomplete interrupt processing.

## Fixes Applied (in order)

### Fix 1: Dual Interrupt Bank Support (tx_isp_core.c)
**File**: `driver/tx_isp_core.c`
**Function**: `ispcore_interrupt_service_routine()`
**Lines**: ~1871-1901

**Problem**: Only reading legacy interrupt bank (0xb4/0xb8), missing new bank (0x98b4/0x98b8)

**Fix**: Read and clear BOTH interrupt banks like working driver
```c
/* Read BOTH interrupt banks */
status_legacy = readl(isp_regs + 0xb4);
status_new    = readl(isp_regs + 0x98b4);
interrupt_status = status_legacy ? status_legacy : status_new;

/* Clear BOTH banks */
if (status_legacy)
    writel(status_legacy, isp_regs + 0xb8);
if (status_new)
    writel(status_new, isp_regs + 0x98b8);
wmb();
```

**Impact**: Prevents interrupt storms from unacknowledged interrupts in new bank

---

### Fix 2: Simplified VIC Interrupt Enable/Disable (tx_isp_vic.c)
**File**: `driver/tx_isp_vic.c`
**Functions**: `tx_vic_enable_irq()`, `tx_vic_disable_irq()`
**Lines**: ~178-234

**Problem**: Complex kernel-level `enable_irq()`/`disable_irq()` calls interfering with hardware interrupt flow

**Fix**: Removed kernel IRQ manipulation - just set/clear flag like working driver
```c
void tx_vic_enable_irq(struct tx_isp_vic_device *vic_dev)
{
    spin_lock_irqsave(&vic_dev->lock, flags);
    vic_dev->irq_enabled = 1;  // Just set flag
    spin_unlock_irqrestore(&vic_dev->lock, flags);
}
```

**Impact**: Eliminates race conditions from redundant IRQ management

---

### Fix 3: Simplified vic_core_ops_init (tx_isp_vic.c)
**File**: `driver/tx_isp_vic.c`
**Function**: `vic_core_ops_init()`
**Lines**: ~2524-2555

**Problem**: Calling `tx_vic_enable_irq()`/`tx_vic_disable_irq()` with kernel IRQ manipulation

**Fix**: Removed IRQ calls - just manage state transitions like working driver
```c
int vic_core_ops_init(struct tx_isp_subdev *sd, int enable)
{
    vic_dev = tx_isp_get_subdevdata(sd);
    old_state = vic_dev->state;
    
    if (enable) {
        if (old_state != 3)
            vic_dev->state = 3; /* READY -> ACTIVE */
    } else {
        if (old_state != 2)
            vic_dev->state = 2; /* ACTIVE -> READY */
    }
    
    return 0;
}
```

**Impact**: Eliminates redundant IRQ management that caused race conditions

---

### Fix 4: Restored Full AE Interrupt Processing (tx_isp_tuning.c)
**File**: `driver/tx_isp_tuning.c`
**Functions**: `ae0_interrupt_static()`, `ae0_interrupt_hist()`, `ae1_interrupt_static()`, `ae1_interrupt_hist()`
**Lines**: ~1676-1823

**Problem**: Stubbed interrupt handlers with early returns - ISP waits for acknowledgment that never comes

**Fix**: Restored full processing like working driver:

#### ae0_interrupt_static
```c
int ae0_interrupt_static(void)
{
    ae0_status = system_reg_read(0xa050);
    buffer_offset = (ae0_status & 3) << 11;
    buffer_addr = (void *)(buffer_offset + data_b2f3c);
    
    // CRITICAL: DMA cache sync
    if (data_b2f3c != 0 && virt_addr_valid(buffer_addr)) {
        tisp_dma_cache_sync_helper(NULL, buffer_addr, 0x1000, DMA_FROM_DEVICE);
        tisp_ae0_get_statistics(buffer_addr, 0xf001f001);
    }
    
    // CRITICAL: Set completion flag
    data_b0df8 = 1;
    
    return 1;
}
```

#### ae0_interrupt_hist
```c
int ae0_interrupt_hist(void)
{
    ae0_status = system_reg_read(0xa050);
    buffer_offset = (ae0_status & 3) << 11;
    buffer_addr = (void *)(buffer_offset + data_b2f48);
    
    // CRITICAL: DMA cache sync
    if (data_b2f48 != 0 && virt_addr_valid(buffer_addr)) {
        tisp_dma_cache_sync_helper(NULL, buffer_addr, 0x800, DMA_FROM_DEVICE);
        tisp_ae0_get_hist(buffer_offset + hist_base, 1, hist_flag);
        
        // CRITICAL: Push event
        event_data.event_id = 1;
        tisp_event_push(&event_data);
    }
    
    return 2;  // Working driver returns 2 for histogram interrupts
}
```

#### ae1_interrupt_static
```c
int ae1_interrupt_static(void)
{
    ae1_status = system_reg_read(0xa850);
    buffer_addr = (void *)((ae1_status << 8) & 0x3000) + data_b2f54;
    
    // CRITICAL: DMA cache sync
    if (data_b2f54 != 0 && virt_addr_valid(buffer_addr)) {
        tisp_dma_cache_sync_helper(NULL, buffer_addr, 0x1000, DMA_FROM_DEVICE);
        tisp_ae1_get_statistics(buffer_addr, 0xf001f001);
    }
    
    // CRITICAL: Set completion flag
    data_b0dfc = 1;
    
    return 1;
}
```

#### ae1_interrupt_hist
```c
int ae1_interrupt_hist(void)
{
    ae1_status = system_reg_read(0xa850);
    buffer_offset = (ae1_status & 3) << 11;
    buffer_addr = (void *)(buffer_offset + data_b2f60);
    
    // CRITICAL: DMA cache sync
    if (data_b2f60 != 0 && virt_addr_valid(buffer_addr)) {
        private_dma_cache_sync(NULL, buffer_addr, 0x800, DMA_FROM_DEVICE);
        tisp_ae1_get_hist(buffer_addr);
        
        // CRITICAL: Push event
        event_data.event_id = 6;
        tisp_event_push(&event_data);
    }
    
    return 2;  // Working driver returns 2 for histogram interrupts
}
```

**Impact**: ISP hardware receives proper acknowledgment, enabling continuous interrupt generation

---

### Fix 5: Added DMA Cache Sync Helper (tx_isp_tuning.c)
**File**: `driver/tx_isp_tuning.c`
**Lines**: ~1172-1177

**Fix**: Added wrapper function for AE interrupt handlers
```c
static inline void tisp_dma_cache_sync_helper(struct device *dev, void *vaddr, size_t size, enum dma_data_direction direction)
{
    private_dma_cache_sync(dev, vaddr, size, direction);
}
```

**Impact**: Provides DMA cache synchronization for statistics buffers

---

## Root Cause Analysis

The current driver had **defensive programming** that prevented interrupt processing:
- Safety checks that skip processing if buffers aren't initialized
- Complex IRQ management that may mask interrupts
- Extensive register manipulation that may interfere with hardware state

The working driver has **minimal, direct interrupt processing**:
- Always processes interrupts (no safety checks)
- Simple flag-based IRQ management
- Minimal register manipulation

**The ISP hardware expects immediate acknowledgment of interrupts. Any delay or skipped processing causes pipeline stalls.**

---

## Expected Results

After these fixes:
1. ✅ Continuous ISP core interrupts (0x100, 0x400, etc.)
2. ✅ No VIC control errors
3. ✅ Frame count progresses beyond 4
4. ✅ Streaming continues indefinitely
5. ✅ Dual interrupt bank support prevents interrupt storms
6. ✅ Simplified VIC IRQ management eliminates race conditions
7. ✅ Full AE processing enables continuous hardware operation

---

## Testing Instructions

1. Build driver: `./script.sh`
2. Load modules on device
3. Start streaming with prudynt
4. Monitor dmesg for:
   - "DUAL BANK" messages showing both banks being read
   - "FULL PROCESSING" messages from AE interrupt handlers
   - "SIMPLIFIED version" messages from VIC IRQ functions
   - Continuous interrupt processing without stalls

---

## Files Modified

1. `driver/tx_isp_core.c` - Dual interrupt bank support
2. `driver/tx_isp_vic.c` - Simplified VIC IRQ management and vic_core_ops_init
3. `driver/tx_isp_tuning.c` - Restored full AE interrupt processing + DMA helper

---

## Build Status

✅ Build completed successfully (2025-10-03)
✅ All modules compiled without errors
✅ Ready for device testing

