# ISP Core Register Initialization Flow

## Memory Map Overview

```
0x10000000: CPM (Clock/Power Management)
0x10023000: VIC Control Bank (Secondary)
0x13300000: Main ISP Base
  ├─ 0x13300000-0x13300114: Core Control Registers
  ├─ 0x13309804-0x133098a8: ISP Pipeline Registers
  ├─ 0x13309a00-0x13309ac8: VIC Routing Registers ⭐ CRITICAL FOR INTERRUPTS
  ├─ 0x1330b000-0x1330b08c: Core Control Registers (0xb018-0xb024 SKIPPED)
  └─ 0x13310000-0x1331003c: CSI PHY Registers
0x133e0000: VIC Primary Bank
```

---

## Initialization Sequence

### Phase 1: CPM Clock Enable (Already Done Earlier)
```
CPM Base: 0x10000000
├─ [0x20] CLKGR0: Clear bit 13 (ISP), bit 21 (Alt ISP), bit 30 (VIC)
└─ [0x28] CLKGR1: Clear bit 30 (VIC)
```

### Phase 2: Main ISP Core Registers (0x13300000)
```
Main ISP Base: 0x13300000
├─ [0x0]   = 0x54560031      (Core control)
├─ [0x4]   = (W<<16)|H       (Dimensions: 1920x1080)
├─ [0x8]   = 0x1             (Enable)
├─ [0xc]   = 0x80700008      (Control)
├─ [0x28]  = 0x1             (Enable)
├─ [0x2c]  = 0x400040        (Config)
├─ [0x90]  = 0x1             (Enable)
├─ [0x94]  = 0x1             (Enable)
├─ [0x98]  = 0x30000         (Config)
├─ [0xa8]  = 0x58050000      (Config)
├─ [0xac]  = 0x58050000      (Config)
├─ [0xc4]  = 0x40000         (Config)
├─ [0xc8]  = 0x400040        (Config)
├─ [0xcc]  = 0x100           (Config)
├─ [0xd4]  = 0xc             (Config)
├─ [0xd8]  = 0xffffff        (Config)
├─ [0xe0]  = 0x100           (Config)
├─ [0xe4]  = 0x400040        (Config)
├─ [0xf0]  = 0xff808000      (Config)
├─ [0x110] = 0x80007000      (Config)
└─ [0x114] = 0x777111        (Config)
```

### Phase 3: ISP Pipeline Registers (0x13309804-0x133098a8)
```
ISP Pipeline: 0x13309804
├─ [0x9804] = 0x3f00         (Initial: 0x3f00, Delta: 0x0)
├─ [0x9864] = (W<<16)|H      (Dimensions)
├─ [0x987c] = 0xc0000000     (Config)
├─ [0x9880] = 0x1            (Enable)
├─ [0x9884] = 0x1            (Enable)
├─ [0x9890] = 0x1010001      (Config)
├─ [0x989c] = 0x1010001      (Config)
└─ [0x98a8] = 0x1010001      (Config)
```

### Phase 4: VIC Routing Registers ⭐ CRITICAL (0x13309a00-0x13309ac8)
```
VIC Routing: 0x13309a00
├─ [0x9a00] = 0x50002d0      (Width config)
├─ [0x9a04] = 0x3000300      (Height config)
├─ [0x9a2c] = 0x50002d0      (Stride config)
├─ [0x9a34] = 0x1            (Enable)
├─ [0x9a70] = 0x1 (W1C)      (Clear frame done status)
├─ [0x9a7c] = 0x1 (W1C)      (Clear frame done status)
├─ [0x9a80] = 0x500          (Stride value)
├─ [0x9a88] = 0x1            (Route/enable latch)
├─ [0x9a94] = 0x1            (Enable)
├─ [0x9a98] = 0x500          (Width-related)
├─ [0x9ac0] = 0x200 → 0x1   ⭐ VIC IRQ GATE (CRITICAL!)
└─ [0x9ac8] = 0x200 → 0x0   ⭐ VIC IRQ GATE (CRITICAL!)
```

**Why 0x9ac0/0x9ac8 are Critical:**
- These are the VIC interrupt gates
- Initial value 0x200 during setup
- Must be cleared to 0x0 during initialization
- Then re-asserted to 0x1/0x0 to enable frame done interrupts
- Without proper gate assertion, NO frame done interrupts will fire

### Phase 5: Core Control Registers (0x1330b000-0x1330b08c)
```
Core Control: 0x1330b000
├─ [0xb004] = 0xf001f001     (Config)
├─ [0xb008] = 0x40404040     (Config)
├─ [0xb00c] = 0x40404040     (Config)
├─ [0xb010] = 0x40404040     (Config)
├─ [0xb014] = 0x404040       (Config)
│
├─ [0xb018] = SKIPPED ❌     (Would kill VIC interrupts!)
├─ [0xb01c] = SKIPPED ❌     (Would kill VIC interrupts!)
├─ [0xb020] = SKIPPED ❌     (Would kill VIC interrupts!)
├─ [0xb024] = SKIPPED ❌     (Would kill VIC interrupts!)
│
├─ [0xb028] = 0x1000080 → 0x10d0046  (Initial → Delta)
├─ [0xb02c] = 0x1000080 → 0xe8002f   (Initial → Delta)
├─ [0xb030] = 0x100 → 0xc50100       (Initial → Delta)
├─ [0xb034] = 0xffff0100 → 0x1670100 (Initial → Delta)
├─ [0xb038] = 0x1ff00 → 0x1f001      (Initial → Delta)
├─ [0xb03c] = 0x0 → 0x22c0000        (Delta only)
├─ [0xb040] = 0x0 → 0x22c1000        (Delta only)
├─ [0xb044] = 0x0 → 0x22c2000        (Delta only)
├─ [0xb048] = 0x0 → 0x22c3000        (Delta only)
├─ [0xb04c] = 0x103 → 0x3            (Initial → Delta)
├─ [0xb050] = 0x3                    (Config)
├─ [0xb078] = 0x0 → 0x10000000       (Delta only)
├─ [0xb07c] = 0x1fffff               (Config)
├─ [0xb080] = 0x1fffff               (Config)
├─ [0xb084] = 0x1fffff               (Config)
├─ [0xb088] = 0x1fdeff               (Config)
└─ [0xb08c] = 0x1fff                 (Config)
```

**Why 0xb018-0xb024 Must Be Skipped:**
- These registers control interrupt routing/masking
- Writing to them disrupts the VIC→ISP interrupt path
- The was-better version explicitly skipped them
- Result: VIC interrupts are preserved

### Phase 6: 280ms Delta Updates
```
Delta Changes (Post-Sensor Detection):
├─ [0x9804] = 0x3f00 → 0x0           (Clear)
├─ [0x9ac0] = 0x200 → 0x0            (Clear gate)
├─ [0x9ac8] = 0x200 → 0x0            (Clear gate)
├─ [0xb018-0xb024] = STILL SKIPPED ❌ (Preserve interrupts)
└─ [0xb028-0xb078] = Updated          (See Phase 5)
```

### Phase 7: CSI PHY Configuration (0x13310000-0x1331003c)
```
CSI PHY: 0x13310000
├─ [0x0]  = 0x7d
├─ [0x4]  = 0xe3
├─ [0x8]  = 0xa0
├─ [0xc]  = 0x83
├─ [0x10] = 0xfa
├─ [0x1c] = 0x88
├─ [0x20] = 0x4e
├─ [0x24] = 0xdd
├─ [0x28] = 0x84
├─ [0x2c] = 0x5e
├─ [0x30] = 0xf0
├─ [0x34] = 0xc0
├─ [0x38] = 0x36
└─ [0x3c] = 0xdb
```

### Phase 8: Final VIC IRQ Gate Re-assertion ⭐
```
Final Gate Setup: 0x13309a00
├─ [0x9a70] = 0x1 (W1C)      (Clear any latched frame done)
├─ [0x9a7c] = 0x1 (W1C)      (Clear any latched frame done)
├─ [0x9ac0] = 0x00000001     ⭐ RE-ASSERT GATE (Enable interrupts!)
└─ [0x9ac8] = 0x00000000     ⭐ RE-ASSERT GATE (Enable interrupts!)
```

**This is the final critical step:**
- W1C clear any pending frame done status
- Re-assert gates to 0x1/0x0
- NOW frame done interrupts can fire!

---

## Interrupt Flow After Initialization

```
Sensor → CSI PHY → VIC → ISP Core → CPU
         (0x13310000)  (0x133e0000)  (0x13309a00)
                                      ↓
                                   VIC Routing
                                   [0x9ac0]=0x1
                                   [0x9ac8]=0x0
                                      ↓
                                   Frame Done IRQ
                                      ↓
                              vic_framedone_irq_function()
```

---

## Register Value Timeline

### Time T0: Initial State (Before Initialization)
```
[0x9ac0] = 0x??????  (Unknown/uninitialized)
[0x9ac8] = 0x??????  (Unknown/uninitialized)
[0xb018-0xb024] = 0x??????  (Unknown/uninitialized)
```

### Time T1: Phase 4 (VIC Routing Init)
```
[0x9ac0] = 0x200  (Gate setup value)
[0x9ac8] = 0x200  (Gate setup value)
[0xb018-0xb024] = UNTOUCHED ✅ (Preserve interrupts)
```

### Time T2: Phase 6 (280ms Delta)
```
[0x9ac0] = 0x0  (Gate cleared)
[0x9ac8] = 0x0  (Gate cleared)
[0xb018-0xb024] = STILL UNTOUCHED ✅ (Preserve interrupts)
```

### Time T3: Phase 8 (Final Re-assertion)
```
[0x9a70] = 0x1 (W1C clear)
[0x9a7c] = 0x1 (W1C clear)
[0x9ac0] = 0x1  ⭐ GATE ENABLED!
[0x9ac8] = 0x0  ⭐ GATE ENABLED!
[0xb018-0xb024] = STILL UNTOUCHED ✅ (Interrupts preserved!)
```

### Time T4: Runtime (Continuous Interrupts)
```
Frame N arrives → [0x9a70] asserts → vic_framedone_irq_function() called
Frame N+1 arrives → [0x9a70] asserts → vic_framedone_irq_function() called
Frame N+2 arrives → [0x9a70] asserts → vic_framedone_irq_function() called
...continuous interrupts! ✅
```

---

## Comparison: Before vs After Patch

### Before Patch (isp-latest)
```
[0x9a00-0x9ac8] = NOT INITIALIZED ❌
[0xb018-0xb024] = WRITTEN (kills interrupts) ❌
Result: Only 4/4 control errors, no continuous interrupts
```

### After Patch (with was-better init)
```
[0x9a00-0x9ac8] = PROPERLY INITIALIZED ✅
[0xb018-0xb024] = SKIPPED (preserves interrupts) ✅
Result: Continuous VIC frame done interrupts ✅
```

---

## Key Takeaways

1. **VIC Routing Registers (0x9a00-0x9ac8)** are essential for frame done interrupts
2. **VIC IRQ Gates (0x9ac0/0x9ac8)** must be properly initialized and re-asserted
3. **Interrupt-Killing Registers (0xb018-0xb024)** must be skipped
4. **CSI PHY Configuration** ensures proper MIPI data flow
5. **280ms Delta** represents post-sensor-detection register updates
6. **Gate Re-assertion** is the final critical step to enable interrupts

Without ALL of these steps, the driver will not generate continuous frame done interrupts!

