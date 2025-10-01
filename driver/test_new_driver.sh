#!/bin/bash

# Test script for new driver with vic_mdma_enable fix
# Connects to device, loads modules, starts streamer, monitors dmesg and trace

set -e

REMOTE_HOST="${REMOTE_HOST:-192.168.50.211}"
REMOTE_USER="root"
REMOTE_PASS="Ami23plop"
REMOTE_PATH="/tmp"
DMESG_INTERVAL=0.5
MONITOR_ITERATIONS=10

echo "=== ISP Driver Test Script ==="
echo "Remote host: $REMOTE_HOST"
echo "Monitoring interval: ${DMESG_INTERVAL}s"
echo "Monitor iterations: ${MONITOR_ITERATIONS}"
echo ""

# Function to run remote command with sshpass
run_remote() {
    sshpass -p "$REMOTE_PASS" ssh -o StrictHostKeyChecking=no "$REMOTE_USER@$REMOTE_HOST" "$@"
}

# Function to get file from remote
get_remote_file() {
    sshpass -p "$REMOTE_PASS" scp -O -o StrictHostKeyChecking=no "$REMOTE_USER@$REMOTE_HOST:$1" "$2"
}

# Function to upload file to remote
upload_file() {
    sshpass -p "$REMOTE_PASS" scp -O -o StrictHostKeyChecking=no "$1" "$REMOTE_USER@$REMOTE_HOST:$2"
}

echo "[1/9] Uploading trace module..."
if [ -f "external/ingenic-sdk/tx-isp-trace.ko" ]; then
    upload_file "external/ingenic-sdk/tx-isp-trace.ko" "$REMOTE_PATH/"
    echo "  ✓ Trace module uploaded"
else
    echo "  ⚠️  WARNING: Trace module not found, skipping"
fi

echo "[2/9] Removing old trace file..."
run_remote "rm -f /opt/trace.txt"
echo "  ✓ Old trace removed"

echo "[3/9] Checking if modules are on device..."
run_remote "ls -lh $REMOTE_PATH/*.ko" || {
    echo "ERROR: Modules not found on device. Please run build script first."
    exit 1
}

echo "[4/9] Unloading old modules (if any)..."
run_remote "rmmod sensor_gc2053_t31 2>/dev/null || true"
run_remote "rmmod tx_isp_t31 2>/dev/null || true"
run_remote "rmmod tx_isp_trace 2>/dev/null || true"
run_remote "killall prudynt 2>/dev/null || true"
sleep 1

echo "[5/9] Clearing dmesg..."
run_remote "dmesg -c > /dev/null"

echo "[6/9] Loading modules in order..."
echo "  - Loading tx-isp-trace.ko (trace driver)..."
run_remote "insmod $REMOTE_PATH/tx-isp-trace.ko" || {
    echo "  ⚠️  WARNING: Failed to load tx-isp-trace.ko (may not exist)"
}

echo "  - Loading tx-isp-t31.ko (main ISP driver)..."
run_remote "insmod $REMOTE_PATH/tx-isp-t31.ko isp_clk=100000000" || {
    echo "ERROR: Failed to load tx-isp-t31.ko"
    run_remote "dmesg | tail -50"
    exit 1
}

echo "  - Loading sensor_gc2053_t31.ko (sensor driver)..."
run_remote "insmod $REMOTE_PATH/sensor_gc2053_t31.ko" || {
    echo "ERROR: Failed to load sensor_gc2053_t31.ko"
    run_remote "dmesg | tail -50"
    exit 1
}

echo "[7/9] Waiting for modules to initialize (2 seconds)..."
sleep 2

echo "[8/9] Capturing dmesg BEFORE starting streamer..."
# Create output directory
mkdir -p driver/test_results
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
RESULT_DIR="driver/test_results/${TIMESTAMP}"
mkdir -p "$RESULT_DIR"

run_remote "dmesg" > "$RESULT_DIR/dmesg_init.txt"
echo "  ✓ Initial dmesg saved to dmesg_init.txt"

echo "[9/9] Starting streamer (prudynt) in background..."
run_remote "prudynt > /tmp/prudynt.log 2>&1 &"
sleep 2

echo ""
echo "=== Monitoring dmesg and trace for ${MONITOR_ITERATIONS} iterations ==="
echo "    Press Ctrl+C to stop early"
echo ""

# Monitor dmesg in intervals
for ITERATION in $(seq 1 $MONITOR_ITERATIONS); do
    echo "[Iteration $ITERATION/$MONITOR_ITERATIONS] Capturing dmesg..."
    run_remote "dmesg | tail -100" > "$RESULT_DIR/dmesg_${ITERATION}.txt"

    # Check for critical errors
    if grep -i "error\|fail\|bug\|oops\|panic" "$RESULT_DIR/dmesg_${ITERATION}.txt" > /dev/null 2>&1; then
        echo "  ⚠️  WARNING: Errors detected in dmesg!"
    fi

    # Check for VIC-related messages
    if grep -i "vic_mdma_enable\|vic_core_s_stream\|VIC PRIMARY" "$RESULT_DIR/dmesg_${ITERATION}.txt" > /dev/null 2>&1; then
        echo "  ✓ VIC activity detected"
    fi

    # Check for CSI initialization
    if grep -i "csi_core_ops_init\|CSI MIPI" "$RESULT_DIR/dmesg_${ITERATION}.txt" > /dev/null 2>&1; then
        echo "  ✓ CSI initialization detected"
    fi

    sleep $DMESG_INTERVAL
done

echo ""
echo "=== Collecting final results ==="

echo "Fetching final dmesg..."
run_remote "dmesg" > "$RESULT_DIR/dmesg_final.txt"

echo "Fetching /opt/trace.txt..."
get_remote_file "/opt/trace.txt" "$RESULT_DIR/trace.txt" 2>/dev/null || {
    echo "  Note: /opt/trace.txt not found (trace driver may not have captured data)"
}

echo "Fetching prudynt log..."
get_remote_file "/tmp/prudynt.log" "$RESULT_DIR/prudynt.log" 2>/dev/null || {
    echo "  Note: prudynt.log not found"
}

echo "Checking module status..."
run_remote "lsmod" > "$RESULT_DIR/lsmod.txt"

echo "Checking /proc/jz/isp..."
run_remote "ls -la /proc/jz/isp/" > "$RESULT_DIR/proc_jz_isp.txt" 2>/dev/null || {
    echo "  Note: /proc/jz/isp not found"
}

echo ""
echo "=== Analysis ==="

# Analyze results
echo "Checking for vic_mdma_enable control register fix..."
if grep "vic_mdma_enable: VIC PRIMARY\[0x300\] = 0x" "$RESULT_DIR/dmesg_final.txt" > /dev/null; then
    CONTROL_VALUE=$(grep "vic_mdma_enable: VIC PRIMARY\[0x300\] = 0x" "$RESULT_DIR/dmesg_final.txt" | tail -1 | sed 's/.*0x\([0-9a-f]*\).*/\1/')
    echo "  ✓ VIC control register value: 0x$CONTROL_VALUE"
    
    # Check if it's the correct value (should be 0x80000020 + format, NOT with buffer_count in bits 16-19)
    if [[ "$CONTROL_VALUE" =~ ^8000002[0-9a-f]$ ]]; then
        echo "  ✓ CORRECT: Control register uses base 0x80000020 (manual buffer index)"
    else
        echo "  ⚠️  WARNING: Control register value unexpected"
    fi
else
    echo "  ⚠️  WARNING: vic_mdma_enable not called or logged"
fi

echo ""
echo "Checking for CONTROL bank writes..."
if grep "vic_mdma_enable: CONTROL bank" "$RESULT_DIR/dmesg_final.txt" > /dev/null; then
    echo "  ✓ CONTROL bank write detected (fix applied)"
else
    echo "  ⚠️  WARNING: CONTROL bank write not detected"
fi

echo ""
echo "Checking for buffer clear operations..."
if grep "ispvic_frame_channel_clearbuf" "$RESULT_DIR/dmesg_final.txt" > /dev/null; then
    echo "  ✓ Buffer clear function called"
else
    echo "  Note: Buffer clear not called (may not be needed yet)"
fi

echo ""
echo "Checking trace.txt..."
if [ -f "$RESULT_DIR/trace.txt" ]; then
    TRACE_LINES=$(wc -l < "$RESULT_DIR/trace.txt")
    echo "  ✓ Trace captured: $TRACE_LINES lines"
    
    # Check for key VIC writes
    if grep "VIC Control.*0x300" "$RESULT_DIR/trace.txt" > /dev/null; then
        echo "  ✓ VIC Control writes detected in trace"
    fi
    
    if grep "Core Control.*0xb0" "$RESULT_DIR/trace.txt" > /dev/null; then
        echo "  ✓ Core Control writes detected in trace"
    fi
else
    echo "  ⚠️  No trace.txt captured"
fi

echo ""
echo "=== Results Summary ==="
echo "Results saved to: $RESULT_DIR"
echo ""
echo "Files:"
ls -lh "$RESULT_DIR/"
echo ""

# Create summary file
cat > "$RESULT_DIR/SUMMARY.txt" << EOF
ISP Driver Test Results
=======================
Timestamp: $TIMESTAMP
Remote Host: $REMOTE_HOST
Monitor Duration: ${MONITOR_DURATION}s

Files Generated:
- dmesg_final.txt: Complete dmesg output
- trace.txt: Hardware trace from /opt/trace.txt
- prudynt.log: Streamer output
- lsmod.txt: Loaded modules
- proc_jz_isp.txt: /proc/jz/isp directory listing
- dmesg_*.txt: Periodic dmesg snapshots

Key Checks:
$(grep -c "csi_core_ops_init" "$RESULT_DIR/dmesg_init.txt" 2>/dev/null || echo "0") csi_core_ops_init calls (init)
$(grep -c "vic_mdma_enable" "$RESULT_DIR/dmesg_final.txt" 2>/dev/null || echo "0") vic_mdma_enable calls
$(grep -c "vic_core_s_stream" "$RESULT_DIR/dmesg_final.txt" 2>/dev/null || echo "0") vic_core_s_stream calls
$(grep -c "CONTROL bank" "$RESULT_DIR/dmesg_final.txt" 2>/dev/null || echo "0") CONTROL bank writes
$(grep -c "ERROR\|error" "$RESULT_DIR/dmesg_final.txt" 2>/dev/null || echo "0") errors in dmesg

Next Steps:
1. Review dmesg_final.txt for any errors
2. Compare trace.txt with reference-trace.txt
3. Check if VIC control register values are correct
4. Verify MIPI lane configuration is preserved
EOF

echo "Summary written to: $RESULT_DIR/SUMMARY.txt"
echo ""
echo "=== Test Complete ==="
echo ""
echo "To compare with reference trace:"
echo "  diff -u driver/reference-trace.txt $RESULT_DIR/trace.txt | less"
echo ""
echo "To view full dmesg:"
echo "  less $RESULT_DIR/dmesg_final.txt"

