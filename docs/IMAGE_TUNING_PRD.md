# PRD: Finish T31 ISP Image Tuning

## Objective

Finish the image-tuning work for the open-source T31 ISP driver so that live output is acceptably close to the OEM driver in color, demosaic quality, denoise behavior, and mode switching, while preserving compatibility with `libimp.so`.

## Problem Statement

The project is no longer blocked on basic stream bring-up. Video streams, but image quality is still not production-ready. The current failure mode is severe false-color / green-magenta blobbing, which indicates the raw-to-color pipeline is still not behaviorally equivalent to the OEM driver.

This PRD turns the current reverse-engineering findings into a finish plan rather than continuing ad-hoc debugging.

## Goals

- Achieve stable, repeatable, visually correct color output in normal streaming
- Match OEM behavior closely enough that `libimp.so` tuning and runtime controls behave as expected
- Replace major synthetic/default tuning content with OEM-derived or OEM-shaped data
- Establish a repeatable validation workflow for day, night, and WDR modes

## Non-Goals

- Perfect mathematical equivalence for every internal register before visible parity is reached
- New user-facing features unrelated to OEM parity
- Retuning the pipeline from scratch without using OEM evidence

## Current State Summary

### What works

- Driver/module loads
- Core MMIO mapping and large ISP window mapping are in place
- Dual IRQ model (`isp-m0` / `isp-w02`) is understood and implemented
- Subdevice graph and stream control are operational enough for live video
- Tuning node exists and major blocks initialize

### What is still broken

- Current live image shows strong false-color blob artifacts
- Some ISP blocks still depend on synthetic/default parameter banks
- Several modules are structurally present but not fully OEM-calibrated

### Most important current historical finding

- `f37c28868810f760002b00e0a00707569b64bbdb` is the last known “crisp image” checkpoint
- The next relevant regression point was expansion of the enabled processing-block whitelist
- Under the current masked logic, the effective crisp-image block set is `0xDD04`

That means the project now has a concrete historical anchor for “good enough to compare against,” even if full parity was not yet achieved.

## High-Confidence Findings Already Established

### 1. Basic hardware path and ownership issues were real blockers

Resolved or understood areas include:

- ISP core mapping must cover the large OEM register space (`0x90000` mapping)
- IRQ ownership must not be double-registered or double-disabled
- platform/subdevice bring-up order matters

### 2. Demosaic/CFA path remains highly sensitive

The following were identified as real color-path concerns:

- live CFA pattern programming at register `0x8`
- DMSC output option ownership around register `0x4800`
- DMSC gain refresh ordering during total-gain updates

### 3. Gamma parity had real mismatches

The gamma path needed OEM-style register programming:

- `0x40000` (R)
- `0x48000` (G)
- `0x50000` (B)

This is now understood and implemented, but gamma alone did not resolve the blobs.

### 4. Not all remaining problems are “logic bugs”

The OEM tuning manifest shows that several subsystems still rely on synthetic, zeroed, or placeholder data. This means some remaining image-quality failures are likely caused by missing OEM-calibrated tables, not just bad control flow.

## Known Synthetic / Placeholder Areas

Based on `OEM_TUNING_BLOB_MANIFEST.md`, the biggest remaining data-quality gaps are:

1. **AE tables**
2. **ADR/WDR tone-mapping tables**
3. **BCSH / CCM / WB tables**
4. **MDNS / RDNS / SDNS banks**

Important interpretation: even if register sequencing is correct, visibly wrong output can persist if the active tables are still synthetic.

## Highest-Value Remaining Technical Hypotheses

### Hypothesis A: The live false-color issue is still concentrated in the early color path

The most likely live-path suspects remain:

- **GIB** (highest-confidence single block suspect)
- LSC
- YDNS

Reason: after restoring the effective crisp whitelist, the strongest residual delta from the OEM-style broad set is in these blocks, while ADR and MDNS are still guarded or parked in practice.

### Hypothesis B: Table content is now as important as control flow

The project has already fixed several obvious sequencing and register-path mismatches. The next large quality gains are likely to require replacing placeholder content with OEM-backed parameter data, especially for AE, CCM/BCSH, gamma-adjacent tables, and denoise banks.

## Product Requirements

### Functional requirements

- Normal streaming must produce stable color without large green/magenta blobs
- Day/night switching must not corrupt color or wedge the pipeline
- WDR enable/disable must execute without hangs and with sane output
- Core `libimp.so` controls for exposure, white balance, and tuning must remain functional

### Quality requirements

- Output must be visually comparable to OEM in daylight and indoor lighting
- No frame-stable color blobs in neutral scenes
- No persistent CFA-phase corruption after sensor flips or mode changes
- No catastrophic regressions when enabling candidate ISP blocks

### Engineering requirements

- Every tuning fix must be tied to either OEM HLIL evidence or a documented empirical comparison
- New data/table imports must have a provenance note (blob range, OEM array, or inferred mapping)
- Validation must produce reproducible captures/logs for before/after comparison

## Execution Plan

### Phase 0 — Freeze a reproducible baseline

- record the exact “crisp-ish” and “broken” checkpoints
- capture the effective top-bypass set, mode, sensor mbus code, and flip state
- save reference screenshots for daylight, indoor warm light, and low light

### Phase 1 — Build a repeatable validation harness

- standardize capture scenes and camera placement
- standardize logs to collect around `tisp_init`, `tiziano_dmsc_init`, `tiziano_gamma_init`, and top-bypass lines
- record per-run block-enable state and current mode (day/night, WDR, shvflip)

### Phase 2 — Finish live-path block parity

Focus on blocks that can directly create visible early-pipeline corruption:

1. GIB
2. LSC
3. YDNS
4. residual DMSC/CFA edge cases

For each block:

- confirm OEM init sequence
- confirm enable/bypass semantics
- confirm register windows and trigger/commit writes
- test block enabled vs bypassed with fixed scene captures

### Phase 3 — Replace synthetic OEM-data gaps

Recommended reconstruction order:

1. AE tables
2. Gamma / degamma adjacent tuning content
3. BCSH / CCM / WB tables
4. ADR/WDR tone-mapping tables
5. RDNS / MDNS / SDNS banks

This order follows both current manifest confidence and likely image-quality payoff.

### Phase 4 — Mode-complete validation

- day mode
- night mode
- WDR on/off
- sensor flip / mirror combinations
- stream stop/start and second-stream stability

## Deliverables

### D1. Baseline capture pack

- labeled screenshots/video frames
- matching logs
- commit hashes / module parameters used

### D2. Block parity sheets

Per major block, document:

- OEM function(s)
- open-source function(s)
- register ranges
- current status: OEM-parity / partial / synthetic / parked

### D3. Table provenance sheet

For each imported or reconstructed table, document:

- blob source range
- destination arrays/functions
- confidence level
- visual impact observed

### D4. Final acceptance report

- comparison against OEM captures
- unresolved differences
- recommended defaults for production use

## Risks

- False positives from changing multiple blocks at once
- Good control flow still producing bad images because of placeholder tables
- WDR / MDNS / ADR changes causing hangs even when they look architecturally correct
- Overfitting to a single sensor/mode/scene

## Dependencies

- OEM Binary Ninja access for function-level parity
- consistent hardware test setup
- known-good reference captures from OEM driver where possible
- stable way to extract or map OEM tuning blob content into open-source arrays

## Suggested Work Queue From Here

1. Keep the new documentation current
2. Build the baseline capture pack
3. Do a controlled block matrix around GIB / LSC / YDNS from the `0xDD04` baseline
4. Start importing real AE tables from the identified OEM blob ranges
5. Move next to BCSH/CCM/WB and denoise-bank content

## Exit Criteria

This effort is complete when:

- normal streaming output is visually sane across representative scenes
- no persistent false-color blob artifacts remain
- day/night and WDR transitions work without hangs or gross corruption
- the main remaining differences vs OEM are minor, documented, and acceptable