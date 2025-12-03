# CSI clock/reset sequencing – working vs cnt-irq-III

This note distills what the working branch does (from the /opt/csi.txt baselines you shared) versus the current cnt-irq-III behavior, and outlines the minimal, low‑risk changes to port.

## High‑signal observations (working branch)

From BEFORE/AFTER tags on the working branch:

- BASIC space is alive before csi_core_ops_init
  - Examples: 0x04=N_LANES non‑zero, 0x0C/0x10 non‑zero, 0x128 (lane mask) non‑zero
  - LANEC block (0x200–0x2F4) is populated
- CPM clock registers (0x20/0x24/0x28/0x2C) stay constant across tags
  - No explicit driver ungate; clocks already suitable for BASIC access
- "Basic CSI init (post interface_type write)" behavior
  - 0x04 (N_LANES) becomes lanes-1 (e.g., 0x1)
  - 0x0C (DPHY_RSTZ) set to 1 (out of reset)
  - 0x10 (CSI2_RESETN) held low in this phase in some runs (0x0), otherwise not asserted until stream-on
- Stream-on (enable=1)
  - 0x00 timing value becomes non‑zero (e.g., 0x7D)
  - 0x10 (CSI2_RESETN) asserted high (1)
  - 0x128 (lane mask) becomes active (e.g., 0x3F for 4‑lane; 0x33 for 2‑lane)
  - LANEC gains additional non‑zeros (e.g., 0x250/0x254)
- W01 (VIC) transitions around stream‑on (e.g., 0x14: 0x630 → 0x230 → 0x200/0x300/0x330)

## High‑signal observations (cnt‑irq‑III)

- BASIC window reads all zeros at all phases (0x00.., 0x128, LANEC)
- ISP CORE window is mapped and alive
- Driver attempts explicit CPM ungates and reset deasserts prior to BASIC access
  - CPM 0x34 stays 0x00000101 and BASIC remains all zeros
  - Modifying CPM did not make BASIC writeable

## Likely root cause

Sequence, not magic values. The working branch does not actively ungate/reset via CPM at this point; instead, it sequences the CSI state machine in BASIC space:

- Program N_LANES at 0x04
- Bring DPHY out of reset at 0x0C
- Keep CSI2_RESETN low during basic init
- On stream‑on: assert CSI2_RESETN and enable lane mask at 0x128
- W01/VIC transitions occur around stream‑on

cnt‑irq‑III diverges mainly by trying to force CPM ungates/resets before BASIC is writable, which correlates with the all‑zeros syndrome.

## Minimal, low‑risk port back to cnt‑irq‑III

1) Stop CPM manipulation by default
   - Do not call the JIT clock ungate helper during CSI basic init
   - Keep the CPM reset helper compiled but disabled and not invoked (param default=0)

2) Preserve the ISP CORE mapping fix
   - Keep `isp_csi_regs` (+0x13c) mapped to ISP CORE base, with VIC‑derived fallback only if needed

3) Align CSI BASIC sequencing with working branch
   - In basic init (pre stream‑on):
     - N_LANES (0x04) = lanes ‑ 1
     - DPHY_RSTZ (0x0C) |= 1 (out of reset)
     - CSI2_RESETN (0x10) &= ~1 (keep low here if the reference run shows that)
   - In stream‑on (enable=1):
     - CSI2_RESETN (0x10) |= 1
     - Lane mask (0x128) per sensor lanes (e.g., 0x33 for 2‑lane, 0x3F for 4‑lane)
   - Do not write the LANEC block directly; it’s populated as a consequence of the above

4) Maintain stream ordering
   - ON: VIC → CSI → Sensor
   - OFF: Sensor → CSI → VIC

5) Diagnostics
   - Keep /opt/csi.txt snapshots at the same tags
   - CPM lines read‑only for confirmation; no CPM writes in this path

## Verification checklist (cnt‑irq‑III after changes)

- BEFORE csi_core_ops_init: BASIC no longer all zeros (at minimum 0x00/0x04/0x0C/0x10 read non‑zero)
- AFTER basic CSI init: 0x04 reflects lanes‑1; 0x0C=1; 0x10 remains low if matching working sequence
- AFTER s_stream enable=1: 0x10=1; 0x128 lane mask non‑zero; LANEC shows non‑zero entries; W01 transitions
- CPM lines remain stable (no active ungate/reset required here)

## Notes

- All values above are examples/signals from the working logs; we avoid hard‑coding trace values and only enact the sequence the reference path performs.
- If the platform’s central clock management evolves, CPM gating can remain as an opt‑in experimental path (module param) but should not be on the default bring‑up path for CSI.

