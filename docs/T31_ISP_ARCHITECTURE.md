# Ingenic T31 ISP Architecture Notes

## Purpose

This document captures the current shared understanding of the Ingenic T31 ISP hardware and the open-source `tx-isp-t31.ko` reimplementation in this repository. It is meant to be a durable architectural reference, not a point-in-time debugging log.

## Ground Truth and Scope

- Open-source implementation: `driver/`
- OEM reference binary: `OEM-tx-isp-t31.ko`
- User-space ABI consumer: Ingenic `libimp.so`
- Primary goal: behavioral equivalence with the OEM driver so `libimp.so` works unmodified

The OEM binary is the authoritative reference for register values, sequencing, ioctl contracts, and error behavior.

## High-Level System Model

At a high level, the T31 camera stack is:

1. Image sensor outputs raw Bayer data
2. CSI receives serialized sensor data
3. VIC/VIN/core path moves frames into the ISP processing pipeline
4. ISP processing blocks transform raw Bayer into tuned output
5. Frame-source devices expose processed frames/channels to user space
6. `libimp.so` configures the pipeline through ioctls and tuning interfaces

The open-source driver models this as a kernel module plus several logical/platform subdevices rather than one monolithic code path.

## Major Hardware Resources

The current driver models the T31 ISP around these key resources:

| Resource | Address / IRQ | Notes |
|---|---:|---|
| ISP core MMIO | `0x13300000` | Main ISP register space; mapped large enough to cover LUT windows used by OEM writes |
| CSI window | `0x10022000` | Camera Serial Interface / DPHY-facing window |
| VIC window | `0x133e0000` | VIC control/config window |
| Primary IRQ | `37` | `isp-m0`, ISP core path |
| Secondary IRQ | `38` | `isp-w02`, VIC / secondary path |

Important implementation detail: `tx_isp_init_memory_mappings()` maps the ISP core at size `0x90000`, specifically to cover large OEM register ranges such as LSC and DEIR/GIB-related windows.

## Driver Decomposition

The source tree is organized by functional block:

| File | Role |
|---|---|
| `driver/tx-isp-module.c` | Module entry/exit, platform devices, shared register helpers, IRQ adoption/dispatch |
| `driver/tx_isp_core.c` | Core probe, MMIO mappings, core init, ISP ISR, first-frame bring-up logic |
| `driver/tx_isp_csi.c` | CSI device / stream control |
| `driver/tx_isp_vic.c` | VIC device / frame movement / buffer handling |
| `driver/tx_isp_vin.c` | VIN-facing subdevice |
| `driver/tx_isp_fs.c` | Frame-source channels presented to user space |
| `driver/tx_isp_tuning.c` | Tuning subsystem, ISP block init, parameter IOCTLs, per-block updates |
| `driver/tx_isp_subdev*.c` | Subdevice registration, graph creation, pad/link management |
| `driver/include/` | Shared headers, structs, register-map helpers |

## Platform and Subdevice Model

`tx-isp-module.c` creates logical platform devices for:

- `tx-isp` (top-level wrapper)
- `isp-m0` (core / primary IRQ domain)
- `isp-w00` (VIN)
- `isp-w01` (CSI)
- `isp-w02` (VIC)
- `isp-fs` (frame source)

The subdevice graph is constructed in `tx_isp_subdev_mgmt.c`.

- `tx_isp_create_subdev_graph()` builds the runtime graph
- `tx_isp_create_basic_pipeline()` creates the basic CSI → VIC pipeline when no registry is present

This graph-based model is the main in-tree abstraction for representing how the ISP hardware blocks are wired together.

## MMIO Access Model

The shared helpers in `tx-isp-module.c` are central:

- `system_reg_write(u32 reg, u32 value)`
- `system_reg_read(u32 reg)`

These helpers resolve the live ISP base from `ourISPdev->core_regs` and are the preferred way to program ISP offsets from tuning code. Many OEM-aligned helpers (`system_reg_write_awb`, `system_reg_write_clm`, `system_reg_write_gib`, etc.) use the same base mechanism and prepend block-enable writes when needed.

## Bring-Up Sequence

The current driver bring-up is split across module init, platform probe, and core init:

1. `tx_isp_init()` in `tx-isp-module.c`
   - allocates `ourISPdev`
   - registers platform devices
   - registers/initializes subdevice platform drivers
   - creates the tuning node
   - builds the subdevice graph
2. `tx_isp_core_probe()` in `tx_isp_core.c`
   - binds the existing `ourISPdev`
   - initializes memory mappings first
   - initializes tuning state after MMIO is available
3. `ispcore_core_ops_init()`
   - runs the core activation sequence
   - calls `tisp_init()` once the VIC is in the ready state

This ordering matters: many failures seen during bring-up were caused by programming registers before mappings, clocks, or IRQ ownership were correct.

## Stream Start and First-Frame Behavior

During stream-on / early ISR handling, two actions are especially important:

- `mbus_to_bayer_write()` writes ISP register `0x8` with the live CFA/Bayer pattern derived from sensor mbus format plus `shvflip`
- `tisp_top_sel()` sets bit 31 of ISP register `0xc` on first interrupt to release top-level processing

If CFA phase is wrong, demosaic output is visibly corrupted.

## Tuning Subsystem Model

`driver/tx_isp_tuning.c` is the bulk of the ISP behavior model. It contains:

- block initialization (`tiziano_*_init`)
- parameter bank storage and IOCTL set/get handlers
- day/night and WDR mode switching
- gain / EV / CT refresh hooks
- DMA-backed statistics setup for AE/AWB/ADR/DPC-style paths

The current `tisp_init()` sequence initializes blocks in an OEM-inspired order, including:

- AE / AWB / Gamma
- GIB / LSC / CCM / DMSC
- Sharpen / SDNS / MDNS / CLM
- DPC / Defog / ADR / AF / BCSH / YDNS / RDNS

## Top Bypass Register and Block Enables

ISP top-level block bypass is computed in `tisp_compute_top_bypass_from_params()` and written to register `0xc`.

The in-tree documented block bits are:

- bit 2: DPC
- bit 4: LSC
- bit 5: GIB
- bit 7: ADR
- bit 8: DMSC
- bit 10: Gamma
- bit 11: Defog
- bit 12: CLM / BCSH path
- bit 14: Sharpen
- bit 15: SDNS
- bit 16: MDNS
- bit 17: YDNS

The first 32 words of the active tuning block contribute to OEM bypass computation, then local safety/debug masks can re-bypass specific blocks.

## Major ISP Processing Blocks

The main image-processing blocks currently modeled in-tree are:

- **AE / AWB / AF**: exposure, white balance, autofocus statistics/control
- **DPC**: defect-pixel correction
- **GIB / GB**: green imbalance / green-balance related stages
- **LSC**: lens shading correction
- **DMSC**: demosaic
- **CCM / BCSH / CLM**: color correction, brightness/contrast/saturation/hue, color luminance mapping
- **Gamma**: LUT-based tone mapping
- **Sharpen**: detail enhancement
- **SDNS / MDNS / RDNS / YDNS**: spatial, motion, raw, and luma denoise families
- **Defog / ADR / WDR**: dynamic range and tone-mapping related paths

`driver/REGMAP_ADR_YDNS.md` documents the ADR/YDNS register windows and some OEM ordering constraints.

## Banked Modes and Runtime Switching

Several subsystems maintain separate linear and WDR banks. The important global switches are:

- `tisp_s_wdr_en()` for WDR transition sequencing
- day/night control paths that swap `tparams_day`, `tparams_night`, and `tparams_active`

Mode changes are not just single-bit toggles; they trigger ordered re-initialization or re-selection of multiple per-block parameter banks.

## Data Sources for Tuning

The driver uses multiple classes of tuning data:

- active day/night tuning blobs (`tparams_day`, `tparams_night`, `tparams_active`)
- hand-modeled or reverse-engineered parameter arrays in `tx_isp_tuning.c`
- DMA-fed runtime statistics for AE/AWB/ADR-style logic
- synthetic placeholder tables where OEM-calibrated content is still missing

`external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md` is the best current map of which subsystem tables are still synthetic or only partially populated.

## Known Reverse-Engineering Rules

These rules have repeatedly proven important:

- OEM register writes and write ordering matter
- clock/reset ordering matters
- IRQ ownership must not be duplicated or double-disabled
- struct layouts and ioctl sizes must remain compatible with `libimp.so`
- image-path fixes must be validated against both live output and OEM HLIL/decompilation

## What We Know Well vs Poorly

### Strongly understood

- dual-IRQ model (`37` + `38`)
- MMIO base ownership and `system_reg_*` access pattern
- overall source-file/module decomposition
- top-bypass architecture
- major block inventory and many register windows

### Still incomplete

- full OEM-calibrated parameter content for AE, ADR/WDR, and denoise banks
- exact image-quality parity for all enabled blocks
- some runtime statistics consumers and per-frame tuning decisions
- full confidence in every block's live enable state under all modes

## Related Documents

- `README.md`
- `CLAUDE.md`
- `driver/REGMAP_ADR_YDNS.md`
- `driver/TX_ISP_VIDEO_S_STREAM_VERIFIED.md`
- `external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md`