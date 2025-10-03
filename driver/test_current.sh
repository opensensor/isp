#!/bin/bash
# Automated ISP Driver Test Script - Current Fix Test
# Tests the buffer count fix and gate enable fix

set -e

HOST="192.168.50.211"
PASSWORD="Ami23plop"
LOG_DIR="/tmp/isp_test_current_$(date +%Y%m%d_%H%M%S)"

echo "=== ISP Driver Current Fix Test ==="
echo "Log directory: $LOG_DIR"
mkdir -p "$LOG_DIR"

# Step 1: Reboot camera for clean state
echo "[1/12] Rebooting camera for clean state..."
sshpass -p "$PASSWORD" ssh root@$HOST "reboot" 2>&1 || echo "Reboot command sent"
echo "Waiting 30 seconds for camera to reboot..."
sleep 30

# Step 2: Wait for camera to be ready
echo "[2/12] Waiting for camera to be ready..."
for i in {1..10}; do
    if sshpass -p "$PASSWORD" ssh root@$HOST "echo ready" 2>/dev/null; then
        echo "Camera is ready!"
        break
    fi
    echo "  Attempt $i/10..."
    sleep 3
done

# Step 3: Upload modules to /opt (persists across reboots)
echo "[3/12] Uploading modules to /opt..."
cd /home/matteius/isp-latest/external/ingenic-sdk
sshpass -p "$PASSWORD" scp -O tx-isp-trace.ko tx-isp-t31.ko sensor_gc2053_t31.ko root@$HOST:/opt/ 2>&1 | tee "$LOG_DIR/upload.log"

# Step 4: Load modules
echo "[4/12] Loading ISP modules..."
sshpass -p "$PASSWORD" ssh root@$HOST "cd /opt && insmod tx-isp-trace.ko && insmod tx-isp-t31.ko isp_clk=100000000 && insmod sensor_gc2053_t31.ko" 2>&1 | tee "$LOG_DIR/insmod.log"

# Step 5: Capture initial dmesg
echo "[5/12] Capturing initial dmesg..."
sleep 2
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg | tail -300" > "$LOG_DIR/dmesg_initial.log"

# Step 6: Run prudynt in background
echo "[6/12] Starting prudynt..."
sshpass -p "$PASSWORD" ssh root@$HOST "nohup prudynt > /dev/null 2>&1 &" 2>&1 | tee "$LOG_DIR/prudynt_start.log"
sleep 3

# Step 7: Capture dmesg after prudynt
echo "[7/12] Capturing dmesg after prudynt..."
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg | tail -500" > "$LOG_DIR/dmesg_after_prudynt.log"

# Step 8: Monitor dmesg 10 times with 1s intervals
echo "[8/12] Monitoring dmesg (10 iterations, 1s apart)..."
for i in {1..10}; do
    echo "  Iteration $i/10..."
    sshpass -p "$PASSWORD" ssh root@$HOST "dmesg | tail -200" > "$LOG_DIR/dmesg_monitor_$i.log"
    sleep 1
done

# Step 9: Scrape /proc/interrupts
echo "[9/12] Scraping /proc/interrupts..."
sshpass -p "$PASSWORD" ssh root@$HOST "cat /proc/interrupts" > "$LOG_DIR/proc_interrupts.log"

# Step 10: Capture final full dmesg
echo "[10/12] Capturing final full dmesg..."
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg" > "$LOG_DIR/dmesg_full.log"

# Step 11: Analyze results
echo ""
echo "=== ANALYSIS ==="
echo ""

echo "--- Interrupt Counts (IRQ 37=ISP Core, IRQ 38=VIC) ---"
grep -E "^\s*(37|38):" "$LOG_DIR/proc_interrupts.log" || echo "No ISP/VIC interrupts found"
echo ""

echo "--- VIC RE-ARM Messages (buffer count fix) ---"
grep "VIC RE-ARM" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | tail -5 || echo "No VIC RE-ARM messages"
echo ""

echo "--- Buffer Count Readback ---"
grep "readback.*count=" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | tail -5 || echo "No readback messages"
echo ""

echo "--- Gate Status ---"
grep "CORE GATES" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | tail -5 || echo "No gate status"
echo ""

echo "--- Control Errors ---"
grep -i "control.*limit\|dma.*overflow" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | tail -10 || echo "No control errors"
echo ""

echo "--- Frame Done Interrupts ---"
grep -i "frame.*done" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | tail -5 || echo "No frame done interrupts"
echo ""

echo "--- VIC Interrupt Count ---"
VIC_COUNT=$(grep "VIC INTERRUPT HANDLER CALLED" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | wc -l || echo "0")
echo "VIC interrupt handler called: $VIC_COUNT times"
echo ""

echo "--- ISP Core Interrupt Count ---"
ISP_COUNT=$(grep "ISP CORE INTERRUPT HANDLER" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null | wc -l || echo "0")
echo "ISP core interrupt handler called: $ISP_COUNT times"
echo ""

# Save summary
VIC_IRQ=$(grep "^\s*38:" "$LOG_DIR/proc_interrupts.log" | awk '{print $2}' || echo "0")
ISP_IRQ=$(grep "^\s*37:" "$LOG_DIR/proc_interrupts.log" | awk '{print $2}' || echo "0")
CTRL_ERR=$(grep -c "control.*limit" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null || echo "0")
DMA_ERR=$(grep -c "dma.*overflow" "$LOG_DIR/dmesg_monitor_10.log" 2>/dev/null || echo "0")

cat > "$LOG_DIR/SUMMARY.txt" << EOF
ISP Driver Current Fix Test Summary
====================================
Test Date: $(date)
Log Directory: $LOG_DIR

Key Metrics:
- VIC Interrupts (IRQ 38): $VIC_IRQ
- ISP Core Interrupts (IRQ 37): $ISP_IRQ
- VIC Handler Calls: $VIC_COUNT
- ISP Handler Calls: $ISP_COUNT
- Control Limit Errors: $CTRL_ERR
- DMA Overflow Errors: $DMA_ERR

Fixes Applied:
1. Force active_buffer_count = 1 during stream setup
2. VIC RE-ARM detects wrong buffer count and corrects it
3. VIC RE-ARM skips VIC[0x0] write to preserve buffer count
4. Gate enable registers (0x9a34, 0x9a88, 0x9a94) written before gates
5. VIC RE-ARM writes buffer addresses from VBM or rmem fallback

Expected Results:
- VIC RE-ARM should trigger on first interrupt (wrong_buffer_count)
- Readback should show buffer_count=1 after RE-ARM
- Control limit errors should stop after RE-ARM
- Continuous VIC and ISP interrupts
- Gates should open (0x9ac0=0x1, 0x9ac8=0x0)

Check dmesg_monitor_*.log for detailed interrupt flow.
EOF

cat "$LOG_DIR/SUMMARY.txt"

echo ""
echo "=== TEST COMPLETE ==="
echo "Logs saved to: $LOG_DIR"
echo ""
echo "Key files:"
echo "  - $LOG_DIR/SUMMARY.txt (test summary)"
echo "  - $LOG_DIR/proc_interrupts.log (interrupt counts)"
echo "  - $LOG_DIR/dmesg_monitor_*.log (kernel messages over time)"
echo "  - $LOG_DIR/dmesg_full.log (complete kernel log)"
echo ""

# Step 12: Kill prudynt
echo "[12/12] Stopping prudynt..."
sshpass -p "$PASSWORD" ssh root@$HOST "killall prudynt" 2>&1 || echo "prudynt stopped"

echo ""
echo "Camera is still running. Reboot manually if needed."
echo ""

