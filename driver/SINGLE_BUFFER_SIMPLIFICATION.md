# Single Buffer Simplification + VIC State Fix

## Problem

After achieving continuous VIC interrupts, the driver encountered persistent errors:

```
[  286.342064] *** VIC ERROR: control limit error (bit 21) ***
[  286.347815] *** VIC ERROR: dma chid overflow (bit 3) ***
```

**Analysis:**
- **Control limit error (bit 21)**: VIC buffer ring is full - no free buffers available
- **DMA chid overflow (bit 3)**: DMA trying to access a channel ID that's out of range

## Root Cause

The ISP has **3 logical channels**, but VIC MDMA only supports **2 DMA channels**:

1. **Channel 0**: Main video stream (VIC MDMA channel 0)
2. **Channel 1**: Secondary stream (VIC MDMA channel 1)
3. **Channel 2**: JPEG snapshot via ISP-M0 `snapraw` ioctl (**NOT VIC MDMA**)

The errors were caused by:
1. **Buffer rotation complexity**: Multi-buffer ring rotation was causing timing issues
2. **Channel confusion**: The driver was conflating "number of buffers" with "number of channels"
3. **Buffer exhaustion**: The control limit error indicated all buffers were in use with none available

## Solution

**Simplified to 1 buffer per channel** to eliminate rotation complexity and get basic streaming working:

```c
/* CRITICAL SIMPLIFICATION: Use 1 buffer per channel to eliminate rotation complexity */
/* VIC MDMA supports 2 DMA channels (0 and 1) for video streaming */
/* Channel 2 is for ISP-M0 JPEG snapshots, NOT VIC MDMA */
/* Start with single-buffer mode to get basic streaming working */
u32 buffer_count = 1;  /* SIMPLIFIED: 1 buffer per channel */
u32 stream_ctrl = (buffer_count << 16) | 0x80000020;  /* Binary Ninja EXACT formula */
```

### Why This Works

1. **Eliminates buffer rotation**: No need to track which buffer is current/next
2. **Reduces DMA complexity**: Hardware always uses the same buffer addresses
3. **Prevents buffer exhaustion**: Only 1 buffer in use at a time, always available
4. **Matches hardware capabilities**: VIC MDMA has 2 channels, we use 1 buffer per channel
5. **Simplifies debugging**: Easier to trace buffer flow with single buffer

## Register `0x300` Format

**VIC Control Register (offset `0x300`):**

```
Bits [31:20]: Reserved
Bits [19:16]: Buffer count (number of buffers to rotate through)
Bits [15:6]:  Reserved
Bit  [5]:     Control bit (0x20)
Bits [4:0]:   Control bits (0x00)
```

**For single-buffer mode:**
- `buffer_count = 1`
- `stream_ctrl = (1 << 16) | 0x80000020 = 0x80010020`

**Control bits `0x80000020`:**
- `0x80000000`: Enable bit (bit 31)
- `0x00000020`: Mode/control bits

## VIC MDMA Architecture

### DMA Channels
- **Channel 0**: Primary video stream (registers `0x318-0x328`)
- **Channel 1**: Secondary stream (registers `0x340-0x350`)
- **Channel 2**: Does NOT exist in VIC MDMA (ISP-M0 handles JPEG)

### Buffer Registers (Channel 0)
- `0x318`: Buffer 0 address
- `0x31c`: Buffer 1 address
- `0x320`: Buffer 2 address
- `0x324`: Buffer 3 address
- `0x328`: Buffer 4 address

### Buffer Registers (Channel 1)
- `0x340`: Buffer 0 address
- `0x344`: Buffer 1 address
- `0x348`: Buffer 2 address
- `0x34c`: Buffer 3 address
- `0x350`: Buffer 4 address

### Single Buffer Mode
In single-buffer mode, only `0x318` (channel 0, buffer 0) is used. The hardware doesn't rotate through multiple buffers.

## Implementation Location

**File**: `driver/tx_isp_vic.c`  
**Function**: `ispvic_frame_channel_s_stream()`  
**Line**: ~2736-2742

The simplification is applied during stream ON, when writing the buffer count to register `0x300`.

## Expected Results

With single-buffer simplification:
- ✅ **No buffer rotation complexity**
- ✅ **No control limit errors** (buffer always available)
- ✅ **No DMA channel ID overflow** (only using channel 0)
- ✅ **Simpler interrupt handling** (no buffer index tracking)
- ✅ **Easier debugging** (single buffer to trace)

## Future Enhancements

Once basic streaming works with 1 buffer, we can incrementally add:
1. **2 buffers per channel**: Ping-pong buffering for smoother streaming
2. **3-5 buffers per channel**: Full ring buffer for maximum throughput
3. **Dual channel mode**: Enable channel 1 for secondary stream

But for now, **keep it simple** - 1 buffer per channel.

## Testing

Build and test the driver to verify:
1. No control limit errors in dmesg
2. No DMA channel ID overflow errors
3. Continuous VIC interrupts
4. Frame count progresses
5. Successful video streaming

## Related Registers

- **`0x300`**: VIC control register - buffer count in bits [19:16]
- **`0x304`**: MDMA dimensions register
- **`0x308`**: MDMA enable register
- **`0x310/0x314`**: MDMA stride registers
- **`0x318`**: Channel 0, Buffer 0 address (only buffer used in single-buffer mode)
- **`0x1e0`**: VIC interrupt status register (group 1)
- **`0x1e4`**: VIC interrupt status register (group 2) - bit 3 = DMA chid overflow, bit 21 = control limit

## Critical Fix: VIC State Transition Clears Buffer Count

### Discovery

The logs showed `CTRL[0x300]=0x80000020` instead of expected `0x80010020`:
- Expected: `0x80010020` (buffer_count=1 in bits [19:16])
- Actual: `0x80000020` (buffer_count=0 in bits [19:16])

The buffer count field was being **cleared after writing**!

### Root Cause

The driver was writing to VIC register `0x0` to transition VIC to RUN state **after** writing the buffer count to register `0x300`. This state transition was **clearing the buffer count field**.

### Binary Ninja Analysis

Decompiled the reference driver's `ispvic_frame_channel_s_stream()`:

```c
vic_pipo_mdma_enable($s0)  // Enable MDMA first
*(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020  // Then write buffer count
// NO WRITE TO REGISTER 0x0!
```

The reference driver:
1. Calls `vic_pipo_mdma_enable()` to configure MDMA (registers `0x308`, `0x304`, `0x310`, `0x314`)
2. Writes buffer count to register `0x300`
3. **Does NOT write to register `0x0` to change VIC state**

### Fix Applied

**Removed the VIC state transition write** that was clearing the buffer count:

```c
/* CRITICAL FIX FROM BINARY NINJA: Reference driver does NOT write to VIC[0x0] here! */
/* Writing to VIC[0x0] clears the buffer count field in register 0x300 */
/* The reference driver only writes to 0x308, 0x304, 0x310, 0x314, then 0x300 */
/* VIC state transitions happen elsewhere, not during stream ON */
```

The VIC state should already be correct from earlier initialization - no need to transition it during stream ON.

## Notes

- VIC MDMA supports maximum 5 buffers per channel
- VIC MDMA supports maximum 2 channels (0 and 1)
- Channel 2 is ISP-M0 JPEG, not VIC MDMA
- Single-buffer mode is the simplest configuration
- Buffer rotation can be added later once basic streaming works
- The reference driver uses multi-buffer mode, but we start simple
- **CRITICAL**: Never write to VIC[0x0] after writing buffer count to VIC[0x300] - it clears the buffer count field!

