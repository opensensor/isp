# ISP Performance Baseline

`tools/isp_benchmark_device.sh` records a repeatable, read-only baseline of an
active Ingenic camera pipeline. It is intended to establish facts before
performance optimization or V4L2 work begins.

The benchmark deliberately does **not** claim to isolate kernel ISP execution
time. The vendor kernels do not expose per-module CPU accounting, and the
current pipeline crosses the sensor driver, TX-ISP, OpenIMP or OEM `libimp`,
the encoder, and Raptor. Results therefore keep these measurements separate:

- whole-system CPU consumption and iowait
- per-daemon CPU capacity, RSS, virtual size, threads, and context switches
- system memory availability and kernel-memory indicators
- loaded module footprint and exact on-disk module size/hash
- delivered encoded frames from the Raptor ring producer sequence
- raw ISP and codec interrupt rates, by interrupt and in aggregate
- Raptor stream configuration, ring allocation, and pipeline file identities
- ISP overflow, kernel-fatal, and userspace-fault deltas

Raw interrupt rate is never reported as FPS. A 25 fps T41 stream can generate
roughly 50 `isp-w02` and 160 `isp-m0` interrupts per second. Delivered FPS is
derived from the ring's `write_seq`, which RVD increments once per encoded
frame, without registering another ring reader or copying frame data.

## Run on a device

Stage the script and pass the exact module file that was loaded:

```sh
scp tools/isp_benchmark_device.sh root@CAMERA:/tmp/
ssh root@CAMERA \
  'chmod +x /tmp/isp_benchmark_device.sh && \
   /tmp/isp_benchmark_device.sh \
     -l open-t41-os04d10-2560x1440-config25-no-clients \
     -m /opt/open-tx-isp-smoke/module.ko \
     -w 30 -d 120 -i 10 \
     -o /tmp/open-t41-baseline'
scp -r root@CAMERA:/tmp/open-t41-baseline ./artifacts/
```

Defaults are a 15-second warm-up, 60-second measurement, and 10-second sample
window. A serious comparison should use at least a 30-second warm-up and a
120-second measurement. The output directory must not already exist.

The script needs only POSIX `sh`, `/proc`, `awk`, `stat`, and the BusyBox tools
normally present in Thingino. `ringdump`, `raptorctl`, `sha256sum`, `logread`,
and `logcat` add evidence when available; missing optional commands do not
abort sampling.

## Comparison discipline

An open-versus-OEM or before-versus-after comparison is valid only when these
conditions are held constant and verified in `metadata.tsv` and the captured
Raptor status:

1. same camera, sensor, lens, scene, illumination, and thermal settling time
2. same resolution, configured FPS, codec, rate-control mode, bitrate, GOP,
   substream/JPEG state, and active client count
3. same CPU governor/frequency policy and background services
4. same day/night, WDR, exposure, gain, and image-tuning mode
5. exact ISP module and `libimp` identities, with `-m` supplied explicitly
6. multiple runs in alternating order when the expected difference is small

The bundle records RWD/RSD daemon status and client lists both before and
after sampling. Use both: an empty detailed client list is not sufficient to
label a run idle if the daemon-level status still reports an active client,
and a client that connects mid-run invalidates a nominal no-client label.

Do not compare a live-preview run with a no-client run. Client demand can
change encoder, WebRTC/RTSP, copy, and network costs. Name that workload in the
run label and preserve the status captures.

The benchmark itself wakes once per sample and reads procfs. This observer cost
is small but nonzero and is included in whole-system CPU. The summary reports
wall time beyond scheduled sleeps as `sampling_collection_and_scheduler_delay`;
that is an upper bound on observer time, not a CPU-time correction. Use the
same version, interval, and process list for both sides. A longer interval
reduces observer cost but also reduces visibility into short stalls.

## Outputs and interpretation

The primary files are:

| File | Meaning |
|---|---|
| `summary.tsv` | compact run-level metrics and units |
| `samples.csv` | CPU, memory, delivered-frame, and aggregate IRQ windows |
| `process_summary.tsv` | per-daemon mean/peak CPU and memory |
| `process_samples.csv` | tidy per-process interval samples |
| `irq_samples.csv` | per-interrupt counts and raw rates |
| `irq_summary.tsv` | per-interrupt run-level count and rate summary |
| `pipeline_files.tsv` | executable/library/module size and SHA-256 identity |
| `loaded_module_footprint.tsv` | `/proc/modules` loaded byte counts |
| `error_deltas.tsv` | new overflow/fatal/fault events during measurement |
| `metadata.tsv` | workload identity and benchmark parameters |
| `sensor_state.tsv` | active sensor identity, geometry, and configured FPS |
| `rwd_status*.txt`, `rwd_clients*.txt` | WebRTC daemon/client workload before and after |
| `rsd_status*.txt`, `rsd_clients*.txt` | RTSP daemon/client workload before and after |

`cpu_capacity_pct` is a share of the whole machine. On a two-core SoC, 50%
means one fully occupied core. `cpu_one_core_pct` uses conventional process
monitor notation, where one full core is 100% and two full cores is 200%.

Process RSS values are not summed into a claim of unique pipeline memory:
shared mappings would be counted more than once. Use per-process RSS, shared
memory allocation sizes, and system `MemAvailable` together. Likewise,
`/proc/modules` reports the loaded module image but not every runtime DMA or
heap allocation performed by the driver. Start/end memory and repeatability
are the honest guardrails until explicit allocation counters exist.

`delivered_rate_window_stddev` measures variation between sample windows. It
is not per-frame latency jitter. Per-frame latency requires a dedicated,
carefully costed consumer and is intentionally outside this low-perturbation
baseline.

## Contract for future V4L2 work

Keep the workload-level columns stable when adding V4L2. Set `-t` to identify
the transport, for example `imp-shm-ring` or `v4l2-mmap`, and preserve the same
resolution/rate-control/client workload. This makes CPU, memory, delivered
throughput, errors, and binary footprint directly comparable.

V4L2-specific extensions should be additive: queue/dequeue latency,
outstanding-buffer depth, sequence gaps, buffer errors, and copy versus mmap or
DMABUF mode. They must not replace the common delivered-throughput and resource
metrics, because those are what let the legacy and V4L2 paths be compared
without changing the definition of success.
