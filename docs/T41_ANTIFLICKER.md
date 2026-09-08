# T41 public anti-flicker control and temporal IQ

The 2026-09-08 investigation isolated a rapid brightness oscillation in raw
ISP output, independently of the encoder's slower GOP-linked detail changes.
Fixing the public anti-flicker control does **not** by itself finish temporal
IQ: strict integration-period locking can overexpose bright scenes.

## Defect and repair

OpenIMP sends Gen3 `IMPISPAntiflickerAttr` through ioctl `0xc0105435`, inner
control `0x08000026`. Its payload is an eight-byte enum/frequency structure,
not the older SoCs' exposure-value response sharing that command number.
The T41 startup dispatcher had no route for it and returned success without
applying SET or filling GET. Raptor therefore reported success while native
AE retained its default AUTO policy. The existing `tisp_s_antiflick` wrapper
was unreachable from that public ioctl.

The dispatcher now safely copies the payload, validates channel/direction,
and routes both operations to native AE. An atomic mode/frequency request
owns policy; the frame worker derives its floor from the current sensor's
total line count (including blanking) and rational FPS:

`round(total_height * fps_num / (fps_den * 2 * mains_hz))`

The arithmetic is a shared, tested helper. No live sensor-bin coefficients,
captured integration tables or extra delays are introduced. Reinitialization
retains the selected policy, and timing changes recompute its floor. Public
anti-flicker does not invoke recovered AE workspace code or the experimental
GIB/CCM image profile. Until a public request is made, the old diagnostic
module parameters retain their meaning; afterwards the public request owns
the floor/frequency and bypasses the diagnostic ceiling/target scaling.

Gen3 policies remain distinct:

- DISABLE: no mains snapping and no integration floor.
- NORMAL: at least one mains half-period, with the existing generated nodes
  used for longer integration.
- AUTO: permit short integrations below the first node, then snap at/above it.

NORMAL cannot promise highlight preservation when even unity sensor gain at
one half-period exceeds the needed exposure. AUTO cannot promise flicker-free
short exposure under modulated lighting. Do not hide either limitation with
scene-specific color gains or silently reinterpret the requested mode.

## Physical evidence

On T41NQ/OS04D10, both H.264 outputs stayed active while a third 640x360 NV12
output provided 500 raw frames. Sampling used a 16x9 grid, with every fourth
pixel in each direction. Analysis omitted the first 50 frames and measured
the largest Fourier amplitude within 4.5-5.5 Hz. Values below are eight-bit
luma amplitudes, not perceptual scores or matched stock/open IQ scores.

| Capture | Indoor wall amplitude | Mean whole-frame luma |
| --- | ---: | ---: |
| Original, ineffective public 60 Hz request | 4.791 | 132.97 |
| Fixed driver, NORMAL 60 Hz after cold boot | 0.0125 | 191.59 |
| Fixed driver, explicit AUTO 60 Hz | 8.233 | 132.36 |

The sensor-derived NORMAL floor was 369 lines at this mode, approximately
8.33 ms. The user confirmed that the rapid flicker stopped in NORMAL but the
image became badly overexposed; AUTO brought the rapid flicker back. This is
an explicit failed *overall IQ* gate despite successful control behavior.
The illumination was not externally locked across recordings.

The earlier OEM ISP/encoder recording also contains a roughly 5 Hz indoor
wall oscillation (9.24 luma amplitude after the same encoded-luma analysis).
Its anti-flicker setting was OFF, so it is not evidence that OEM NORMAL or
AUTO behaves identically. That effect is distinct from the previously
documented I/P-frame difference. A similar T31 symptom was reported by the
user, but T31 has not been measured in this test.

A subsequent fresh, cold-boot **OEM ISP + OEM encoder** A/B at 25 fps
confirmed the same tradeoff. Each recording contains 500 frames, with the
first 50 omitted. Both use the same encoded-Y extraction and 640x360 zone
measurement (not the subsampled raw diagnostic above):

| OEM policy | Wall 4.5-5.5 Hz amplitude | Mean Y | Pixels with Y >= 250 |
| --- | ---: | ---: | ---: |
| OFF | 4.759 | 125.06 | 6.09% |
| NORMAL 60 Hz | 0.0318 | 213.95 | 61.71% |

Raptor's settled OEM exposure query reports EV 369 and unity analog gain
under NORMAL; GIB reads `0x04440444` for both gain pairs. Stock also loses
the highlight detail at its 60 Hz minimum exposure. Neither proprietary
firmware nor arbitrary post-gain attenuation is evidence of a solution to
short-exposure mixed-light flicker. The open default dispatcher bug is real,
but the measured rapid symptom itself is not unique to multistream/open.
The two algorithms' exposure/WB were not locked, so these are within-stock
policy measurements, not a claim of overall stock/open IQ parity.

Full host checks and exposure ASan/UBSan pass. On-target GET/SET round trips
cover off, AUTO, NORMAL and 50/60 Hz; invalid mode/frequency/channel/direction
and bad pointers are rejected without changing the selected policy. The
recovered BSS layout is unchanged. Concurrent 60-second main TCP and sub UDP
recordings decode H.264/AAC with empty decoder-warning logs. Each stream-copy
capture still warns once about an unset packet timestamp; that separate
warning is not repaired here. Calibration, saved configuration and Neo audio
are unchanged; deployment uses a reversible one-shot overlay, not a flash.

## Reproduction

Build the target-only tests with the T41 compiler:

```sh
"${CROSS}gcc" -std=c11 -D_GNU_SOURCE -Os -static -Wall -Wextra -Werror \
  -Idriver/include tests/t41_antiflicker_ioctl_test.c -o antiflicker-test
"${CROSS}gcc" -std=c11 -Os -static -Wall -Wextra -Werror \
  tests/t41_v4l2_luma_test.c -o luma-test
```

Stage under `/tmp`. With the normal capture owner active, `antiflicker-test`
alone reads the policy, `--check` exercises and restores it, and `MODE FREQ`
sets it. For example, `1 60` selects NORMAL, while `2 60` selects AUTO. These
are SDK mode numbers, **not** Raptor's off/50/60 CLI values. On Raptor,
`raptorctl rvd set-antiflicker 2` selects NORMAL 60 Hz.

`luma-test 500` captures the unused third output and prints CSV numeric rows
after the lifecycle preamble/header. Check source sequence/timestamps as well
as zone means; FFT frame-to-frame luma after subtracting each zone's mean.
Keep raw/encoded private scene captures outside the repository. Record the
exact policy and exposure alongside every image comparison.
