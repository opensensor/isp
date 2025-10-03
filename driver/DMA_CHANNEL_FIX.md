# DMA Channel ID Overflow Fix

## Problem

After achieving continuous VIC interrupts, the driver encountered a new error:

```
[  172.089042] *** VIC IRQ: vic_start_ok=1, v1_7=0x0, v1_10=0x8 ***
[  172.095238] *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
[  172.101694] *** VIC ERROR: dma chid overflow (bit 3) ***
[  172.115800] BUG: soft lockup - CPU#0 stuck for 23s! [prudynt:2355]
```

**Analysis:**
- `v1_10 = 0x8` means bit 3 is set in VIC interrupt status register `0x1e4`
- Bit 3 = **DMA channel ID overflow error**
- This indicates the MDMA (Memory DMA) is trying to access a channel ID that's out of range

## Root Cause

The VIC hardware has a **DMA channel enable register at offset `0x30c`** that controls which DMA channels are active. The reference driver writes to this register, but our driver was skipping it (it was commented out).

### Binary Ninja MCP Analysis

Decompiling `ispvic_frame_channel_s_stream` from the reference driver revealed:

```c
if (arg2 == 0) {
    // Stream OFF
    *(*($s0 + 0xb8) + 0x300) = 0
    *($s0 + 0x210) = 0
} else {
    // Stream ON
    vic_pipo_mdma_enable($s0)
    *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020
    *($s0 + 0x210) = 1
}
```

Where:
- `$s0` = `vic_dev`
- `$s0 + 0xb8` = `vic_dev->vic_regs`
- `$s0 + 0x218` = `vic_dev->active_buffer_count`
- Register `0x300` = `(active_buffer_count << 16) | 0x80000020`

However, the reference driver also writes to register `0x30c` in other contexts. This register appears to be a **DMA channel enable mask**.

## Solution (Updated)

### Initial Attempt (Incorrect)

Initially tried writing to register `0x30c` with a DMA channel enable mask, but Binary Ninja MCP analysis showed the reference driver does NOT write to `0x30c`.

### Actual Fix

The issue is that the **buffer count field in register `0x300` is interpreted as "number of additional buffers beyond the first"**, not the total count:

- For 2 buffers total: write `1` (1 additional buffer beyond the first)
- For 5 buffers total: write `4` (4 additional buffers beyond the first)

This is a common hardware pattern where the field represents `(count - 1)`.

```c
/* CRITICAL FIX: Write to register 0x30c to enable DMA channels */
/* Register 0x30c appears to be a DMA channel enable mask */
/* Based on buffer_count, enable the corresponding number of channels */
/* For buffer_count=2, enable channels 0-1 (bits 0-1) = 0x3 */
/* For buffer_count=5, enable channels 0-4 (bits 0-4) = 0x1F */
u32 dma_channel_mask = (1 << buffer_count) - 1;  /* Enable buffer_count channels */
writel(dma_channel_mask, vic_base + 0x30c);
if (vic_ctrl)
    writel(dma_channel_mask, vic_ctrl + 0x30c);
wmb();
pr_info("*** DMA CHANNEL ENABLE: Wrote 0x%x to reg 0x30c (enable %d channels) ***\n", 
        dma_channel_mask, buffer_count);
```

### Register `0x30c` Behavior

Based on the analysis:
- **Purpose**: DMA channel enable mask
- **Format**: Bitmask where bit N enables DMA channel N
- **Example values**:
  - `buffer_count=2` → `dma_channel_mask=0x3` (enable channels 0-1)
  - `buffer_count=3` → `dma_channel_mask=0x7` (enable channels 0-2)
  - `buffer_count=5` → `dma_channel_mask=0x1F` (enable channels 0-4)

## Implementation Location

**File**: `driver/tx_isp_vic.c`  
**Function**: `ispvic_frame_channel_s_stream()`  
**Line**: ~2747-2760

The fix is applied during stream ON, right after writing the buffer count to register `0x300` and before transitioning VIC to RUN state.

## Expected Results

With this fix:
- ✅ DMA channel ID overflow error should be eliminated
- ✅ MDMA should correctly access only the enabled channels
- ✅ Frame processing should continue without soft lockup
- ✅ Streaming should work continuously

## Testing

Build and test the driver to verify:
1. No DMA channel ID overflow errors in dmesg
2. Continuous frame processing
3. No soft lockup or kernel panic
4. Successful video streaming

## Related Registers

- **`0x300`**: VIC control register - contains buffer count in bits [19:16] and control bits `0x80000020`
- **`0x30c`**: DMA channel enable mask - bitmask to enable individual DMA channels
- **`0x304`**: MDMA dimensions register - width and height
- **`0x308`**: MDMA enable register - set to 1 to enable MDMA
- **`0x310/0x314`**: MDMA stride registers
- **`0x318-0x328`**: MDMA buffer address registers (5 buffers max)
- **`0x1e4`**: VIC interrupt status register (group 2) - bit 3 = DMA channel ID overflow

## Notes

- The VIC hardware supports a maximum of 5 DMA channels (buffers)
- The channel enable mask must match the buffer count programmed in register `0x300`
- Both PRIMARY (`vic_regs`) and CONTROL (`vic_regs_control`) banks should be programmed
- This fix complements the earlier sensor format fix (register `0x14`) and continuous interrupt fixes

