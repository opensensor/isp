#!/bin/bash
# Verbose ISP Driver Test - Captures all logs with maximum verbosity

set -e

HOST="192.168.50.211"
PASSWORD="Ami23plop"
LOG_FILE="/tmp/isp_verbose_test_$(date +%Y%m%d_%H%M%S).log"

echo "=== ISP Driver Verbose Test ===" | tee "$LOG_FILE"
echo "Waiting 30 seconds for camera to boot..." | tee -a "$LOG_FILE"
sleep 30

echo "[1/8] Checking camera connectivity..." | tee -a "$LOG_FILE"
for i in {1..10}; do
    if sshpass -p "$PASSWORD" ssh root@$HOST "echo ready" 2>/dev/null; then
        echo "Camera is ready!" | tee -a "$LOG_FILE"
        break
    fi
    echo "  Attempt $i/10..." | tee -a "$LOG_FILE"
    sleep 3
done

echo "[2/8] Setting log level to maximum..." | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "echo '7 4 1 7' > /proc/sys/kernel/printk" 2>&1 | tee -a "$LOG_FILE"

echo "[3/8] Clearing dmesg..." | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg -c > /dev/null" 2>&1 | tee -a "$LOG_FILE"

echo "[4/8] Loading ISP modules..." | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "cd /opt && insmod tx-isp-trace.ko && insmod tx-isp-t31.ko isp_clk=100000000 && insmod sensor_gc2053_t31.ko" 2>&1 | tee -a "$LOG_FILE"

echo "[5/8] Capturing module load dmesg..." | tee -a "$LOG_FILE"
sleep 2
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg" > /tmp/dmesg_module_load.log
echo "Saved to /tmp/dmesg_module_load.log" | tee -a "$LOG_FILE"

echo "[6/8] Starting prudynt..." | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "nohup prudynt > /dev/null 2>&1 &" 2>&1 | tee -a "$LOG_FILE"
sleep 5

echo "[7/8] Capturing full dmesg after prudynt..." | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "dmesg" > /tmp/dmesg_full.log
echo "Saved to /tmp/dmesg_full.log" | tee -a "$LOG_FILE"

echo "[8/8] Analyzing logs..." | tee -a "$LOG_FILE"
echo "" | tee -a "$LOG_FILE"
echo "=== HANDLER REGISTRATION ===" | tee -a "$LOG_FILE"
grep -i "system_irq_func_set\|Registered handler" /tmp/dmesg_module_load.log | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== INTERRUPT COUNTS ===" | tee -a "$LOG_FILE"
sshpass -p "$PASSWORD" ssh root@$HOST "cat /proc/interrupts | grep -E '^\s*(37|38):'" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== VIC INTERRUPT HANDLER CALLS ===" | tee -a "$LOG_FILE"
grep -c "VIC INTERRUPT HANDLER CALLED" /tmp/dmesg_full.log 2>/dev/null | tee -a "$LOG_FILE" || echo "0" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== VIC IRQ MESSAGES (first 20) ===" | tee -a "$LOG_FILE"
grep "VIC.*IRQ" /tmp/dmesg_full.log 2>/dev/null | head -20 | tee -a "$LOG_FILE" || echo "No VIC IRQ messages found" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== VIC RE-ARM MESSAGES ===" | tee -a "$LOG_FILE"
grep "VIC RE-ARM" /tmp/dmesg_full.log 2>/dev/null | head -10 | tee -a "$LOG_FILE" || echo "No VIC RE-ARM messages found" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== CONTROL LIMIT ERRORS ===" | tee -a "$LOG_FILE"
grep -i "control.*limit" /tmp/dmesg_full.log 2>/dev/null | head -10 | tee -a "$LOG_FILE" || echo "No control limit errors" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== GATE STATUS ===" | tee -a "$LOG_FILE"
grep "CORE GATES" /tmp/dmesg_full.log 2>/dev/null | head -10 | tee -a "$LOG_FILE" || echo "No gate status messages" | tee -a "$LOG_FILE"

echo "" | tee -a "$LOG_FILE"
echo "=== TEST COMPLETE ===" | tee -a "$LOG_FILE"
echo "Full logs:" | tee -a "$LOG_FILE"
echo "  - /tmp/dmesg_module_load.log (module loading)" | tee -a "$LOG_FILE"
echo "  - /tmp/dmesg_full.log (complete dmesg)" | tee -a "$LOG_FILE"
echo "  - $LOG_FILE (test output)" | tee -a "$LOG_FILE"

# Kill prudynt
sshpass -p "$PASSWORD" ssh root@$HOST "killall prudynt" 2>&1 || echo "prudynt stopped"

echo "" | tee -a "$LOG_FILE"
cat "$LOG_FILE"

