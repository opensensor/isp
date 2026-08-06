# T41 Open-Stack Performance Baseline — 2026-08-06

This is the first versioned baseline produced by
`tools/isp_benchmark_device.sh`. It describes correctness and resource use
before performance optimization or V4L2 implementation work.

## Workload identity

- device: Wyze Cam v4, Ingenic T41, two logical CPUs, 84,940 KiB RAM
- kernel: Linux 4.4.94 SMP PREEMPT
- sensor: OS04D10 (`chip_id=0x530444`), active at 2560x1440 and configured 25 fps
- stream: main H.264, 8,000,000 bit/s CBR, GOP 25, configured 25 fps
- clients: no RTSP or WebRTC clients
- transport: OpenIMP to Raptor shared-memory ring
- warm-up: 30 seconds
- measurement: 120 seconds scheduled, 122.280 seconds actual, 12 windows
- collection interval: 10 seconds

Exact file identities:

| Component | File bytes | SHA-256 |
|---|---:|---|
| open `tx_isp_t41` module | 756,320 | `cf838ed8f4dfc612389fdee5aeb2377a16636818298b6087da278025a55e8814` |
| mapped OpenIMP `libimp.so` | 315,676 | `768898c86f36cecaa3c7fb065a545b41f57be7d54ebd31f298bc313b4fd91804` |
| RVD executable | 150,192 | `0ff407b97e890f88359889ce383483d39ecd073d8c9cc7d51b5a336431046acc` |

The module occupied 829,941 bytes according to `/proc/modules`. This loaded
image count is distinct from the ELF file size and does not include every
runtime allocation made by the driver.

## Baseline results

| Metric | Result |
|---|---:|
| whole-system CPU, mean | 22.441% of two-core capacity |
| whole-system CPU, min / max | 20.768% / 24.320% |
| named pipeline-process CPU, mean | 21.554% of two-core capacity |
| RVD CPU, mean / max | 19.324% / 20.245% |
| RVD RSS, mean / max | 3,944 / 3,944 KiB |
| all named daemons, individual RSS sum | 13,340 KiB |
| MemAvailable, start / end / minimum | 58,224 / 58,120 / 58,088 KiB |
| MemAvailable drift | -104 KiB |
| allocated main / audio SHM files | 2,564,352 / 136,192 bytes |
| delivered frames | 1,526 |
| delivered rate, overall | 12.496 fps |
| delivered window rate, mean / min / max | 12.500 / 12.365 / 12.575 fps |
| delivered-rate window standard deviation | 0.074 fps |
| new ISP overflows / kernel fatals / userspace faults | 0 / 0 / 0 |

Individual daemon CPU means were RVD 19.324%, RAD 2.093%, RSD 0.120%, RIC
0.013%, and RWD 0.004% of total machine capacity. The RSS sum is shown only as
an inventory; it is not unique memory because shared mappings may be counted
in more than one process.

Raw interrupt rates were:

| IRQ | Mean rate |
|---|---:|
| `isp-w02` | 50.009/s |
| `isp-m0` | 160.871/s |
| `isp-ivdc` | 0/s |
| `avpu.0` | 12.500/s |

These are activity counters, not frame rates. Their aggregate ISP rate was
210.533 interrupts/s.

## Correctness finding

The sensor procfs state, Raptor stream status, and ring header all advertised
25 fps, but the producer sequence delivered 12.496 fps. A direct `ringdump`
check independently showed alternating frame timestamp gaps of approximately
52 ms and 108 ms and an average near 12.9 fps over a short sample. The 2.5K
pipeline is therefore not currently delivering its configured 25 fps.

This mismatch is part of the baseline, not a benchmark substitution: future
changes must report configured and delivered rate separately. A performance
improvement that preserves 12.5 fps is not equivalent to one that delivers the
full configured workload.

## Observer bound and replicate

Sampling plus scheduler delay added 2.280 seconds beyond 120 seconds of
scheduled sleeps, or 1.865% of actual wall time. That is reported as an upper
bound on observer wall time, not subtracted from CPU.

An earlier version-1 replicate used 24 five-second windows. It produced
12.496 delivered fps, approximately 21.50% named-process CPU, -64 KiB
MemAvailable drift, and zero new faults. Whole-system CPU was 23.553%; the
larger collector duty cycle is why version 2 defaults to ten-second windows.

The complete version-2 evidence bundle is kept in the local artifact archive
at `artifacts/device-baselines/isp-benchmark-v2-open-t41-os04d10-20260806/`.
It includes raw interval samples, process/IRQ summaries, module parameters,
file hashes, stream/client status, and before/after kernel and service logs.

## Post-baseline optimized checkpoint

Later correctness work restored the configured QHD cadence before any
optimization was accepted. After removing OpenIMP's discarded source sampler
and redundant T41 access-unit rescan, the August 6 compact-module checkpoint
measured 24.992 delivered fps and 3.176% named pipeline-process CPU. The
30-second run changed MemAvailable by -44 KiB and recorded zero ISP overflow,
kernel-fatal, or userspace-fault events. A subsequent ten-second RTSP probe
decoded High-profile 2560x1440 at 25/1 without warnings.

The recovered module also contained ten zero-filled absolute-address anchors
which had each been emitted as generic 16 KiB placeholders. Exhaustive source
references and the retained OEM address spacing bound their reachable sizes
to 18 through 514 bytes. Correcting those declarations reduced `.bss` from
270,288 to 107,808 bytes and the live `/proc/modules` footprint from 829,941
to 667,525 bytes. The OEM module occupies 666,727 bytes, leaving the open
module only 798 bytes larger when loaded. Its 756,384-byte ELF file remains
smaller than OEM's 835,476-byte file because NOBITS BSS does not contribute
payload bytes.

This checkpoint is not compared directly with the original 12.5 fps CPU
figures above: it performs twice the delivered work and includes substantial
correctness changes. The original bundle remains the honest pre-optimization
baseline; the newer run is the full-rate production checkpoint.

## Allocation-free IRQ wait and production trace gate

The next measured build combines two lifecycle/performance changes without
altering image tuning or the H.264 command path:

- OpenIMP reuses one aligned thread-local result buffer for synchronous AVPU
  `WAIT_IRQ`, eliminating one heap allocation/free pair per encoded frame.
- T41's recovered QBUF, remote-event, pad-event, ISP-IRQ, and tuning-poll
  diagnostics are behind the writable `t41_runtime_trace` parameter, which is
  off by default. Before the gate, the module emitted multiple warning-level
  records per frame; the final boot emitted zero records from these hot paths.

The committed artifacts are open-tx-isp `bcf6d1df` and OpenIMP `b313479`:

| Component | File bytes | Loaded bytes | SHA-256 |
|---|---:|---:|---|
| open `tx_isp_t41` module | 757,580 | 668,136 | `998427b79384826e50435a2fdd3d8f479d4431b5a4748206aeb9df6c8ecc53c3` |
| mapped OpenIMP `libimp.so` | 317,004 | n/a | `b5eb3526d555051b6bb40b08913a2b74002c337369437e0d4fbe39374f7fb91f` |

The 60-second QHD run delivered 24.996 fps with 0.041 fps window standard
deviation, +112 KiB `MemAvailable` drift, and zero ISP-overflow, kernel-fatal,
or userspace-fault deltas. Named pipeline CPU was 7.474% of total two-core
capacity; RVD was 0.809%.

The immediately preceding full-rate run measured 7.866% named pipeline CPU
and 1.047% RVD CPU, so the directional changes are -5.0% and -22.7%,
respectively. The whole-system figures are not compared: the earlier client
snapshot showed one established WebRTC peer, while the final bundle's detailed
client list was empty even though RWD retained the same live-worker CPU/RSS
signature and reported a client immediately after the run. Benchmark version
2 now records daemon-level RWD/RSD status and before/after client lists so
future comparisons cannot hide that ambiguity behind a single snapshot.

The loaded open module is 1,409 bytes (0.21%) larger than OEM and its ELF file
is 77,896 bytes (9.3%) smaller. The 426-byte loaded increase from the preceding
open checkpoint is the explicit runtime trace gate and parameter metadata; it
keeps recovery diagnostics available without paying per-frame logging cost.
