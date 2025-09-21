#!/bin/bash

# Script to show only the important trace messages
# Filters out verbose logging to focus on key register traces

echo "=== ISP Driver Clean Traces ==="
echo "Showing only CSI PHY, VIC interrupts, and register traces..."
echo ""

# Clear dmesg first if requested
if [ "$1" = "clear" ]; then
    dmesg -c > /dev/null
    echo "Cleared dmesg buffer. Run your test now, then run this script again."
    exit 0
fi

# Show filtered traces
dmesg | grep -E "(CSI PHY|VIC INTERRUPT|ISP isp-|write at offset|VIC FRAME|control limit)" | tail -50
