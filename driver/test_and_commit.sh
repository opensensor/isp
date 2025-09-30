#!/bin/bash
# Comprehensive test script with git history tracking
# This script builds, tests, analyzes results, and commits everything to git

set -e  # Exit on error

# Configuration
REMOTE_HOST="192.168.50.211"
REMOTE_PASS="Ami23plop"
DRIVER_DIR="/home/matteius/isp-latest/driver"
SDK_DIR="/home/matteius/isp-latest/external/ingenic-sdk"
REPO_ROOT="/home/matteius/isp-latest"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
TEST_LOG="${DRIVER_DIR}/test_results_${TIMESTAMP}.txt"
ANALYSIS_LOG="${DRIVER_DIR}/analysis_${TIMESTAMP}.txt"

echo "=========================================="
echo "ISP DRIVER TEST & COMMIT CYCLE"
echo "Timestamp: ${TIMESTAMP}"
echo "=========================================="

# Step 1: Build the driver
echo ""
echo "=== STEP 1: Building driver ==="
cd "${REPO_ROOT}"
cp -rf driver/* external/ingenic-sdk/3.10/isp/t31/
cd "${SDK_DIR}"

# Touch modified files to force rebuild
touch 3.10/isp/t31/tx_isp_vic.c
touch 3.10/isp/t31/tx_isp_subdev.c
touch 3.10/isp/t31/tx_isp_vin.c

export CROSS_COMPILE=mipsel-linux-
export KDIR=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/build/linux-4fb8bc9f91c2951629f818014b7d3b5cc2a1ec81/
export PATH=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/per-package/toolchain-external-custom/host/bin/:$PATH

SENSOR_MODEL=gc2053 ./build.sh t31 > /tmp/build_${TIMESTAMP}.log 2>&1

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    tail -50 /tmp/build_${TIMESTAMP}.log
    exit 1
fi

echo "Build successful!"
ls -lh tx-isp-t31.ko sensor_gc2053_t31.ko
md5sum tx-isp-t31.ko sensor_gc2053_t31.ko

# Step 2: Reboot device
echo ""
echo "=== STEP 2: Rebooting device ==="
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'reboot' || true
echo "Waiting 35 seconds for device to come back up..."
sleep 35

# Step 3: Run test
echo ""
echo "=== STEP 3: Running test ==="
"${DRIVER_DIR}/test_driver.sh"

# Step 4: Analyze results
echo ""
echo "=== STEP 4: Analyzing results ==="

cat > "${ANALYSIS_LOG}" << 'EOF'
========================================
ISP DRIVER TEST ANALYSIS
========================================

EOF

echo "Timestamp: ${TIMESTAMP}" >> "${ANALYSIS_LOG}"
echo "Build: $(md5sum ${SDK_DIR}/tx-isp-t31.ko | awk '{print $1}')" >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Extract key metrics
echo "=== KEY METRICS ===" >> "${ANALYSIS_LOG}"
echo "VIC START messages: $(grep -c 'VIC START' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "VIN linked: $(grep -c 'LINKED VIN device' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "VIN input configured: $(grep -c 'VIN input configured' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "CSI input configured: $(grep -c 'CSI input configured' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "VIC input configured: $(grep -c 'VIC input source configured' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "Complete pipeline configured: $(grep -c 'Complete pipeline configured' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "Control limit errors: $(grep -c 'control limit error' ${DRIVER_DIR}/logs.txt || echo 0)" >> "${ANALYSIS_LOG}"
echo "Frame done interrupts: $(grep 'Frame done:' ${DRIVER_DIR}/logs.txt | tail -1 | awk '{print $NF}')" >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Extract interrupt counts
echo "=== INTERRUPT COUNTS ===" >> "${ANALYSIS_LOG}"
grep -E "isp-m0|isp-w02" ${DRIVER_DIR}/logs.txt | tail -2 >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Extract pipeline configuration
echo "=== PIPELINE CONFIGURATION ===" >> "${ANALYSIS_LOG}"
grep -E "VIN input configured|CSI input configured|VIC input source configured" ${DRIVER_DIR}/logs.txt | head -10 >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Extract errors
echo "=== ERRORS ===" >> "${ANALYSIS_LOG}"
grep -i "error\|fail" ${DRIVER_DIR}/logs.txt | grep -v "Frame done: 0" | head -20 >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Extract register writes
echo "=== KEY REGISTER WRITES ===" >> "${ANALYSIS_LOG}"
grep -E "VIN\[0x10\]|CSI\[0x20\]|VIC\[0x380\]|ISP\[0x800\]|ISP\[0x804\]" ${DRIVER_DIR}/logs.txt | head -20 >> "${ANALYSIS_LOG}"
echo "" >> "${ANALYSIS_LOG}"

# Display analysis
cat "${ANALYSIS_LOG}"

# Step 5: Commit to git
echo ""
echo "=== STEP 5: Committing to git ==="
cd "${REPO_ROOT}"

# Copy logs to driver directory for commit
cp "${DRIVER_DIR}/logs.txt" "${DRIVER_DIR}/logs_${TIMESTAMP}.txt"

# Create commit message
COMMIT_MSG="Test run ${TIMESTAMP}

Build: $(md5sum ${SDK_DIR}/tx-isp-t31.ko | awk '{print $1}')

Key metrics:
- VIN linked: $(grep -c 'LINKED VIN device' ${DRIVER_DIR}/logs.txt || echo 0)
- VIN input configured: $(grep -c 'VIN input configured' ${DRIVER_DIR}/logs.txt || echo 0)
- CSI input configured: $(grep -c 'CSI input configured' ${DRIVER_DIR}/logs.txt || echo 0)
- VIC input configured: $(grep -c 'VIC input source configured' ${DRIVER_DIR}/logs.txt || echo 0)
- Control limit errors: $(grep -c 'control limit error' ${DRIVER_DIR}/logs.txt || echo 0)
- Frame done: $(grep 'Frame done:' ${DRIVER_DIR}/logs.txt | tail -1 | awk '{print $NF}')
- Interrupts: $(grep 'isp-m0' ${DRIVER_DIR}/logs.txt | tail -1 | awk '{print $2}')

Status: $(if [ $(grep 'Frame done:' ${DRIVER_DIR}/logs.txt | tail -1 | awk '{print $NF}') -gt 0 ]; then echo 'SUCCESS - Frames captured!'; else echo 'FAILED - No frames captured'; fi)"

# Add files
git add driver/*.c driver/*.h driver/*.sh
git add "${DRIVER_DIR}/logs_${TIMESTAMP}.txt"
git add "${ANALYSIS_LOG}"

# Commit
git commit -m "${COMMIT_MSG}" || echo "Nothing to commit or commit failed"

echo ""
echo "=========================================="
echo "TEST CYCLE COMPLETE"
echo "=========================================="
echo "Logs saved to:"
echo "  - ${DRIVER_DIR}/logs_${TIMESTAMP}.txt"
echo "  - ${ANALYSIS_LOG}"
echo ""
echo "Git commit created with test results"
echo ""

