# Binary Ninja PIPO Fix - Correct Implementation

## Problem

`ispvic_frame_channel_s_stream` was being called from `tx_isp_subdev_pipo`, which caused VIC register 0x300 to be written BEFORE REQBUFS set the buffer count. This resulted in the VIC being configured with the fallback value of 3 buffers instead of the correct value from REQBUFS.

## Binary Ninja Decompilation Analysis

### Reference Driver: `tx_isp_subdev_pipo`

```c
void* $s0 = nullptr

if (arg1 != 0 && arg1 u< 0xfffff001)
    $s0 = *(arg1 + 0xd4)

*($s0 + 0x20c) = 1
raw_pipe = arg2

if (arg2 == 0)
    *($s0 + 0x214) = 0
else
    *($s0 + 0x204) = $s0 + 0x204
    *($s0 + 0x208) = $s0 + 0x204
    *($s0 + 0x1f4) = $s0 + 0x1f4
    *($s0 + 0x1f8) = $s0 + 0x1f4
    *($s0 + 0x1fc) = $s0 + 0x1fc
    *($s0 + 0x200) = $s0 + 0x1fc
    private_spin_lock_init()
    *raw_pipe = 0xbd4
    uint32_t raw_pipe_1 = raw_pipe
    *(raw_pipe_1 + 8) = 0xd04
    *(raw_pipe_1 + 0xc) = ispvic_frame_channel_s_stream
    *(raw_pipe_1 + 0x10) = arg1
    int32_t i = 0
    void** $v0_3 = $s0 + 0x168
    
    do
        $v0_3[4] = i
        void*** $a0_1 = *($s0 + 0x200)
        *($s0 + 0x200) = $v0_3
        $v0_3[1] = $a0_1
        *$v0_3 = $s0 + 0x1fc
        *$a0_1 = $v0_3
        int32_t $a1 = (i + 0xc6) << 2
        i += 1
        *(*($s0 + 0xb8) + $a1) = 0
        $v0_3 = &$v0_3[7]
    while (i != 5)
    
    *($s0 + 0x214) = 1

return 0
```

**Key Observations:**
1. **Does NOT call `ispvic_frame_channel_s_stream`** - only stores it as a callback pointer
2. **Does NOT call `ispvic_frame_channel_qbuf`** - doesn't queue any buffers
3. **Only initializes buffer queues** - sets up linked lists
4. **Clears VIC buffer registers** - writes 0 to registers 0x318-0x328 directly
5. **Sets processing flag** - `*($s0 + 0x214) = 1`

The key line that clears buffer registers:
```c
*(*($s0 + 0xb8) + $a1) = 0  // where $a1 = (i + 0xc6) << 2
```
This translates to: `writel(0, vic_base + (0xc6 + i) * 4)` which is registers 0x318-0x328

### Reference Driver: `vic_pipo_mdma_enable`

```c
int32_t $v1 = *(arg1 + 0xdc)
*(*(arg1 + 0xb8) + 0x308) = 1
int32_t $v1_1 = $v1 << 1
*(*(arg1 + 0xb8) + 0x304) = *(arg1 + 0xdc) << 0x10 | *(arg1 + 0xe0)
*(*(arg1 + 0xb8) + 0x310) = $v1_1
void* result = *(arg1 + 0xb8)
*(result + 0x314) = $v1_1
return result
```

**Key Observations:**
1. **Does NOT write to register 0x300** - only writes MDMA config registers
2. **Writes to 0x308** - MDMA enable
3. **Writes to 0x304** - dimensions
4. **Writes to 0x310, 0x314** - stride

### Reference Driver: `ispvic_frame_channel_s_stream`

```c
if (arg2 == *($s0 + 0x210))
    return 0

__private_spin_lock_irqsave($s0 + 0x1f4, &var_18)

if (arg2 == 0)
    *(*($s0 + 0xb8) + 0x300) = 0
    *($s0 + 0x210) = 0
else
    vic_pipo_mdma_enable($s0)
    *(*($s0 + 0xb8) + 0x300) = *($s0 + 0x218) << 0x10 | 0x80000020
    *($s0 + 0x210) = 1

private_spin_unlock_irqrestore($s0 + 0x1f4, var_18)
return 0
```

**Key Observations:**
1. **Writes to register 0x300** - buffer count configuration
2. **Uses value from offset 0x218** - this is `active_buffer_count`
3. **Only called when streaming starts** - not during PIPO setup

## The Problem in Our Implementation

### What We Were Doing (WRONG):

```
1. tx_isp_subdev_pipo called
   ↓
2. Calls ispvic_frame_channel_s_stream(sd, 1)
   ↓
3. ispvic_frame_channel_s_stream writes VIC[0x300] = (active_buffer_count << 16) | 0x80000020
   ↓
4. active_buffer_count = 0 (not set yet by REQBUFS)
   ↓
5. Uses fallback: active_buffer_count = 3
   ↓
6. VIC[0x300] = 0x80030020 (3 buffers) ❌
   ↓
7. REQBUFS called later
   ↓
8. Sets active_buffer_count = 4
   ↓
9. But VIC[0x300] already written with 3! ❌
```

### What Reference Driver Does (CORRECT):

```
1. tx_isp_subdev_pipo called
   ↓
2. Only sets up pipe callbacks and buffer queues
   ↓
3. Does NOT call ispvic_frame_channel_s_stream
   ↓
4. REQBUFS called
   ↓
5. Sets active_buffer_count = 4 via 0x3000008 event
   ↓
6. STREAMON called
   ↓
7. Calls vic_core_s_stream
   ↓
8. vic_core_s_stream calls ispvic_frame_channel_s_stream(sd, 1)
   ↓
9. ispvic_frame_channel_s_stream writes VIC[0x300] = (4 << 16) | 0x80000020 ✅
   ↓
10. VIC[0x300] = 0x80040020 (4 buffers) ✅
```

## The Fix

### File: `driver/tx_isp_vic.c` (lines 4011-4014)

**Before:**
```c
/* CRITICAL MISSING CALL: Start VIC frame channel streaming */
pr_info("*** tx_isp_subdev_pipo: CALLING ispvic_frame_channel_s_stream to start VIC streaming ***\n");
int stream_ret = ispvic_frame_channel_s_stream(sd, 1);  /* Enable streaming */
if (stream_ret == 0) {
    pr_info("*** tx_isp_subdev_pipo: ispvic_frame_channel_s_stream SUCCESS - VIC streaming started! ***\n");
    
    /* CRITICAL FIX: Call vic_core_s_stream to enable VIC interrupts - this was missing! */
    pr_info("*** tx_isp_subdev_pipo: CALLING vic_core_s_stream to enable VIC interrupts ***\n");
    stream_ret = vic_core_s_stream(sd, 1);  /* Enable VIC interrupts */
    // ...
}
```

**After:**
```c
/* CRITICAL FIX: Reference driver does NOT call ispvic_frame_channel_s_stream here! */
/* Binary Ninja decompilation shows tx_isp_subdev_pipo only sets up pipe callbacks */
/* and initializes buffer queues - streaming is started later by vic_core_s_stream */
pr_info("*** tx_isp_subdev_pipo: PIPO setup complete (Binary Ninja EXACT - no streaming calls) ***\n");
```

## Expected Behavior After Fix

### Initialization Sequence:

```
1. Driver loads
   ↓
2. VIC probe: active_buffer_count = 4 (default)
   ↓
3. tx_isp_subdev_pipo called
   ↓
4. Sets up pipe callbacks only (Binary Ninja EXACT)
   ↓
5. active_buffer_count = 4 (preserved, not reset)
   ↓
6. REQBUFS called
   ↓
7. Sends 0x3000008 event to VIC with buffer_count=4
   ↓
8. vic_core_ops_ioctl: active_buffer_count = 4 (confirmed)
   ↓
9. QBUF called 4 times
   ↓
10. active_buffer_count = 4 (unchanged - no increment)
   ↓
11. STREAMON called
   ↓
12. Calls vic_core_s_stream
   ↓
13. vic_core_s_stream calls ispvic_frame_channel_s_stream(sd, 1)
   ↓
14. ispvic_frame_channel_s_stream writes VIC[0x300] = (4 << 16) | 0x80000020
   ↓
15. VIC[0x300] = 0x80040020 (4 buffers) ✅
   ↓
16. VIC starts streaming with correct buffer count ✅
```

## Expected Log Output

```
*** tx_isp_subdev_pipo: PIPO setup complete (Binary Ninja EXACT - no streaming calls) ***

*** Channel 0: REQBUFS - count=4, memory=2 ***
*** REQBUFS: Sending 0x3000008 event to VIC with buffer_count=4 ***
*** vic_core_ops_ioctl: VIC active_buffer_count set to 4 (Binary Ninja EXACT) ***

*** Channel 0: QBUF - Buffer 0 ***
ispvic_frame_channel_qbuf: Buffer queued (active_buffer_count=4 from REQBUFS)

*** Channel 0: VIDIOC_STREAMON ***
*** vic_core_s_stream: Calling ispvic_frame_channel_s_stream(ENABLE) ***
*** ispvic_frame_channel_s_stream: Stream state different - proceeding ***
*** CRITICAL: Calling vic_pipo_mdma_enable ***
vic_pipo_mdma_enable: reg 0x308 = 1 (MDMA enable)
vic_pipo_mdma_enable: reg 0x304 = 0x7800438 (dimensions 1920x1080)
vic_pipo_mdma_enable: reg 0x310 = 1920 (stride)
vic_pipo_mdma_enable: reg 0x314 = 1920 (stride)
*** VIC MDMA: Using active_buffer_count=4 from REQBUFS (Binary Ninja EXACT) ***
*** Binary Ninja EXACT: Wrote 0x80040020 to reg 0x300 (4 buffers) ***

*** VIC: Frame done interrupt ***
*** VIC: Frame done interrupt ***
... (continuous interrupts!)
```

## Summary of All Fixes

1. ✅ **Removed `active_buffer_count++` from QBUF** - it's a ring size, not a counter
2. ✅ **Removed `active_buffer_count--/++` from IRQ handler** - it's a ring size, not a busy queue depth
3. ✅ **Removed 0x3000008 event from STREAMON** - REQBUFS already sets it
4. ✅ **Removed `active_buffer_count = 0` from PIPO** - preserve the default value
5. ✅ **Removed `ispvic_frame_channel_s_stream` call from PIPO** - Binary Ninja shows it's not called there

Now the driver follows the **EXACT** Binary Ninja reference implementation!

