# V4L2 Capture Path

The public V4L2 interface will be an additive adapter over the proven TX-ISP
frame pipeline. It must not replace or fork the private frame-channel path
while OEM parity is still being established.

## Landed foundation

The reusable pieces below are independent of Ingenic generation and Linux
media-framework version:

- `tx_isp_frame_abi.h` defines the exact 68-byte MIPS32 buffer wire object,
  state flags, and T31/T41 flag policies.
- `tx_isp_frame_channel.h` defines the private remote events, request-buffer
  object, ioctl families, and common ownership states.
- `tx_isp_frame_format.h` defines the fixed 112/116-byte private formats.
- `tx_isp_frame_layout.c` validates NV12 geometry, allocation length, and the
  complete 32-bit Y/UV DMA address range.
- `tx_isp_video_queue.c` is an allocation-free queue core with caller-owned
  storage. It implements QBUF ownership, hardware take, completion-order
  DQBUF, monotonically increasing sequence numbers, nanosecond-to-timeval
  timestamps, error propagation, counters, and deterministic STREAMOFF
  ownership recovery.

The queue core deliberately contains no `videobuf2`, `file_operations`, or
kernel-version types. A 3.10 private frame-channel adapter, a 4.4 V4L2 node,
and a mainline V4L2 node can therefore exercise exactly the same state
machine. The embedding adapter owns locking, sleeping, DMA allocation, and
wakeups.

## Adapter phases

1. T41 registers `/dev/video0` on frame-source channel 0 by default (selectable
   with the read-only `v4l2_channel` module parameter). It exposes the native
   2560x1440 NV12 format from the shared checked layout helper. The public and
   private paths remain mutually exclusive owners of the selected channel.
2. T41 connects `REQBUFS`/`QUERYBUF`/`QBUF`/`DQBUF`/`STREAMON`/`STREAMOFF` to
   the common queue core through Linux 4.4 vb2 MMAP and contiguous DMA. The
   private queue metadata receives those physical buffers and the existing
   ISP completion path returns them to vb2 in hardware completion order.
3. `poll()`, blocking and nonblocking DQBUF, queue cancellation, process exit,
   repeated open/close, and stream restart are wired through vb2. Channel
   claim/release is strict: a second owner receives `EBUSY`, while STREAMOFF
   wakes the completion thread and deterministically returns all buffers.
4. T41 exports each contiguous MMAP capture buffer with `VIDIOC_EXPBUF`.
   `tests/t41_v4l2_dmabuf_test.c` maps the returned DMA-BUF fds, observes ten
   changing full-resolution frames through those mappings, and leaves buffer
   ownership with the V4L2 capture queue. DMA-BUF import belongs to the
   separate encoder queue; USERPTR is not part of the public contract.
5. When no legacy IMP process has selected a sensor, the first T41
   `REQBUFS` performs the complete sensor registration, graph activation,
   MDNS coherent allocation, and sensor prepare/start/enable sequence. The
   last V4L2 owner performs the inverse sequence and releases the sensor.
   If a private client already owns the sensor lifecycle, the adapter may
   attach only when the selected frame-source channel is unclaimed; otherwise
   the strict ownership gate returns `EBUSY`.
6. Add a mainline media-controller adapter without changing the queue and
   layout cores. Version-specific vb2 glue stays in a small compatibility
   layer.

`tests/t41_v4l2_discovery_test.c` covers QUERYCAP, format, frame-size, and
frame-interval enumeration. `tests/t41_v4l2_mmap_test.c` allocates two full
resolution buffers, captures ten frames, checks sequence/timestamp/payload
metadata, and optionally writes the final NV12 frame for visual validation.
Both tests run with the selected channel exclusively owned by V4L2. The Raptor
V4L2 backend uses the same node rather than opening a parallel IMP channel.

The standalone lifecycle was additionally validated with Raptor fully
stopped: three consecutive open/capture/close cycles each delivered ten
2560x1440 frames at 40 ms intervals, then Raptor restarted and reclaimed the
sensor normally. This removes Raptor as a prerequisite for `/dev/video0`.

The T41 adapter's call into the recovered private ioctl handlers uses the
narrow `KERNEL_DS` bridge available in Linux 4.4. This compatibility glue is
kept out of the common queue/layout cores and must be replaced by native
kernel-operation callbacks in the mainline adapter, where `set_fs()` no
longer exists.

## Correctness contract

- The private Raptor/OpenIMP path and public V4L2 path are mutually exclusive
  owners of a scaler channel until multi-consumer DMA semantics are proven.
- `sizeimage`, Y/UV offsets, and allocation validation come only from
  `tx_isp_nv12_layout_build()` and `tx_isp_nv12_buffer_build()`.
- Sequence values count completed frames from zero for each STREAMON epoch.
- DQBUF order follows hardware completion order, not QBUF order.
- STREAMOFF returns queued, active, and completed slots to the caller without
  retaining stale completion records.
- Buffer errors are visible through the standard error flag and benchmark
  counters; they are never silently converted to successful frames.
- Adding `/dev/video*` must not change ISP tuning, exposure, image quality,
  encoder cadence, or the existing private ABI.

## Tuning lifecycle

V4L2 capture deliberately does not own image policy. The T41 private tuning
node exposes AE expression/statistics reads plus crash-safe sharpness and
saturation controls for a standalone userspace controller. Recovered
brightness, contrast, and hue setters fail with `EOPNOTSUPP` until their BCSH
workspace is complete; they never enter the known invalid pointer path.
`openimp-tuningd` holds that descriptor and tracks gain independently of
Raptor, so capture/encoder restarts do not reset the tuning policy.

## Performance gate

Use `tools/isp_benchmark_device.sh` with `transport=v4l2-mmap` and the same
resolution, frame rate, scene, and client workload as the private path.
V4L2-specific data is additive: QBUF-to-completion latency, outstanding queue
depth, DQBUF wake latency, sequence gaps, buffer errors, and bytes copied.
Delivered FPS, CPU, memory, IRQ rates, and file/module size retain their
existing definitions.
