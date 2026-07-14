# Ingenic T23 ISP Recovery

This directory is the T23 bring-up workspace for matching the OEM
`tx-isp-t23.ko` behavior on Linux 3.10.14 targets.

The initial `tx_isp_t23_recovered.c` file is the kbuild-compatible recovered
whole-driver seed from `tx-isp-t23-v1`, plus the local compatibility shims
needed to load the SC2336 T23 sensor module and start the Raptor pipeline.

Current smoke-test status:

- `sensor_sc2336_t23.ko` loads against the recovered module.
- `/dev/tx-isp`, `/dev/isp-m0`, `/dev/misc-ivdc`, and `/dev/framechan0..3`
  are present.
- `/proc/jz/sensor/sensor0` reports the SC2336 metadata expected by Raptor.
- Raptor starts and publishes stable H.264 streams on both MSCA channels.
- The source-derived SC2336 DMSC, Gamma, static AWB, and BCSH startup images
  produce a clean, artifact-free image with working ISP/VIC interrupts.
- The image still has a broad green cast. The best verified startup keeps the
  top-level CCM bypassed; pairing it with the current daylight CCM image makes
  the cast worse. The next bring-up target is the missing GIB/LSC and dynamic
  3A path, using the T31/T40 source and commit history as the reference.

Build from a compatible Thingino T23 3.10.14 kernel tree with:

```sh
make -C <kernel-src> M=$(pwd)/driver/t23 ARCH=mips CROSS_COMPILE=<mipsel-prefix> modules
```

Expected artifact:

- `driver/t23/tx_isp_t23_recovered.ko`
