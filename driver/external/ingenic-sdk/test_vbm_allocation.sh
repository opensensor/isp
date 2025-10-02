#!/bin/bash

# Test VBM buffer allocation by checking /tmp files created by libimp.so

CAMERA_IP="192.168.50.211"
CAMERA_USER="root"
CAMERA_PASS="ismart12"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
LOG_DIR="/tmp/isp_test_vbm_${TIMESTAMP}"

echo "=== VBM Buffer Allocation Test ==="
echo "Timestamp: $(date)"
echo "Log Directory: ${LOG_DIR}"
echo ""

# Create log directory
mkdir -p "${LOG_DIR}"

# Function to run SSH commands
run_ssh() {
    sshpass -p "${CAMERA_PASS}" ssh -o StrictHostKeyChecking=no "${CAMERA_USER}@${CAMERA_IP}" "$@"
}

# Function to copy files from camera
copy_from_camera() {
    sshpass -p "${CAMERA_PASS}" scp -o StrictHostKeyChecking=no "${CAMERA_USER}@${CAMERA_IP}:$1" "$2"
}

echo "[1/8] Uploading kernel modules..."
sshpass -p "${CAMERA_PASS}" scp -o StrictHostKeyChecking=no \
    /home/matteius/isp-latest/external/ingenic-sdk/tx-isp-t31.ko \
    /home/matteius/isp-latest/external/ingenic-sdk/sensor_gc2053_t31.ko \
    "${CAMERA_USER}@${CAMERA_IP}:/tmp/"

echo "[2/8] Removing old /tmp files..."
run_ssh "rm -f /tmp/alloc_manager_info /tmp/continuous_mem_info /tmp/isp_tuning_func"

echo "[3/8] Loading ISP modules..."
run_ssh "insmod /tmp/tx-isp-t31.ko isp_clk=100000000 && insmod /tmp/sensor_gc2053_t31.ko"
sleep 2

echo "[4/8] Checking device nodes..."
run_ssh "ls -la /dev/tx-isp /dev/isp-m0 /dev/tisp /dev/framechan*" | tee "${LOG_DIR}/device_nodes.txt"

echo "[5/8] Capturing initial dmesg..."
run_ssh "dmesg" > "${LOG_DIR}/dmesg_before_prudynt.log"

echo "[6/8] Starting prudynt..."
run_ssh "prudynt &"
sleep 5

echo "[7/8] Checking for /tmp files created by libimp.so..."
run_ssh "ls -la /tmp/" | tee "${LOG_DIR}/tmp_files.txt"

echo "[8/8] Capturing VBM allocation info..."
if run_ssh "test -f /tmp/alloc_manager_info"; then
    echo "✓ /tmp/alloc_manager_info exists!"
    copy_from_camera "/tmp/alloc_manager_info" "${LOG_DIR}/alloc_manager_info.txt"
    echo ""
    echo "=== VBM Pool Allocations ==="
    grep -A6 "VBMPool" "${LOG_DIR}/alloc_manager_info.txt"
else
    echo "✗ /tmp/alloc_manager_info NOT found - libimp.so may have failed to initialize"
fi

if run_ssh "test -f /tmp/continuous_mem_info"; then
    echo "✓ /tmp/continuous_mem_info exists!"
    copy_from_camera "/tmp/continuous_mem_info" "${LOG_DIR}/continuous_mem_info.txt"
else
    echo "✗ /tmp/continuous_mem_info NOT found"
fi

echo ""
echo "Capturing dmesg after prudynt..."
run_ssh "dmesg" > "${LOG_DIR}/dmesg_after_prudynt.log"

echo ""
echo "Checking for prudynt errors..."
grep -i "error\|fail\|cannot" "${LOG_DIR}/dmesg_after_prudynt.log" | tail -20

echo ""
echo "=== Test Complete ==="
echo "Logs saved to: ${LOG_DIR}"
echo ""
echo "To analyze:"
echo "  cat ${LOG_DIR}/alloc_manager_info.txt"
echo "  cat ${LOG_DIR}/continuous_mem_info.txt"
echo "  grep VBMPool ${LOG_DIR}/alloc_manager_info.txt"

