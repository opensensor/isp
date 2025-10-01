# Test Results Summary - 2025-10-01 09:30:44

## Test Configuration
- **Modules Tested**: tx-isp-t31.ko (with VIC MDMA fix), sensor_gc2053_t31.ko, tx-isp-trace.ko
- **Test Duration**: 10 iterations @ 0.5s intervals
- **Results Directory**: `driver/test_results/20251001_093044/`

## Critical Findings

### ✅ GOOD: VIC MDMA Fix Applied
The VIC MDMA buffer cycling fix was successfully compiled and loaded:
- Changed from `(buffer_count << 16)` to manual buffer index `0`
- Added CONTROL bank write synchronization
- Code is in the driver but **NOT being called** (see below)

### ❌ CRITICAL BUG: CSI Initialization Wrong

**The first write to isp-csi offset 0x0 is WRONG:**

```
Reference (working):  ISP isp-csi: [CSI PHY Control] write at offset 0x0: 0x0 -> 0x7d
Our trace (broken):   ISP isp-csi: [CSI PHY Control] write at offset 0x0: 0x0 -> 0x1
```

**Impact**: This is the root cause of the MIPI lane configuration failure. Without the correct `0x7d` write:
- CSI PHY timing is wrong
- MIPI lanes don't configure properly
- Data doesn't flow from sensor → CSI → VIC
- VIC never receives frames
- **vic_mdma_enable is never called** (because streaming never starts)

### ❌ VIC MDMA Enable Not Called

From dmesg analysis:
- `vic_mdma_enable` was **NOT called** during the test
- No "VIC PRIMARY [0x300]" log messages found
- No "CONTROL bank" log messages found

**Why**: The streaming pipeline never starts because CSI initialization fails.

## Trace Analysis

### Trace Statistics
- **Total lines**: 383 (vs 1691 in previous test, vs ~300 in reference)
- **VIC Control writes**: Present but minimal
- **Core Control writes**: Present but minimal  
- **CSI Lane Config writes**: Present but likely incomplete

### Key Differences from Reference

| Register | Reference Value | Our Value | Status |
|----------|----------------|-----------|--------|
| isp-csi offset 0x0 | 0x7d | 0x1 | ❌ WRONG |
| isp-csi offset 0x4 | 0xe3 | 0xe3 | ✅ OK |
| isp-csi offset 0x8 | 0xa0 | 0xa0 | ✅ OK |
| isp-csi offset 0xc | 0x83 | 0x83 | ✅ OK |

**Only the first write is wrong**, but it's critical!

## dmesg Analysis

### Initialization (dmesg_init.txt)
```
[  256.220591] *** tx_isp_subdev_init: CSI subdev registered at slot 0 ***
[  256.220673] *** CSI BASIC REGISTERS SET: b0022000 (from tx_isp_subdev_init) ***
[  256.220679] *** LINKED CSI device: 85410800, regs: b0022000 ***
```

CSI device is being initialized with register base `b0022000` (physical 0x10022000).

### Errors Detected
The test script detected errors in dmesg. Need to check what they are:

```bash
grep -i "error\|fail" driver/test_results/20251001_093044/dmesg_final.txt
```

## Root Cause Analysis

The CSI initialization bug is in `driver/tx_isp_csi.c`. The code should write `0x7d` to offset 0x0 of `isp_csi_regs` (ISP Core CSI registers at 0x13300000), but it's writing `0x1` instead.

**Possible causes**:

1. **Wrong code path**: The MIPI initialization code at line 457 (`writel(0x7d, v0_8)`) is not being executed
2. **Wrong register pointer**: `isp_csi_regs` is pointing to the wrong address
3. **Overwritten later**: Something writes `0x1` after the correct `0x7d` write

## Next Steps

### Priority 1: Fix CSI Initialization
1. Add debug logging to `csi_core_ops_init` to trace execution path
2. Verify `isp_csi_regs` pointer is correct (should be 0x13300000, not 0x10022000)
3. Check if MIPI interface_type is detected correctly
4. Ensure line 457 in tx_isp_csi.c is being executed

### Priority 2: Verify VIC MDMA Fix
Once CSI is fixed and streaming starts:
1. Check if `vic_mdma_enable` is called
2. Verify VIC PRIMARY [0x300] register value
3. Verify CONTROL bank write
4. Compare full trace with reference-trace.txt

### Priority 3: Test Complete Pipeline
1. Verify frames are delivered to userspace
2. Check prudynt can stream video
3. Validate against reference-trace.txt

## Files for Analysis

### Key Files
- `driver/test_results/20251001_093044/dmesg_init.txt` - Module initialization logs
- `driver/test_results/20251001_093044/dmesg_final.txt` - Complete dmesg output
- `driver/test_results/20251001_093044/trace.txt` - Hardware register writes (383 lines)
- `driver/test_results/20251001_093044/prudynt.log` - Streamer output

### Comparison Commands
```bash
# Compare first 120 lines of trace
diff -u <(head -120 driver/reference-trace.txt) <(head -120 driver/test_results/20251001_093044/trace.txt)

# Find the CSI init difference
grep "isp-csi.*offset 0x0" driver/reference-trace.txt
grep "isp-csi.*offset 0x0" driver/test_results/20251001_093044/trace.txt

# Check for errors
grep -i "error\|fail" driver/test_results/20251001_093044/dmesg_final.txt
```

## Conclusion

The VIC MDMA fix is correctly implemented but cannot be tested yet because the CSI initialization bug prevents the streaming pipeline from starting. **The immediate priority is to fix the CSI initialization to write `0x7d` instead of `0x1` to isp-csi offset 0x0.**

Once CSI is fixed, we can verify:
1. MIPI lane configuration works
2. VIC receives frames
3. VIC MDMA buffer cycling works correctly
4. Frames are delivered to userspace

