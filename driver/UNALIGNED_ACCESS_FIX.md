# Unaligned Access Crash Fix

## Problem
System was crashing with "Unaligned kernel unaligned access" errors immediately after VIC interrupts were enabled.

## Root Cause Analysis

### Issue 1: VIC Interrupt Handler Signature Mismatch
**File**: `driver/tx_isp_vic.c` line 145

**Problem**: The VIC interrupt handler was declared with the wrong signature:
```c
irqreturn_t isp_vic_interrupt_service_routine(void *arg1);  // WRONG
```

**Correct signature**:
```c
irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id);  // CORRECT
```

**Impact**: When the kernel called the interrupt handler with two parameters (irq=38, dev_id=pointer), but the declaration said it only took one parameter, the compiler generated incorrect code that caused unaligned memory access.

### Issue 2: ISP Core Callback Signature Mismatch
**Files**: 
- `driver/tx_isp_tuning.c` line 125
- `driver/tx_isp_tuning.c` line 2219

**Problem**: The `ip_done_interrupt_static` callback was declared with the wrong signature:
```c
irqreturn_t ip_done_interrupt_static(int irq, void *dev_id);  // WRONG
```

**Correct signature**:
```c
int ip_done_interrupt_static(void);  // CORRECT
```

**Impact**: Callbacks registered via `system_irq_func_set()` are called with NO parameters (Binary Ninja MCP shows this clearly). The signature mismatch caused the compiler to generate code expecting parameters on the stack, but the caller wasn't passing any, resulting in unaligned access when the function tried to read non-existent parameters.

## Fixes Applied

### Fix 1: VIC Interrupt Handler Signature
**File**: `driver/tx_isp_vic.c`

Changed line 145 from:
```c
irqreturn_t isp_vic_interrupt_service_routine(void *arg1);
```

To:
```c
/* CRITICAL FIX: Correct signature to match Linux IRQ handler requirements and implementation in tx-isp-module.c */
irqreturn_t isp_vic_interrupt_service_routine(int irq, void *dev_id);
```

### Fix 2: ISP Core Callback Signature (Declaration 1)
**File**: `driver/tx_isp_tuning.c`

Changed lines 121-125 from:
```c
/* Forward declaration for frame channel wakeup function */
//int tisp_netlink_init(void);
int isp_trigger_frame_data_transfer(struct tx_isp_dev *dev);
int tisp_lsc_write_lut_datas(void);
irqreturn_t ip_done_interrupt_static(int irq, void *dev_id);
```

To:
```c
/* Forward declaration for frame channel wakeup function */
//int tisp_netlink_init(void);
int isp_trigger_frame_data_transfer(struct tx_isp_dev *dev);
int tisp_lsc_write_lut_datas(void);
/* CRITICAL FIX: Correct signature - callbacks registered via system_irq_func_set take NO parameters */
int ip_done_interrupt_static(void);
```

### Fix 3: ISP Core Callback Signature (Declaration 2)
**File**: `driver/tx_isp_tuning.c`

Changed lines 2217-2221 from:
```c
    /* Binary Ninja: system_irq_func_set(0xd, ip_done_interrupt_static) - Set IRQ handler */
    /* CRITICAL: This sets up the ISP processing completion callback - missing piece! */
    extern irqreturn_t ip_done_interrupt_static(int irq, void *dev_id);

    int irq_ret = system_irq_func_set(0xd, ip_done_interrupt_static);
```

To:
```c
    /* Binary Ninja: system_irq_func_set(0xd, ip_done_interrupt_static) - Set IRQ handler */
    /* CRITICAL: This sets up the ISP processing completion callback - missing piece! */
    /* CRITICAL FIX: Correct signature - callbacks registered via system_irq_func_set take NO parameters */
    extern int ip_done_interrupt_static(void);

    int irq_ret = system_irq_func_set(0xd, ip_done_interrupt_static);
```

## Technical Details

### Why This Causes Unaligned Access on MIPS

MIPS architecture requires strict alignment for memory access:
- 32-bit reads/writes must be 4-byte aligned
- 16-bit reads/writes must be 2-byte aligned

When a function signature mismatch occurs:
1. The **caller** generates code assuming one calling convention (e.g., passing parameters in registers a0, a1)
2. The **callee** generates code assuming a different calling convention (e.g., expecting no parameters)
3. The callee tries to access memory using register values that were meant to be parameters
4. These register values are often not properly aligned addresses, causing unaligned access exceptions

### Binary Ninja MCP Evidence

The Binary Ninja decompilation clearly shows:
- `isp_vic_interrupt_service_routine` is called with signature `(int irq, void *dev_id)` in `tx-isp-module.c`
- `ip_done_interrupt_static` is defined with signature `int (void)` in `tx_isp_core.c`
- Callbacks in `irq_func_cb[]` array are all `int (*)(void)` - no parameters

## Testing

After applying these fixes:
1. Build completed successfully with no warnings about signature mismatches
2. System should no longer crash with unaligned access errors when interrupts fire
3. VIC and ISP core interrupts should be handled correctly

## Related Files
- `driver/tx_isp_vic.c` - VIC interrupt handler
- `driver/tx_isp_tuning.c` - ISP tuning and callback registration
- `driver/tx_isp_core.c` - ISP core interrupt handler and callback implementation
- `driver/tx-isp-module.c` - Main interrupt dispatcher

## Date
2025-10-03

