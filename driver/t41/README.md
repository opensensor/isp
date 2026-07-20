# Ingenic T41 ISP Recovery

This directory contains the recovered T41 TX-ISP driver baseline for the Wyze
Cam v4 T41NQ/OS04D10 Linux 4.4.94 target. It was generated from the OEM
`tx-isp-t41.ko` whose SHA-256 is
`2426eea9c3c8268373eccc3c3753424f0429ff02044e4ddd4e86c6caa40a6338`.

The source was salvaged from the June 2026 `tx-isp-t41-v1` reconstruction
workspace. All 1,510 function candidates were emitted and 1,508 compiled in
isolation, but only 157 were normalized exact matches. A later manual cleanup
made the whole driver build; it was never promoted into the open driver tree or
validated on hardware.

## Build

Build against the matching Thingino Wyze Cam v4 output:

```sh
ROOT=/home/matteius/thingino-firmware-opensensor/output/master/wyze_cam4_t41nq_os04d10_atbm6062s-4.4.94-uclibc \
KDIR="$ROOT/build/linux-2aca1252ac4a304172b870777365f42bfb100674" \
SOC=t41 ./build_local.sh
```

Expected artifact:

- `driver/t41/tx_isp_t41_recovered.ko`

## Baseline risk

The recovered module is a bring-up artifact, not a production-ready driver.
The initial linked-binary audit found 31 stub functions, 91 collapsed
functions, 400 shorter functions, and 41 OEM-only symbols. Critical deficits
include subdevice initialization, core control/ioctl dispatch, interrupt
helpers, and tuning paths.

Hardware smoke tests must stage the module under `/tmp`, unload conflicting
stock ISP modules first, capture kernel and userspace logs, and reboot after
each experiment. Do not install this baseline into the persistent module tree.
