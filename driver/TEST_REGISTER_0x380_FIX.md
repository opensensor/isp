# Test Plan: Register 0x380 Conflict Fix

## What We Fixed

1. **Removed incorrect write to register 0x380** in buffer initialization
2. **Removed vic_initialize_buffer_ring function** (doesn't exist in reference driver)
3. **Preserved VIC input source configuration** set by ispcore_link_setup

## Expected Results

### Before Fix (4/4 Stall)
```
[   25.977648] *** VIC IRQ: vic_start_ok=1, v1_7=0x200000, v1_10=0x0 ***
[   25.977648] *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
[   25.977648] *** VIC ERROR: control limit error (bit 21) ***
[   26.801935] *** VIC IRQ: vic_start_ok=1, v1_7=0x200000, v1_10=0x0 ***
[   26.801935] *** VIC IRQ: No frame done interrupt (v1_7 & 1 = 0) ***
[   26.801935] *** VIC ERROR: control limit error (bit 21) ***
[STALL - No more interrupts after 4]
```

### After Fix (Expected)
```
[   25.977648] *** VIC IRQ: vic_start_ok=1, v1_7=0x1, v1_10=0x0 ***
[   25.977648] *** VIC IRQ: Frame done interrupt (v1_7 & 1 = 1) ***
[   26.011234] *** VIC IRQ: vic_start_ok=1, v1_7=0x1, v1_10=0x0 ***
[   26.011234] *** VIC IRQ: Frame done interrupt (v1_7 & 1 = 1) ***
[   26.044567] *** VIC IRQ: vic_start_ok=1, v1_7=0x1, v1_10=0x0 ***
[   26.044567] *** VIC IRQ: Frame done interrupt (v1_7 & 1 = 1) ***
[Continuous interrupts at ~30 FPS]
```

## Test Steps

### 1. Build and Deploy
```bash
cd /home/matteius/isp-latest/driver
make clean
make
scp *.ko root@192.168.50.211:/root/
```

### 2. Load Driver
```bash
ssh root@192.168.50.211
rmmod tx_isp 2>/dev/null
insmod tx_isp.ko
```

### 3. Check Kernel Logs
```bash
dmesg | tail -200 > /tmp/test_logs.txt
```

### 4. Verify Key Log Messages

#### A. VIC Input Configuration Preserved
Look for:
```
ispcore_link_setup: VIC input configured: 0x00000011
```
This should appear and NOT be followed by any writes to 0x380 from buffer code.

#### B. No Buffer Ring Initialization
Should NOT see:
```
VIC BUFFER RING: Initializing 5-buffer ring
VIC INIT: Set current address register 0x380
```

#### C. Frame Done Interrupts
Should see:
```
VIC IRQ: Frame done interrupt (v1_7 & 1 = 1)
```

#### D. No Control Limit Errors
Should NOT see:
```
VIC ERROR: control limit error (bit 21)
```

#### E. Continuous Interrupts
```bash
grep -c "VIC IRQ: vic_start_ok=1" /tmp/test_logs.txt
```
Should show many interrupts (not just 4!)

### 5. Check Interrupt Count
```bash
cat /proc/interrupts | grep isp
```
Should show:
```
 37:       XXXX   jz-intc  isp-m0
 38:       YYYY   jz-intc  isp-w02
```
Where YYYY >> 4 (many more than 4 interrupts)

### 6. Test Video Capture
```bash
# Start RTSP server
/root/your_rtsp_server &

# Check streams
ffprobe rtsp://192.168.50.211/ch0
ffprobe rtsp://192.168.50.211/ch1
```

Should show valid video streams with continuous frames.

## Success Criteria

✅ **VIC input configuration preserved** (register 0x380 not overwritten)
✅ **Frame done interrupts** (bit 0 set in v1_7)
✅ **No control limit errors** (bit 21 not set)
✅ **Continuous interrupts** (>> 4 interrupts)
✅ **Video streams working** (RTSP streams playable)

## Failure Scenarios

### If Still Getting 4/4 Stall
- Check if register 0x380 is being written elsewhere
- Verify ispcore_link_setup is being called
- Check CSI PHY is enabled (register 0x10022010 = 1)

### If Getting Different Errors
- Check for new kernel panics or crashes
- Verify buffer registers 0x318-0x328 are programmed
- Check vic_pipo_mdma_enable is being called

## Debug Commands

```bash
# Monitor interrupts in real-time
watch -n 1 'cat /proc/interrupts | grep isp'

# Monitor kernel logs
dmesg -w | grep -E "VIC IRQ|Frame done|control limit"

# Check CSI status
cat /sys/kernel/debug/tx-isp/csi/status

# Check VIC status  
cat /sys/kernel/debug/tx-isp/vic/status
```

## Rollback Plan

If the fix causes issues:
```bash
cd /home/matteius/isp-latest/driver
git checkout HEAD~1 tx_isp_vic.c
make
scp *.ko root@192.168.50.211:/root/
ssh root@192.168.50.211 'rmmod tx_isp && insmod /root/tx_isp.ko'
```

## Next Steps After Success

1. Document the fix in commit message
2. Update architecture documentation
3. Remove other custom buffer management code if not needed
4. Verify long-term stability (24+ hour test)

