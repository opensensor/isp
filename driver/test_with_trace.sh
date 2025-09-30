#!/bin/bash
# Test script that loads trace driver first, then our driver

REMOTE_HOST="192.168.50.211"
REMOTE_PASS="Ami23plop"

echo "=== Loading trace driver first, then ISP driver ==="

# Build both drivers
cd /home/matteius/isp-latest/external/ingenic-sdk
export CROSS_COMPILE=mipsel-linux-
export KDIR=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/build/linux-4fb8bc9f91c2951629f818014b7d3b5cc2a1ec81/
export PATH=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/per-package/toolchain-external-custom/host/bin/:$PATH

echo "Building ISP driver..."
SENSOR_MODEL=gc2053 ./build.sh t31

echo "Trace driver already built by build.sh"

echo "Rebooting device..."
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'reboot' || true
sleep 35

echo "Loading trace driver..."
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'cat > /tmp/tx-isp-trace.ko' < /home/matteius/isp-latest/external/ingenic-sdk/tx-isp-trace.ko
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'insmod /tmp/tx-isp-trace.ko'
sleep 2

echo "Loading ISP driver..."
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'cat > /tmp/tx-isp-t31.ko' < /home/matteius/isp-latest/external/ingenic-sdk/tx-isp-t31.ko
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'cat > /tmp/sensor_gc2053_t31.ko' < /home/matteius/isp-latest/external/ingenic-sdk/sensor_gc2053_t31.ko
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'insmod /tmp/tx-isp-t31.ko isp_clk=200000000 print_level=1 && insmod /tmp/sensor_gc2053_t31.ko sensor_max_fps=30 data_interface=1'

echo "Waiting for initialization..."
sleep 5

echo "Capturing trace..."
sshpass -p "${REMOTE_PASS}" ssh root@${REMOTE_HOST} 'cat /opt/trace.txt' > /home/matteius/isp-latest/driver/our-trace.txt

echo "Done! Trace saved to driver/our-trace.txt"
echo "Compare with reference-trace.txt to see differences"

