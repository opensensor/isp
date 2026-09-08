# T41 multi-output scaler

The portable polyphase generator is shared with T23. Its T41 adapter selects
32 stored intervals/eight fractional phases for channels 0 and 2, and 256
stored intervals/64 fractional phases for channel 1. Both use eight signed
Q11 taps and the SoC's sinc kernel and rounding order, independently of the
sensor and scene. Output/source geometry determines each axis's Q14 ratio.
The existing calibration may select a fixed first phase; the writer honors
that policy without substituting captured live coefficient tables.

The live adapter previously loaded only channel 0. The shared IMP trial
produced main and 640x360 substream H.264, but the decoded substream was
black/green and corrupted. Channel 1 requires coefficient FIFO RAM writes,
not the linear registers used by channels 0/2:

| Channel | Workspace V/H | Transport | Shadow pairs, both axes |
| --- | --- | --- | --- |
| 0 | `0x008` / `0x04a` | `0xf0740` / `0xf0790`, increment 4 | 34 |
| 1 | `0x08c` / `0x28e` | reset `0xf1100` / `0xf1108`, data `+4` | 266 |
| 2 | `0x490` / `0x4d2` | `0xf0840` / `0xf0890`, increment 4 | 34 |

For each of channel 1's 33 hardware phases `p=0..32`, the coefficient
indices are `[256-p, 192-p, 128-p, 64-p, p, 64+p, 128+p, 192+p]`.
The shadow port accepts a value followed by its target address, with two
signed 13-bit coefficients in each value. DMA length counts those pairs,
not port writes. The checked writer is shared by the live adapter and tests.
The live adapter generates before taking the shadow lock, then checks busy,
writes and kicks under the same lock as the per-frame shadow worker. Its
temporary curve storage is bounded at 1028 bytes, allocated with GFP_ATOMIC
because stream setup can already hold a channel spinlock. No new recovered
BSS aliases or sensor-specific tables are introduced.

## Independent reference and tests

The test-only oracle executes the actual H20250310a instructions in userspace
against private arrays and a register-write recorder. It never accesses ISP
hardware or the installed sensor binary. Reference object SHA256:
`572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee`.
ELF functions: `tisp_sin` at `0x603b0`, `tisp_msca_normalized` at `0x60068`,
and `tisp_msca_ch_curve_write` at `0x60b40`.

```
sh tools/build_t41_scaler_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/scaler-oracle-check
```

The oracle checks 6000 synthetic geometry curves across all three channels
and 512 combinations of active-channel masks and fixed-phase modes. Every
coefficient, shadow-port value/address write and accumulated DMA pair count
must match. Synthetic writer inputs also exercise negative/oversized packed
coefficients and masking. Host tests cover invalid channel/geometry, short
calibration/curve buffers, null arguments, unchanged output on rejection,
and callback-error propagation. Host ASan/UBSan checks pass.

The disassembly matters: `tisp_sin` divides the ratio to form the step before
multiplying by phase. The decompiler renders a misleading multiply/divide
expression. The independent instruction oracle rejected that interpretation;
the existing shared arithmetic is retained unchanged.

These are algorithm/transport tests, not proof of live multi-output parity.
The physical test must still check decoded pixels, source and RTSP cadence,
independent output lifetimes and JPEG coexistence. An earlier dual-output
trial had clean decoder logs despite visibly corrupted pixels, and local
25 fps rings despite RTSP send-queue drops. Neither log alone is an exit gate.

## Physical checkpoint and input/output lifetime separation

The generated all-channel writer fixed the black/green 640x360 output on
T41NQ/OS04D10 while preserving the native main image. Main and substream each
decoded 1499 frames in a concurrent 60-second H.264/AAC check with empty
warning logs. A second concurrent RTP check saw no sequence gaps or long
media intervals. The old OpenIMP path still manufactured exactly 25 fps
timestamps, however, so those media intervals did not prove capture timing.

Restarting only the substream then stalled both outputs. The driver log
showed `ispcore_frame_channel_streamon` resetting CSI through the full late
input-start fanout even though the main output was already running. This
also reseeded input-wide tuning history. Output enable state is not input
ownership. The new capture gate serializes the first deferred start for each
input and retains that ownership across individual output stops. Only the
input-level stop releases it. State lives in a separate object, leaving the
recovered object's BSS aliases untouched. Host tests exercise failed-start
retry, independent inputs, full-stop restart and simultaneous output starts.
With OpenIMP's matching shared capture/encoder allocation owner, ten
substream stop/start cycles preserved the main stream at 24.9865 capture fps
without source-timestamp or RTP sequence gaps. RVD RSS/data stayed at
4620/2932 KiB. Different output rates (25/15 fps) and resizing the substream
to 960x544 also preserved the main stream, and the resized pixels decoded
correctly. Main stop/start no longer stalls the graph, but a reverse-direction
probe caught one missing substream frame; it is not a zero-gap pass.

Real QHD JPEG fanout now coexists with both video outputs in the shared IMP
graph, but the first software-JPEG load test caught occasional missing source
frames. These are visible because OpenIMP now preserves the actual capture
timestamp rather than manufacturing a nominal timeline. Standalone V4L2
still exports one selected scaler channel; the shared-graph repairs do not
automatically provide multiple V4L2 queues or encoder arbitration. Neither
JPEG stress nor standalone V4L2 multi-output is declared complete here.
