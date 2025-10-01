# Register 0x380 Conflict - Root Cause of 4/4 Interrupt Stall

## Problem Summary

The system was experiencing exactly **4 VIC interrupts** followed by a complete stall. Every interrupt showed:
- **Control limit error (bit 21)** 
- **No frame done (bit 0 never set)**
- Frames not arriving from CSI to VIC

## Root Cause

**Register 0x380 was being used for TWO CONFLICTING PURPOSES:**

### Purpose 1: VIC Input Source Configuration (ispcore_link_setup)
```c
// tx_isp_core.c line 730
writel(vic_input_config, vic_dev->vic_regs + 0x380);
```
This configures the VIC to receive data from CSI (the data path connection).

### Purpose 2: VIC Current Buffer Address (vic_initialize_buffer_ring)
```c
// tx_isp_vic.c line 786 (WRONG!)
writel(buffer_entry->buffer_addr, vic_regs + 0x380);
```
This was **overwriting** the VIC input source configuration!

## The Sequence of Events

1. **ispcore_link_setup** writes VIC input config to 0x380 → CSI→VIC path enabled ✅
2. **vic_initialize_buffer_ring** writes buffer address to 0x380 → CSI→VIC path **BROKEN** ❌
3. VIC starts but receives no frames from CSI
4. VIC generates control limit errors (bit 21)
5. After exactly 4 interrupts, system stalls

## Why Register 0x380 is Special

Looking at the code, register **0x380** is actually a **READ-ONLY status register** that shows the current buffer address being processed by VIC hardware. It should **NEVER** be written to!

### Evidence from the code:

```c
// tx_isp_vic.c - Reading current buffer (correct usage)
u32 current_buffer = readl(vic_base + 0x380);  /* Read current VIC buffer address */

// tx_isp_core.c - Writing input config (correct usage)  
writel(vic_input_config, vic_dev->vic_regs + 0x380);  /* Configure VIC input from CSI */

// tx_isp_vic.c - Writing buffer address (WRONG!)
writel(buffer_entry->buffer_addr, vic_regs + 0x380);  /* OVERWRITES input config! */
```

## The Fix

### 1. Remove the incorrect write to 0x380
```c
// BEFORE (WRONG):
writel(buffer_entry->buffer_addr, vic_regs + 0x380);

// AFTER (CORRECT):
// Don't write to 0x380 - it's read-only!
u32 readback = readl(vic_regs + 0x380);  // Only read
```

### 2. Remove vic_initialize_buffer_ring function entirely

The reference driver **doesn't have** a `vic_initialize_buffer_ring` function. Buffer initialization happens inline during:
- `vic_pipo_mdma_enable` 
- `vic_mdma_enable`

These functions program the buffer registers (0x318-0x328) directly when MDMA is enabled.

## Why This Caused Exactly 4 Interrupts

The VIC was configured but couldn't receive frames from CSI because the input path was broken:

1. **Interrupt 1-4**: VIC generates interrupts for control limit errors
2. **After 4 interrupts**: Some internal counter or state machine gives up
3. **System stalls**: No more interrupts, no recovery

## Verification

After the fix, we should see:
- ✅ VIC input source configuration preserved
- ✅ CSI→VIC data path working
- ✅ Frame done interrupts (bit 0 set)
- ✅ No control limit errors
- ✅ Continuous frame capture

## Logs Showing the Problem

```
[   25.723295] *** VIC VERIFY: Current address register [0x380] = 0x0 ***
```

The buffer initialization wrote **0x0** to register 0x380, overwriting the VIC input source configuration!

## Related Files Changed

1. **driver/tx_isp_vic.c**:
   - Removed `vic_initialize_buffer_ring` function (lines 724-831)
   - Removed call to `vic_initialize_buffer_ring` in `vic_core_s_stream` (line 3199)

2. **driver/tx_isp_core.c**:
   - `ispcore_link_setup` correctly writes VIC input config to 0x380 (line 730)
   - This configuration must be preserved!

## Key Takeaway

**Never write to register 0x380** - it's either:
1. A read-only status register showing current buffer address, OR
2. A configuration register for VIC input source (written once during link setup)

Either way, buffer initialization code should **NOT** touch it!

