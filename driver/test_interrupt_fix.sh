#!/bin/bash
# Test script for ISP interrupt stall fix
# This script helps verify that the CSI interface check fix resolves the interrupt stall issue

set -e

echo "=========================================="
echo "ISP Interrupt Stall Fix - Test Script"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    case $status in
        "success")
            echo -e "${GREEN}✓${NC} $message"
            ;;
        "error")
            echo -e "${RED}✗${NC} $message"
            ;;
        "info")
            echo -e "${YELLOW}ℹ${NC} $message"
            ;;
    esac
}

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    print_status "error" "Please run as root (sudo)"
    exit 1
fi

print_status "info" "Step 1: Rebuilding driver..."
cd /home/matteius/isp-latest/driver
make clean > /dev/null 2>&1
if make > /tmp/build.log 2>&1; then
    print_status "success" "Driver built successfully"
else
    print_status "error" "Build failed. Check /tmp/build.log"
    cat /tmp/build.log
    exit 1
fi

print_status "info" "Step 2: Unloading old driver..."
rmmod tx-isp-module 2>/dev/null || true
sleep 1

print_status "info" "Step 3: Loading new driver..."
if insmod tx-isp-module.ko; then
    print_status "success" "Driver loaded"
else
    print_status "error" "Failed to load driver"
    dmesg | tail -20
    exit 1
fi

print_status "info" "Step 4: Monitoring interrupts for 30 seconds..."
echo ""
echo "Looking for key indicators:"
echo "  - 'Sensor interface type is MIPI (1) - proceeding'"
echo "  - 'CSI state set to 4 (enable=1)'"
echo "  - 'vic_mdma_irq_function: channel=0 processing MDMA interrupt'"
echo "  - 'Info[VIC_MDAM_IRQ] : channel[0] frame done'"
echo ""

# Clear dmesg buffer
dmesg -c > /dev/null

# Start monitoring in background
timeout 30 dmesg -w | grep -E "csi_video_s_stream|vic_mdma_irq|VIC_MDAM_IRQ|interrupt" > /tmp/interrupt_log.txt 2>&1 &
MONITOR_PID=$!

# Wait for monitoring to complete
sleep 30
kill $MONITOR_PID 2>/dev/null || true

print_status "info" "Step 5: Analyzing results..."
echo ""

# Count interrupts
INTERRUPT_COUNT=$(grep -c "VIC_MDAM_IRQ" /tmp/interrupt_log.txt 2>/dev/null || echo "0")
CSI_MIPI_OK=$(grep -c "Sensor interface type is MIPI (1) - proceeding" /tmp/interrupt_log.txt 2>/dev/null || echo "0")
CSI_STATE_OK=$(grep -c "CSI state set to 4" /tmp/interrupt_log.txt 2>/dev/null || echo "0")
INTERFACE_ERROR=$(grep -c "Interface type.*!= 1.*returning 0" /tmp/interrupt_log.txt 2>/dev/null || echo "0")

echo "Results:"
echo "  - Total MDMA interrupts: $INTERRUPT_COUNT"
echo "  - CSI MIPI check passed: $CSI_MIPI_OK times"
echo "  - CSI state set correctly: $CSI_STATE_OK times"
echo "  - Interface type errors: $INTERFACE_ERROR"
echo ""

# Determine success/failure
SUCCESS=true

if [ "$INTERRUPT_COUNT" -lt 20 ]; then
    print_status "error" "Too few interrupts ($INTERRUPT_COUNT < 20) - System may be stalling"
    SUCCESS=false
else
    print_status "success" "Good interrupt count ($INTERRUPT_COUNT >= 20)"
fi

if [ "$CSI_MIPI_OK" -eq 0 ]; then
    print_status "error" "CSI MIPI check not found - May indicate initialization issue"
    SUCCESS=false
else
    print_status "success" "CSI MIPI check passed"
fi

if [ "$CSI_STATE_OK" -eq 0 ]; then
    print_status "error" "CSI state not set correctly"
    SUCCESS=false
else
    print_status "success" "CSI state set correctly"
fi

if [ "$INTERFACE_ERROR" -gt 0 ]; then
    print_status "error" "Interface type errors detected ($INTERFACE_ERROR)"
    SUCCESS=false
else
    print_status "success" "No interface type errors"
fi

echo ""
echo "=========================================="
if [ "$SUCCESS" = true ]; then
    print_status "success" "TEST PASSED - Fix appears to be working!"
    echo ""
    echo "The driver is getting continuous interrupts and CSI is properly configured."
    echo "You can now test with your application."
else
    print_status "error" "TEST FAILED - Issue may not be fully resolved"
    echo ""
    echo "The fix may need additional work. Check the logs below:"
    echo ""
    echo "Last 50 lines of interrupt log:"
    tail -50 /tmp/interrupt_log.txt
    echo ""
    echo "Full log saved to: /tmp/interrupt_log.txt"
    echo "Full dmesg saved to: /tmp/dmesg_full.txt"
    dmesg > /tmp/dmesg_full.txt
fi
echo "=========================================="

exit 0

