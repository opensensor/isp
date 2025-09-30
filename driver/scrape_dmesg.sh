#!/bin/bash

# Reusable dmesg scraping function
# Usage: ./scrape_dmesg.sh [output_file] [duration_seconds] [interval_seconds]
#
# This script repeatedly scrapes dmesg to capture all messages before they're lost
# from the ring buffer. It's especially useful for embedded systems with small dmesg buffers.

OUTPUT_FILE="${1:-/home/matteius/isp-latest/driver/logs.txt}"
DURATION="${2:-10}"
INTERVAL="${3:-1}"
REMOTE_HOST="${REMOTE_HOST:-192.168.50.211}"
REMOTE_PASS="${REMOTE_PASS:-Ami23plop}"

echo "=== DMESG SCRAPER ==="
echo "Output file: $OUTPUT_FILE"
echo "Duration: ${DURATION}s"
echo "Interval: ${INTERVAL}s"
echo "Remote host: $REMOTE_HOST"
echo ""

# Clear the output file
> "$OUTPUT_FILE"

# Scrape dmesg repeatedly
for i in $(seq 1 $DURATION); do
    echo "Scraping dmesg... ($i/$DURATION)"
    sshpass -p "$REMOTE_PASS" ssh root@$REMOTE_HOST 'dmesg' >> "$OUTPUT_FILE"
    
    if [ $i -lt $DURATION ]; then
        sleep $INTERVAL
    fi
done

echo ""
echo "=== SCRAPING COMPLETE ==="
echo "Total lines captured: $(wc -l < "$OUTPUT_FILE")"
echo "Logs saved to: $OUTPUT_FILE"
echo ""
echo "Quick stats:"
echo "  VIC START messages: $(grep -c "VIC START" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  VIC input config: $(grep -c "VIC input" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  vic_start_ok=1: $(grep -c "vic_start_ok=1" "$OUTPUT_FILE" 2>/dev/null || echo 0)"
echo "  Errors: $(grep -c -i "error\|fail\|cannot" "$OUTPUT_FILE" 2>/dev/null || echo 0)"

