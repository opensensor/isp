#!/bin/bash
# Test script for ISP clock tracking feature
# This script demonstrates how to use the /proc/jz/isp/clks file

set -e

echo "========================================="
echo "ISP Clock Tracking Test Script"
echo "========================================="
echo ""

# Check if proc file exists
if [ ! -f /proc/jz/isp/clks ]; then
    echo "ERROR: /proc/jz/isp/clks not found!"
    echo "Make sure the ISP driver is loaded."
    exit 1
fi

echo "1. Current Clock Status:"
echo "------------------------"
cat /proc/jz/isp/clks
echo ""

echo "2. Recent Clock Events (last 20):"
echo "----------------------------------"
dmesg | grep '\[CLK\]' | tail -20
echo ""

echo "3. Clock Enable Events:"
echo "-----------------------"
dmesg | grep '\[CLK\]' | grep 'Enabling' | tail -10
echo ""

echo "4. Clock Disable Events:"
echo "------------------------"
dmesg | grep '\[CLK\]' | grep 'Disabling' | tail -10
echo ""

echo "5. Clock Rates Summary:"
echo "-----------------------"
cat /proc/jz/isp/clks | grep 'rate='
echo ""

echo "6. Check for Missing Clocks:"
echo "----------------------------"
if cat /proc/jz/isp/clks | grep -q "NOT ACQUIRED"; then
    echo "WARNING: Some clocks are not acquired:"
    cat /proc/jz/isp/clks | grep "NOT ACQUIRED"
else
    echo "All clocks acquired successfully."
fi
echo ""

echo "========================================="
echo "Test Complete"
echo "========================================="
echo ""
echo "To monitor clock events in real-time, run:"
echo "  dmesg -w | grep '\\[CLK\\]'"
echo ""

