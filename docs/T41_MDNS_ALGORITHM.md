# T41 calibration-driven temporal denoise

The portable MDNS boundary takes a 0x61a-byte calibration block, log2-Q16
gain and input dimensions. Twenty binding runs interpolate 140 byte-valued
fields. Its register writer computes geometry, scalar fields, packed LUTs,
and two kinds of eight-point ramp. Ramp intermediates are private local
arrays, not shared OEM scratch globals or captured sensor values.

The simple ramp has a flat prefix, truncated slope and explicit final
endpoint. The difference ramp separately truncates Q13 slope and bin spacing;
replacing these two truncations with one division is not equivalent. Function
enables follow calibration and the configured memory optimization, including
the OEM mutation of disabled plane bits. The writer rejects zero divisors
before emitting any transaction, rather than reproducing an OEM fault.

Manual strength uses the original day/night calibration body at +0x17406,
not the runtime parameter offset +0x1417e. WDR selects a further +0x39c in
the source window. Five eleven-knot tables have maxima 128/128/128/180/255.
Unlike the unrestricted Q7 ratio, values outside the open interval
`(0, maximum)` become zero at strength <=128, but remain unchanged above 128.
The source range is checked against the loaded calibration allocation.

The captured MDNS delta is removed. Initialization programs calibrated
registers while keeping TOP bit 13 bypassed and does not start DMA. Only
after the existing reserved-memory setter has validated and programmed the
buffer addresses does the driver start/trigger MDNS and restore calibrated
TOP routing. Gain changes are frame-driven with 0x100 hysteresis. Calibration
replacement and day/night/strength paths use checked state and record the
gain actually represented by the register bank. Buffer-activation errors
are returned to userspace instead of silently reporting success.

Reproduction:

```sh
make -C tests check
sh tools/build_t41_mdns_oracle.sh CROSS_PREFIX STOCK_OBJECT OUTPUT_DIR
qemu-mipsel OUTPUT_DIR/mdns-oracle-check
```

QEMU and physical T41 match 100,000 randomized pairs of both ramp generators
and 10,000 synthetic strength/interpolation/enable/full-register cases with
zero mismatches. Tests cover both channel apertures, all strength values,
day/night and WDR source selection, independent writer inputs, negative
intermediate geometry and nonzero signed reciprocal denominators. Host
ASan/UBSan tests include malformed dimensions, zero divisors, unaligned
buffers and output canaries. OEM code exists only in the userspace oracle.

The existing kernel allocation remains channel-0 only. Buffer-size formulas
and optional AI buffer leasing are separate from this scalar oracle. Full
motion quality, WDR transitions, AE/AWB convergence and whole-ISP parity
are not established merely by passing these arithmetic tests.

The one-shot OS04D10 camera also passes three TCP and three UDP reconnects,
a unity-to-maximum analog-gain sweep with all block errors zero, and a
120-second H.264/AAC decode without warnings. Automatic exposure is restored.
The outdoor snapshot is not a controlled noise/motion reference.
