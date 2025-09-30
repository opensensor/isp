#!/bin/bash

# Complete driver test script with proper dmesg capture timing
# Usage: ./test_driver.sh

REMOTE_HOST="${REMOTE_HOST:-192.168.50.211}"
REMOTE_PASS="${REMOTE_PASS:-Ami23plop}"
OUTPUT_FILE="/home/matteius/isp-latest/driver/logs.txt"
MODULE_DIR="/home/matteius/isp-latest/external/ingenic-sdk"

echo "=== ISP DRIVER TEST SCRIPT ==="
echo "Remote host: $REMOTE_HOST"
echo "Output file: $OUTPUT_FILE"
echo ""

# Clear output file
> "$OUTPUT_FILE"

echo "Step 1: Uploading modules..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'cat > /tmp/tx-isp-t31.ko' < "$MODULE_DIR/tx-isp-t31.ko"
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'cat > /tmp/sensor_gc2053_t31.ko' < "$MODULE_DIR/sensor_gc2053_t31.ko"
echo "Modules uploaded"
echo ""

echo "Step 2: Stopping prudynt and unloading old modules..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'killall prudynt 2>/dev/null; /etc/init.d/S95prudynt stop 2>/dev/null; rmmod sensor_gc2053_t31 2>/dev/null; rmmod tx_isp_t31 2>/dev/null'
echo "Old modules unloaded"
echo ""

echo "Step 3: Loading new modules..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'insmod /tmp/tx-isp-t31.ko isp_clk=200000000 print_level=1 && insmod /tmp/sensor_gc2053_t31.ko sensor_max_fps=30 data_interface=1'
echo "New modules loaded"
echo ""

echo "Step 4: Capturing dmesg after module load..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'dmesg' >> "$OUTPUT_FILE"
echo "Captured dmesg after module load"
echo ""

echo "Step 5: Starting prudynt (non-blocking)..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'prudynt > /dev/null 2>&1 &'
echo "Prudynt started"
echo ""

echo "Step 6: Immediate dmesg capture (no wait)..."
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'dmesg' >> "$OUTPUT_FILE"
echo "Captured immediate dmesg"
echo ""

echo "Step 7: Wait 0.5s and capture dmesg..."
sleep 0.5
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'dmesg' >> "$OUTPUT_FILE"
echo "Captured dmesg after 0.5s"
echo ""

echo "Step 8: Scraping dmesg every 1 second for 10 iterations..."
for i in $(seq 1 10); do
    echo "  Scraping dmesg... ($i/10)"
    sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'dmesg' >> "$OUTPUT_FILE"
    if [ $i -lt 10 ]; then
        sleep 1
    fi
done
echo "Dmesg scraping complete"
echo ""

echo "Step 9: Checking interrupts..."
echo "=== INTERRUPTS BEFORE ===" >> "$OUTPUT_FILE"
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'cat /proc/interrupts' >> "$OUTPUT_FILE"
echo "Waiting 5 seconds..."
sleep 5
echo "=== INTERRUPTS AFTER 5 SEC ===" >> "$OUTPUT_FILE"
sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'cat /proc/interrupts' >> "$OUTPUT_FILE"
echo "Interrupt check complete"
echo ""

echo "=== TEST COMPLETE ==="
echo "Total lines captured: $(wc -l < "$OUTPUT_FILE")"
echo "Logs saved to: $OUTPUT_FILE"
echo ""
echo "Quick analysis:"
echo "  VIC START messages: $(grep -c "VIC START" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  VIC input config: $(grep -c "VIC input" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  vic_start_ok=1: $(grep -c "vic_start_ok=1" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  Frame done: $(grep -c "Frame done" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  Control limit errors: $(grep -c "control limit error" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  ISP interrupts: $(grep "isp-" "$OUTPUT_FILE" | tail -2)"

