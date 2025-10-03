# ISP Driver Debug Iteration Summary

## Iteration 1-2: Log Level and Handler Registration

### What I Did
1. ✅ Checked `/proc/sys/kernel/printk` - was `3 3 1 7` (too low)
2. ✅ Set to `7 4 1 7` for maximum verbosity
3. ✅ Rebooted camera and loaded modules fresh
4. ✅ Started prudynt and monitored for 60+ seconds

### Key Findings

**CRITICAL DISCOVERY**: The driver behavior has **completely changed** from your earlier tests!

#### Earlier Behavior (Your Manual Tests)
- **5 ISP interrupts** (IRQ 37)
- **5 VIC interrupts** (IRQ 38)
- **5/5 stall** - exactly 5 interrupts then stop
- **Control limit errors** and **DMA overflow errors**
- **VIC interrupt handler logs** visible in dmesg

#### Current Behavior (My Automated Tests)
- **ZERO ISP interrupts** (IRQ 37)
- **ZERO VIC interrupts** (IRQ 38)
- **No stall** - stream never starts!
- **No errors** - because no interrupts are firing
- **NO VIC interrupt handler logs** - handlers never called

### Root Cause Analysis

The stream is **not starting at all**. Evidence:

1. **Prudynt is running** - `ps` shows process active
2. **Sensor is being configured** - I2C writes to GC2053 sensor registers
3. **CSI PHY is being reconfigured repeatedly** - Logs show continuous PHY control writes
4. **No STREAMON ioctl** - No "stream" or "qbuf" messages in dmesg
5. **No interrupts** - `/proc/interrupts` shows 0 for both IRQ 37 and 38

### Hypothesis

Prudynt is **stuck in initialization** and never calls `VIDIOC_STREAMON`. Possible reasons:

1. **Waiting for something** - Maybe waiting for a device file or resource
2. **Configuration error** - Prudynt config might be wrong for our driver
3. **Missing dependency** - Some userspace component not available
4. **Silent failure** - Prudynt encountering error but not logging it

### Comparison with Your Tests

Your manual tests showed **5/5 stall**, which means:
- Stream DID start (STREAMON was called)
- Interrupts DID fire (5 times)
- VIC handler DID run (you saw logs)
- Then it stalled after exactly 5 interrupts

My automated tests show **0/0 no-start**, which means:
- Stream NEVER started (STREAMON not called)
- Interrupts NEVER fired (0 times)
- VIC handler NEVER ran (no logs)
- Prudynt stuck in initialization

### What's Different?

The only difference between your tests and mine:
- **You**: Manual SSH, manual commands, saw 5/5 stall
- **Me**: Automated script, same commands, see 0/0 no-start

Possible explanations:
1. **Timing** - Maybe prudynt needs more time to start (I waited 60s, should be enough)
2. **Environment** - Maybe something in the environment is different after reboot
3. **Modules** - Maybe the modules I uploaded are different from what you tested
4. **Prudynt state** - Maybe prudynt needs to be killed/restarted differently

## Next Steps

### Option A: Reproduce Your 5/5 Stall
I need to figure out how to get the stream to actually start so I can test the VIC RE-ARM fix. Steps:

1. Check if prudynt is actually trying to open `/dev/video0`
2. Check if there are any errors in prudynt's output (currently redirected to `/dev/null`)
3. Try running prudynt in foreground to see what it's doing
4. Check if prudynt config file exists and is correct

### Option B: Test with Stock Modules
To verify the issue isn't with my build:

1. Upload the stock modules from `external/ingenic-sdk`
2. Test if those show 5/5 stall
3. If yes, then my build is the problem
4. If no, then something else changed

### Option C: Manual Testing
Follow your exact manual steps:

1. SSH to camera
2. Load modules manually
3. Start prudynt manually
4. Watch dmesg in real-time
5. See if 5/5 stall appears

## Files Created

- `driver/test_verbose.sh` - Automated test with full logging
- `driver/ITERATION_SUMMARY.md` - This file
- `/tmp/dmesg_module_load.log` - Dmesg during module load
- `/tmp/dmesg_full.log` - Complete dmesg after prudynt start
- `/tmp/isp_verbose_test_*.log` - Test output

## Current Module Status

Modules are loaded on camera at `/opt/`:
- `tx-isp-trace.ko`
- `tx-isp-t31.ko`
- `sensor_gc2053_t31.ko`

These are the modules built with all the fixes:
- VIC RE-ARM enabled
- Buffer count detection and correction
- VIC[0x0] write removed
- Gate enable prerequisites added
- Force active_buffer_count=1

## Recommendation

**I need to reproduce your 5/5 stall before I can test the fixes!**

The current 0/0 no-start behavior means the stream isn't even starting, so none of the VIC RE-ARM code is being executed. I need to:

1. Figure out why prudynt isn't starting the stream
2. Get it to the point where interrupts fire (even if just 5)
3. Then test if the VIC RE-ARM fix resolves the stall

**For the user**: When you return, please try manually loading the modules and starting prudynt to see if you can reproduce the 5/5 stall with the current build. If you can, capture the full dmesg and share it so I can see what's different.

