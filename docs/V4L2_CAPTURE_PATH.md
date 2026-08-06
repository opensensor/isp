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

1. Add a read-only capability/format adapter and register one capture node per
   enabled scaler channel. Expose NV12 only, using the checked active format
   and layout rather than independently calculating strides.
2. Connect `REQBUFS`/`QUERYBUF`/`QBUF`/`DQBUF`/`STREAMON`/`STREAMOFF` to the
   common queue core. The first implementation is MMAP with contiguous DMA;
   it reuses the existing frame-source queue event and completion callback.
3. Add `poll()` and blocking/nonblocking DQBUF wakeups, then validate queue
   cancellation, process exit, repeated open/close, and stream restart.
4. Add DMABUF import only after MMAP correctness and cache ownership are
   proven on T31 and T41. USERPTR is not part of the first public contract.
5. Add a mainline media-controller adapter without changing the queue and
   layout cores. Version-specific vb2 glue stays in a small compatibility
   layer.

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

## Performance gate

Use `tools/isp_benchmark_device.sh` with `transport=v4l2-mmap` and the same
resolution, frame rate, scene, and client workload as the private path.
V4L2-specific data is additive: QBUF-to-completion latency, outstanding queue
depth, DQBUF wake latency, sequence gaps, buffer errors, and bytes copied.
Delivered FPS, CPU, memory, IRQ rates, and file/module size retain their
existing definitions.
