# Ingenic T31 ISP Pipeline Architecture

## Comprehensive Driver Documentation

This document provides a detailed technical reference for the open-source Ingenic T31 ISP
kernel driver (`tx-isp-t31.ko`). It covers the full pipeline from sensor input through
ISP processing to user-space frame delivery, including register maps, data structures,
interrupt handling, and state machines.

---

## Table of Contents

1. [System Overview](#1-system-overview)
2. [Hardware Resources and Memory Map](#2-hardware-resources-and-memory-map)
3. [Module Initialization and Bring-Up](#3-module-initialization-and-bring-up)
4. [Subdevice Graph Infrastructure](#4-subdevice-graph-infrastructure)
5. [CSI Subsystem (Camera Serial Interface)](#5-csi-subsystem)
6. [VIC Subsystem (Video Input Controller)](#6-vic-subsystem)
7. [ISP Core Processing Pipeline](#7-isp-core-processing-pipeline)
8. [Frame Source and Delivery](#8-frame-source-and-delivery)
9. [Tuning Subsystem](#9-tuning-subsystem)
10. [Interrupt Architecture](#10-interrupt-architecture)
11. [Buffer Management](#11-buffer-management)
12. [State Machines](#12-state-machines)
13. [Register Reference](#13-register-reference)
14. [Debug and Diagnostics](#14-debug-and-diagnostics)
15. [End-to-End Data Flow](#15-end-to-end-data-flow)

---

## 1. System Overview

### 1.1 System Model

The T31 camera stack processes image data through a multi-stage hardware pipeline:

```
                                    +------------------+
                                    |   libimp.so      |
                                    |  (user-space)    |
                                    +--------+---------+
                                             |
                                    ioctls / mmap / QBUF / DQBUF
                                             |
  +----------+     +----------+     +--------+---------+     +----------------+
  |  Image   |---->|   CSI    |---->|      VIC         |---->| Frame Channels |
  |  Sensor  |MIPI |Receiver  |     | (Video Input     |     | /dev/framechanX|
  |  (GC2053)|     |0x10022000|     |  Controller)     |     +----------------+
  +----------+     +----+-----+     | 0x133e0000       |
                        |           +--------+---------+
                   W01 Wrapper               |
                   0x10023000          ISP Core Regs
                   (PHY timing)        0x13300000
                                             |
                                    +--------+---------+
                                    |  ISP Processing  |
                                    |  Pipeline        |
                                    |  (AE/AWB/Gamma/  |
                                    |   DMSC/Denoise/  |
                                    |   Sharpen/etc.)  |
                                    +------------------+
```

### 1.2 Design Principles

- **Behavioral equivalence** with the OEM `tx-isp-t31.ko` driver
- **ABI compatibility** with Ingenic's closed-source `libimp.so`
- **Global device singleton** (`ourISPdev`) for simplified cross-module access
- **Event-driven subdevice communication** via pad/link graph
- **Dual-IRQ model** separating ISP core (IRQ 37) and VIC (IRQ 38) paths

### 1.3 Source Layout

```
driver/
  tx-isp-module.c          Module init/exit, platform driver, shared register helpers
  tx_isp_core.c            ISP core subdev, MMIO mappings, core ISR, bring-up logic
  tx_isp_vic.c             VIC subdev, MDMA, buffer management, frame capture
  tx_isp_csi.c             CSI subdev, MIPI PHY, lane configuration
  tx_isp_vin.c             VIN subdev (video input abstraction)
  tx_isp_fs.c              Frame source / channel devices (/dev/framechanX)
  tx_isp_tuning.c          ISP tuning ioctls, block init, AE/AWB, parameter banks
  tx_isp_subdev.c          Subdev initialization and lifecycle
  tx_isp_subdev_mgmt.c     Subdev link/pad management, graph construction
  tx_isp_v4l2.c            V4L2 interface helpers
  tx_isp_proc.c            /proc/jz/isp/ entries
  tx_isp_sysfs.c           Sysfs attribute exposure
  tx_isp_frame_done.c      Frame completion signaling
  tx_isp_ae_zone.c         AE zone statistics management
  tx_isp_fixpt.c           Fixed-point math (Q16/Q24 for gain/exposure)
  tx_isp_reset.c           Hardware reset sequences
  tx-isp-trace.c           Register change tracing to /opt/trace.txt
  tx-isp-debug.c           Debug infrastructure, logging levels, compat wrappers
  tx_isp_vic_debug.c       VIC-specific debugging and MIPI PHY monitoring
  include/                 All shared headers
```

---

## 2. Hardware Resources and Memory Map

### 2.1 MMIO Regions

| Region | Physical Address | Size | Field in `tx_isp_dev` | Purpose |
|--------|----------------:|-----:|----------------------|---------|
| ISP Core | `0x13300000` | 576 KB (`0x90000`) | `core_regs` | Main ISP register space + LSC LUT area |
| Primary VIC | `0x133e0000` | 4 KB | `vic_regs` | VIC control, MDMA, frame capture |
| Secondary VIC / W01 | `0x10023000` | 4 KB | `vic_regs2` | W01 wrapper, PHY timing coordination |
| CSI Basic / DPHY | `0x10022000` | 4 KB | `csi_regs` | MIPI CSI-2 receiver, lane control |
| PHY Registers | `0x10021000` | 4 KB | `phy_base` | MIPI PHY configuration |

All mappings are established in `tx_isp_init_memory_mappings()` via `ioremap()`.

The ISP core region is intentionally large (576 KB instead of 64 KB) to cover
hardware LUT windows used by OEM register writes at high offsets (LSC, DEIR/GIB).
Previous 64 KB mappings caused colored-blob artifacts due to unmapped register access.

### 2.2 Interrupts

| IRQ | Name | Handler | Purpose |
|----:|------|---------|---------|
| 37 | `isp-m0` | `ispcore_interrupt_service_routine()` | ISP core: frame sync, FIFO drain, bayer setup |
| 38 | `isp-w02` | `isp_vic_interrupt_service_routine()` | VIC: frame-done, MDMA completion, error tracking |

### 2.3 Clocks

Three clock sources managed through the Linux Clock Framework:

| Clock | Name | Rate | Purpose |
|-------|------|------|---------|
| CGU ISP | `cgu_isp` | 100 MHz | Parent clock generator |
| ISP Core | `isp` | (derived) | Core processing clock |
| CSI | `csi` | (derived) | MIPI interface clock |

Clock enable order: CGU parent first, then children. A 10 ms stabilization delay
follows clock enable before proceeding with register access.

---

## 3. Module Initialization and Bring-Up

### 3.1 Module Load Sequence (`tx_isp_init()`)

```
tx_isp_init()
  |
  +-- kzalloc(sizeof(struct tx_isp_dev))          Allocate global ourISPdev
  +-- spin_lock_init, refcnt=0, is_open=false      Initialize synchronization
  +-- INIT_DELAYED_WORK(&vic_frame_work)            Frame generation workqueue
  +-- tx_isp_create_vic_device(ourISPdev)           Create VIC device structure
  +-- platform_device_register()                    Register tx-isp platform device
  +-- platform_driver_register(.probe=tx_isp_platform_probe)
  +-- misc_register(/dev/tx-isp)                    Create character device
  +-- I2C infrastructure init                       Prepare for sensor registration
  +-- Set subdev ops: vic_subdev_ops, csi_subdev_ops
  +-- Register CSI, VIC, VIN, FS platform devices
  +-- tx_isp_subdev_platform_init()                 Register subdev platform drivers
  +-- tx_isp_create_subdev_graph()                  Build processing pipeline graph
  +-- tisp_code_create_tuning_node()                Create /dev/isp-m0 tuning device
```

### 3.2 Core Probe Sequence (`tx_isp_core_probe()`)

Triggered after platform device registration:

```
tx_isp_core_probe()
  |
  +-- Bind existing ourISPdev
  +-- Set driver data on all 5 platform devices
  +-- spin_lock_init, mutex_init for device and IRQ
  +-- create_singlethread_workqueue("isp_frame_sync")  Frame sync workqueue
  +-- INIT_WORK(&fs_work, ispcore_irq_fs_work)
  +-- tx_isp_subdev_init(core, &core_subdev_ops)        Register core subdev + IRQ 37
  +-- Allocate channel array (6 channels x 0xc4 bytes)
  +-- Initialize each channel: pads, locks, completions
  +-- tx_isp_core_bind_event_dispatch_tables()
  +-- tx_isp_init_memory_mappings()                     Map all MMIO regions
  +-- tx_isp_configure_clocks()                         Enable CGU/ISP/CSI clocks
```

### 3.3 Stream Start Sequence

When `libimp.so` initiates streaming:

```
1. Sensor registration:
   VIDIOC_REGISTER_SENSOR  ->  tx_isp_ispcore_activate_module_complete()

2. Core initialization:
   ispcore_core_ops_init(sd, 1)
     -> Verify sensor attributes
     -> tisp_init(width, height, fps, bayer, mode)  -- ISP block initialization
     -> Set state = INIT (3)

3. CSI initialization:
   csi_core_ops_init(sd, 1)
     -> MIPI lane configuration
     -> PHY synchronization (W01 phase)
     -> CSI enable

4. VIC start:
   tx_isp_vic_start()
     -> Configure input engine (geometry, MIPI control, crop)
     -> VIC RUN (writel(1, vic_regs + 0x0))
     -> Program IRQ registers

5. MDMA enable:
   ispvic_frame_channel_s_stream(vic_dev, 1)
     -> Program MDMA banks and strides
     -> writel(ctrl, vic_regs + 0x300)  -- MDMA output engine starts

6. Frame delivery:
   IRQ 38 (VIC) fires on frame completion
   -> vic_mdma_irq_function() delivers frames via callback
   -> IRQ 37 (ISP core) handles FIFO drain and frame sync
```

### 3.4 Module Unload (`tx_isp_exit()`)

Teardown in reverse order:

```
tx_isp_exit()
  |
  +-- cancel_delayed_work_sync(&vic_frame_work)
  +-- tx_isp_cleanup_subdev_graph()
  +-- tisp_code_destroy_tuning_node()
  +-- clk_disable_unprepare(isp_clk, csi_clk, cgu_isp)
  +-- cleanup_i2c_infrastructure()
  +-- free_irq(isp_irq), free_irq(vic_irq)
  +-- Destroy VIC queue lists, free VIC device
  +-- iounmap(vic_regs, csi_regs, core_regs, phy_base)
  +-- Sensor reset
  +-- misc_deregister(/dev/tx-isp)
  +-- Unregister platform devices/drivers
  +-- tx_isp_subdev_platform_exit()
  +-- kfree(ourISPdev)
```

---

## 4. Subdevice Graph Infrastructure

### 4.1 Subdevice Model

The driver organizes hardware blocks as a graph of subdevices connected by
pad/link abstractions:

```
         +--------+     +--------+     +--------+     +--------+
         | Sensor |---->|  CSI   |---->|  VIC   |---->|  Core  |
         | (sd 5+)|     | (sd 1) |     | (sd 0) |     | (sd 3) |
         +--------+     +---+----+     +---+----+     +---+----+
                            |              |              |
                         outpad 0       outpad 0       outpad 0
                            |              |              |
                         inpad 0        inpad 0        inpad 0
                                                          |
                                                     +----+----+
                                                     |   FS    |
                                                     | (sd 4)  |
                                                     +---------+
```

### 4.2 Fixed Subdevice Array Layout

The `subdevs[]` array in `tx_isp_dev` has fixed slot assignments.
`tx_isp_video_link_stream()` walks this array linearly, so order is critical:

| Index | Name | Device | Type |
|------:|------|--------|------|
| 0 | `isp-w02` | VIC | Frame capture |
| 1 | `isp-w01` | CSI | MIPI interface |
| 2 | `isp-w00` | VIN | Parallel input |
| 3 | `isp-m0` | Core | ISP processing |
| 4 | `isp-fs` | FS | Frame source |
| 5+ | (dynamic) | Sensor | Image sensor |

### 4.3 Core Data Structures

#### `tx_isp_subdev` (0xDC bytes)

The base structure for all ISP subdevices. Size must match the OEM SDK layout
exactly for ABI compatibility since real sensors embed this at offset 0.

```
Offset  Field              Type                    Purpose
------  -----              ----                    -------
0x00    module             tx_isp_module           Parent module descriptor
0x40+   irqdev             tx_isp_irq_device       IRQ handling (spinlock, irq, enable/disable)
0x60+   chip               tx_isp_chip_ident       Chip identification
0xb8    base               void __iomem *          Register base mapping
0xbc    res                struct resource *        Memory resource
0xc0    clks               struct clk **            Clock array
0xc4    clk_num            unsigned int             Number of clocks
0xc8    ops                tx_isp_subdev_ops *      Operations vtable
0xcc    num_outpads        unsigned short           Output pad count
0xce    num_inpads         unsigned short           Input pad count
0xd0    outpads            tx_isp_subdev_pad *      Output pad array
0xd4    inpads             tx_isp_subdev_pad *      Input pad array
0xd8    dev_priv           void *                   Device-specific private data
0xdc    host_priv          void *                   Host-specific private data (LAST FIELD)
```

**Critical constraint**: No fields after `host_priv`; real sensors embed this struct.

#### `tx_isp_subdev_pad` (0x24 bytes)

```
Field           Type                      Purpose
-----           ----                      -------
sd              tx_isp_subdev *           Parent subdevice
index           unsigned char             Pad index in entity
type            unsigned char             0x01=INPUT, 0x02=OUTPUT
links_type      unsigned char             Link type flags (DDR/LFB/FS)
state           unsigned char             FREE(0x2) / LINKED(0x3) / STREAM(0x4)
link            tx_isp_subdev_link        Active link to remote pad
event           function pointer          Event handler
event_callback  function pointer          Event callback
priv            void *                    Private data
```

#### `tx_isp_subdev_link`

```
Field    Type                      Purpose
-----    ----                      -------
source   tx_isp_subdev_pad *       Source pad
sink     tx_isp_subdev_pad *       Sink pad
reverse  tx_isp_subdev_link *      Reverse direction link
flag     unsigned int              TX_ISP_LINKFLAG_* flags
state    unsigned int              TX_ISP_MODULE_* state
```

### 4.4 Operations Interface

Each subdevice implements a set of operations through `tx_isp_subdev_ops`:

```c
struct tx_isp_subdev_ops {
    struct tx_isp_subdev_core_ops *core;      // init, reset, ioctl, ISR
    struct tx_isp_subdev_video_ops *video;    // s_stream, link_stream, link_setup
    struct tx_isp_subdev_pad_ops *pad;        // g_fmt, s_fmt, streamon, streamoff
    struct tx_isp_subdev_sensor_ops *sensor;  // release_all, sync_attr, ioctl
    struct tx_isp_subdev_internal_ops *internal;
};
```

### 4.5 Event/Notification System

Subdevices communicate via event notifications:

| Event | Category | Value | Purpose |
|-------|----------|-------|---------|
| SUBDEV_INIT | CORE_OPS | `0x1<<24` | Subdev initialization |
| SYNC_SENSOR_ATTR | CORE_OPS | | Propagate sensor attributes |
| SENSOR_REGISTER | SENSOR_OPS | `0x2<<24` | Register sensor |
| SENSOR_RELEASE | SENSOR_OPS | | Release sensor |
| SENSOR_INT_TIME | SENSOR_OPS | | Set integration time |
| SENSOR_AGAIN | SENSOR_OPS | | Set analog gain |
| SENSOR_DGAIN | SENSOR_OPS | | Set digital gain |
| FRAME_CHAN_STREAM_ON | FS_OPS | `0x3<<24` | Start streaming |
| FRAME_CHAN_STREAM_OFF | FS_OPS | | Stop streaming |
| ACTIVATE_MODULE | TUN_OPS | `0x4<<24` | Module activation |
| CORE_FRAME_DONE | TUN_OPS | | Frame completed |

Event dispatch uses `tx_isp_send_event_to_remote()` with fallback chain:
event callback -> sensor ops -> core ops -> video ops.

---

## 5. CSI Subsystem

### 5.1 Overview

The Camera Serial Interface (CSI) receives serialized MIPI data from the image
sensor, deserializes it, and feeds parallel video to the VIC input FIFO.

### 5.2 CSI Device Structure

```c
struct tx_isp_csi_device {
    struct tx_isp_subdev sd;    // Base subdev at offset 0
    struct clk *csi_clk;       // CSI clock
    int state;                 // 1=init, 2=active, 3=streaming_off, 4=streaming_on
    struct mutex mlock;        // State change protection
    int interface_type;        // 1=MIPI, 2=DVP
    int lanes;                 // MIPI lane count (1, 2, or 4)
    void __iomem *csi_regs;    // CSI basic block (0x10022000)
    void __iomem *cpm_regs;    // CPM for clock/reset
    void __iomem *phy_regs;    // MIPI PHY registers
    struct resource *phy_res;  // PHY memory resource
    spinlock_t lock;           // Register access protection
};
```

### 5.3 Register Layout

#### CSI Basic Block (`0x10022000`)

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x00` | CTRL | Control (reset, enable) |
| `0x04` | N_LANES | Lane count minus 1 (bits 0-1) |
| `0x0c` | MODE | MIPI/DVP mode select (0=off, 1=MIPI) |
| `0x10` | ENABLE | CSI enable (write 0x1) |
| `0x14` | PHY_STATE | PHY lane states (bits 0,4,8 = clock/data) |
| `0x20` | ERR1 | Protocol errors (SOT, ECC, CRC) |
| `0x24` | ERR2 | Application errors (data ID, frame sync) |
| `0x128` | LANE_MASK | Active lane bitmask |

#### Lane Mask Values

| Config | Mask | Description |
|--------|-----:|-------------|
| 1 lane | `0x31` | Clock + lane 0 |
| 2 lanes | `0x33` | Clock + lanes 0-1 |
| 4 lanes | `0x3f` | Clock + lanes 0-3 |

#### W01 Wrapper Block (`0x10023000`)

Used exclusively for MIPI PHY timing coordination:

| Offset | Purpose |
|-------:|---------|
| `0x0c` | Phase advance trigger |
| `0x14` | PHY phase status (0x200 -> 0x630 when locked) |
| `0x40` | W01 coordination status |
| `0x160` | Rate selection / timing clock |

### 5.4 MIPI Initialization Sequence

```
csi_core_ops_init(sd, enable=1)
  |
  1. Read sensor_attr->dbus_type to determine MIPI vs DVP
  2. Extract lane count: sensor_attr->mipi.lans
  3. Program N_LANES: writel((lanes - 1) & 0x3, csi_regs + 0x04)
  4. Clear control: writel(0, csi_regs + 0x0c)
  5. Deassert CSI reset in CPM[0x34]
  6. Enable MIPI mode: writel(1, csi_regs + 0x0c)
  7. Wait W01 phase: csi_wait_w01_phase(250ms)
     - Polls register 0x14 until phase advances from 0x200 to 0x630
     - Indicates MIPI lanes are synchronized
  8. Program rate_sel from sensor_attr->mipi_clk
     - Written to wrapper registers 0x160/0x1e0/0x260
  9. Set lane_mask: writel(lane_mask, csi_regs + 0x128)
  10. Kick W01: vic_write32(0x10, 1)
  11. Enable CSI: writel(1, csi_regs + 0x10)
  12. State -> 3 (ACTIVE)
```

### 5.5 Error Handling

**ERR1 Register (`0x20`)** - Protocol errors:
- Bit 0: SOT Sync Error
- Bit 2: ECC Single-bit (corrected)
- Bit 3: ECC Multi-bit (uncorrectable)
- Bit 4: CRC Error
- Bit 5: Packet Size Error

**ERR2 Register (`0x24`)** - Application errors:
- Data ID mismatch
- Frame synchronization errors

Errors are cleared by writing the status value back to the register.

---

## 6. VIC Subsystem

### 6.1 Overview

The Video Input Controller (VIC) captures frames from the CSI output FIFO and
transfers them to DRAM via MDMA (Memory DMA). It manages a 5-bank circular
buffer system for continuous frame capture.

### 6.2 Architecture

```
CSI Output -> VIC Input FIFO -> MDMA Engine -> DRAM Banks (0-4)
                                    |
                              IRQ 38 on completion
                                    |
                              Buffer rotation
                              (queue -> free -> done -> hw -> delivered)
```

The VIC has two independent engines:
- **Input engine**: Activated by VIC RUN (`vic_regs + 0x0 = 1`), captures from CSI
- **Output engine**: Activated by MDMA enable (`vic_regs + 0x308 = 1`), writes to DRAM

These are controlled independently and sequentially in the pipeline.

### 6.3 Key Register Layout (`0x133e0000`)

#### Control and Configuration

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x00` | VIC_RUN | Phase control: write 2/4 to configure, 1 to run |
| `0x04` | GEOMETRY | Frame size: `width[31:16] \| height[15:0]` |
| `0x0c` | INTERFACE | 0=DVP, 1=internal, 2=MIPI |
| `0x10` | FRAME_TYPE | Frame type and mode |
| `0x14` | DATA_TYPE | Sensor format (0x2b=RAW10) |

#### MIPI Configuration

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x100` | BITS_PACKED | Packed bits per line |
| `0x104-0x108` | CROP_Y | Y-axis crop windows (packed pairs) |
| `0x10c` | MIPI_CTRL | MIPI control bits (hcrop, vcomp, hcomp, etc.) |
| `0x110-0x11c` | CROP_X | X-axis crop windows |
| `0x1a0` | UNLOCK | MIPI configuration unlock |
| `0x1a4` | MIPI_MODE | 0xa000a=standard, 0x100010=SONY mode |
| `0x1a8-0x1ac` | FRAME_MODE | 0x4440=linear, 0x4140=2-frame WDR |

#### MDMA Engine

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x300` | MDMA_CTRL | Enable + frame count + format: `cnt[31:16] \| 0x80000020 \| fmt[5:0]` |
| `0x304` | MDMA_SIZE | Frame dimensions: `width[31:16] \| height[15:0]` |
| `0x308` | MDMA_ENABLE | Write 1 to activate MDMA output |
| `0x310` | STRIDE_Y | Y-plane line stride in bytes |
| `0x314` | STRIDE_UV | UV-plane line stride in bytes |
| `0x318-0x328` | Y_BANK[0-4] | Y-plane DMA addresses (5 banks) |
| `0x32c-0x33c` | UV_BANK[0-4] | UV shadow / NV12 addresses |
| `0x340-0x350` | CH1_BANK[0-4] | Channel-1 addresses (dual capture) |
| `0x380` | CURRENT_ADDR | Hardware current DMA pointer (read-only) |

Bank register offset calculation: `offset = (buffer_index + 0xc6) << 2`

#### Interrupt Registers

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x1e0` | IRQ_STATUS_1 | Frame-done and error status |
| `0x1e4` | IRQ_STATUS_2 | MDMA completion status |
| `0x1e8` | IRQ_MASK_1 | Mask register (bit=1 means MASKED) |
| `0x1ec` | IRQ_MASK_2 | MDMA mask register |
| `0x1f0` | IRQ_CLEAR_1 | Write to acknowledge interrupts |
| `0x1f4` | IRQ_CLEAR_2 | Write to acknowledge MDMA |

**Note**: Interrupt mask logic is inverted from typical convention.
`Pending = (~mask) & status`.

Standard configuration:
```
0x1e8 = 0xFFFFFFFE  (unmask bit 0 only: frame_done)
0x1ec = 0xFFFFFFFC  (unmask bits 0,1: MDMA ch0/ch1 done)
```

### 6.4 VIC Start Sequence (MIPI)

```
tx_isp_vic_start(vic_dev)
  |
  1. Read sensor attributes (mipi_sc from cached copy)
  2. Determine MIPI mode: SONY vs standard
     - SONY: reg_1a4 = 0x100010
     - Other: reg_1a4 = 0xa000a
  3. Calculate bits_per_pixel from sensor_csi_fmt:
     - RAW8=8, RAW10=10, RAW12=12, YUV422=16
  4. Program packed bits: vic_regs[0x100] = ((bpp * total_width) + 0x1f) >> 5
  5. Set interface mode: vic_regs[0x0c] = 2 (MIPI)
  6. Set data type: vic_regs[0x14] = sensor_csi_fmt
  7. Set geometry: vic_regs[0x04] = (width << 16) | height
  8. Program MIPI control: vic_regs[0x10c]
  9. Program crop windows: vic_regs[0x110-0x11c]
  10. Set frame mode: vic_regs[0x1a8/0x1ac] = frame_mode
  11. Arm sequence (CRITICAL ORDER):
      vic_regs[0x00] = 2       // Phase 1
      vic_regs[0x00] = 4       // Phase 2
      vic_regs[0x1a0] = unlock // Unlock MIPI config
  12. Wait for arm: poll vic_regs[0x00] returns to 0 (100ms timeout)
  13. Set Y-axis crop: vic_regs[0x104/0x108]
  14. VIC RUN: vic_regs[0x00] = 1   <-- Input engine starts
      wmb()
  15. Program IRQ registers
  16. State = 4 (STREAMING)
```

### 6.5 MDMA Enable (Separate from VIC Start)

```
ispvic_frame_channel_s_stream(vic_dev, enable=1)
  |
  1. vic_pipo_mdma_enable(): program stride/size registers
     - vic_regs[0x308] = 1                    // MDMA enable
     - vic_regs[0x304] = (width << 16) | height
     - vic_regs[0x310] = stride               // Y stride
     - vic_regs[0x314] = stride               // UV stride
  2. Program bank addresses: vic_regs[0x318-0x328]
  3. Program MDMA control: vic_regs[0x300] = (count << 16) | 0x80000020
  4. stream_state = 1
```

### 6.6 VIC Error Tracking

The VIC tracks 13 error categories from IRQ status bits:

| Bit | Error | Counter |
|----:|-------|---------|
| 0x200 | Frame ASFIFO overflow | `vic_errors[0]` |
| 0x400-0x2000 | Horizontal errors (CH0-CH3) | `vic_errors[1-4]` |
| 0x4000-0x20000 | Vertical errors (CH0-CH3) | `vic_errors[5-8]` |
| 0x200000 | Control limit error | `vic_errors[9]` |
| 0x400000 | Image SYFIFO overflow | `vic_errors[10]` |
| 0x800000+ | MIPI errors (FID/HCOMP/VCOMP/CHID) | `vic_errors[11-12]` |

---

## 7. ISP Core Processing Pipeline

### 7.1 Top-Level Block Architecture

The ISP core at `0x13300000` contains multiple processing blocks controlled by a
top-level bypass register at offset `0x0c`:

```
Input (Bayer) -> DPC -> LSC -> GIB -> [ADR] -> DMSC -> [Gamma] ->
  [Defog] -> CLM/BCSH -> [Sharpen] -> SDNS -> MDNS -> YDNS -> Output (NV12)
```

### 7.2 Top Bypass Register (ISP `0x0c`)

Block enable/bypass is computed in `tisp_compute_top_bypass_from_params()`:

| Bit | Block | Purpose |
|----:|-------|---------|
| 2 | DPC | Defect Pixel Correction |
| 4 | LSC | Lens Shading Correction |
| 5 | GIB | Green Imbalance |
| 7 | ADR | Adaptive Dynamic Range |
| 8 | DMSC | Demosaic |
| 10 | Gamma | Tone mapping LUT |
| 11 | Defog | Haze removal |
| 12 | CLM/BCSH | Color luminance / brightness-contrast-saturation-hue |
| 14 | Sharpen | Detail enhancement |
| 15 | SDNS | Spatial denoise |
| 16 | MDNS | Motion denoise |
| 17 | YDNS | Luma denoise |
| 31 | Top sel | Set on first interrupt (`tisp_top_sel()`) to release processing |

### 7.3 ISP Processing Blocks

#### DPC (Defect Pixel Correction)
- Detects and corrects hot/dead pixels in the raw Bayer image
- Parameter range: 0xe6-0x104

#### LSC (Lens Shading Correction)
- Compensates for lens vignetting using calibration LUT tables
- Large LUT area in high ISP register offsets
- Parameter range: 0x54-0x5e

#### GIB (Green Imbalance)
- Corrects green channel imbalance between Gr and Gb pixels
- Parameter range: 0x3f5-0x3fe

#### ADR (Adaptive Dynamic Range)
- Tone-mapping for dynamic range compression
- LUT-based: 68 words at registers 0x4084-0x428c
- Control parameters at 0x4004-0x4068
- Knee points at 0x406c-0x4080

#### DMSC (Demosaic)
- Converts raw Bayer pattern to full-color RGB
- Critical: wrong CFA phase causes visible corruption
- `mbus_to_bayer_write()` writes register `0x8` with live CFA/Bayer pattern

#### CCM (Color Correction Matrix)
- 3x3 matrix for color space transformation
- Parameter range: 0xa9-0xb4

#### Gamma
- LUT-based tone curve
- Two tables: `tiziano_gamma_lut_linear[256]` and `tiziano_gamma_lut_wdr[256]`
- Written to hardware via `tiziano_gamma_lut_parameter()`

#### BCSH (Brightness/Contrast/Saturation/Hue)
- User-adjustable image quality parameters
- Controlled via ISP_CTRL_BRIGHTNESS (0x980900), etc.

#### Sharpen
- Edge enhancement filter
- Strength controlled by ISP_CTRL_SHARPNESS (0x98091b)

#### SDNS / MDNS / YDNS / RDNS (Denoise Family)
- **SDNS**: Spatial denoise (3D spatial filtering)
- **MDNS**: Motion denoise (temporal filtering). Currently runtime-parked due to stability.
  - Globals at 0x9ab00 (ratio=0x80), 0x9a9d0 (interpolation key=0x10000)
- **YDNS**: Luma denoise. Registers at 0x7af0-0x7afc
- **RDNS**: Raw denoise (pre-demosaic filtering)

#### Defog
- Haze/fog removal
- Per-frame hook via `tisp_defog_on_frame()` called from frame-done ISR

#### WDR (Wide Dynamic Range)
- Multiple exposure fusion
- Frame modes: linear (0x4440), 2-frame WDR (0x4140), 3-frame WDR (0x4240)
- Switched via `tisp_s_wdr_en()` with ordered re-initialization

### 7.4 ISP Initialization Order

`tisp_init()` initializes blocks in OEM-inspired order:

```
1. AE (Auto Exposure)
2. AWB (Auto White Balance)
3. Gamma
4. GIB / LSC / CCM / DMSC
5. Sharpen / SDNS / MDNS / CLM
6. DPC / Defog / ADR / AF / BCSH / YDNS / RDNS
```

### 7.5 MSCA Output (Processed Frame Delivery)

The ISP core writes processed frames to user buffers via MSCA registers
(Memory Storage Channel A), separate from VIC's raw MDMA:

| Register | Offset | Purpose |
|----------|-------:|---------|
| MSCA Y address | `0x996c + (channel << 8)` | Y-plane output DMA address |
| MSCA UV address | `0x9984 + (channel << 8)` | UV-plane output DMA address |

These are programmed by QBUF ioctls, routing ISP-processed output to user buffers.

**Key design point**: VIC MDMA captures raw frames into internal banks. MSCA routes
processed output to user-provided buffers. These paths are independent.

### 7.6 First-Frame Behavior

During stream-on / early ISR handling:

1. `mbus_to_bayer_write()` writes ISP register `0x8` with the CFA/Bayer pattern
   derived from sensor mbus format + `shvflip`
2. `tisp_top_sel()` sets bit 31 of register `0x0c` on first interrupt to
   release top-level processing
3. Guard variable `bayer_write_pending` ensures one-shot execution

If CFA phase is wrong, demosaic output is visibly corrupted (color artifacts).

---

## 8. Frame Source and Delivery

### 8.1 Frame Channel Architecture

The Frame Source (FS) subsystem exposes up to 6 frame channels as character
devices at `/dev/framechanX`:

```
/dev/framechan0  -  Channel 0 (primary, 1920x1080)
/dev/framechan1  -  Channel 1 (secondary, 1920x1080)
/dev/framechan2  -  Channel 2 (additional)
/dev/framechan3  -  Channel 3 (additional)
...
```

### 8.2 Channel Structure

Each channel (`struct isp_channel`) contains:

| Field | Type | Purpose |
|-------|------|---------|
| `id` | int | Channel ID (0-5) |
| `state` | uint32_t | Channel state (1=active, 3=ready, 4=streaming) |
| `attr` | `imp_channel_attr` | Format/size attributes (0x50 bytes) |
| `pool` | nested struct | VBM pool management |
| `dma_addr` | dma_addr_t | DMA base address |
| `buffer_dma_addrs` | dma_addr_t * | Per-buffer DMA addresses |

### 8.3 Channel Attributes (`imp_channel_attr`, 0x50 bytes)

```
Offset  Field              Type        Purpose
------  -----              ----        -------
0x00    enable             uint32_t    Channel enabled
0x04    width              uint32_t    Frame width
0x08    height             uint32_t    Frame height
0x0c    format             uint32_t    Pixel format
0x10    crop_enable        uint32_t    Crop enabled
0x14-0x20  crop            struct      Crop rectangle (x, y, width, height)
0x24    scaler_enable      uint32_t    Scaler enabled
0x28    scaler_outwidth    uint32_t    Scaler output width
0x2c    scaler_outheight   uint32_t    Scaler output height
0x30    picwidth           uint32_t    Picture width
0x34    picheight          uint32_t    Picture height
0x38    fps_num            uint32_t    FPS numerator
0x3c    fps_den            uint32_t    FPS denominator
0x40-0x4f  reserved        uint32_t[4] Padding to 0x50
```

### 8.4 IOCTL Interface

| Command | Value | Purpose |
|---------|------:|---------|
| REQBUFS | `0xc0145608` | Allocate N buffers |
| QBUF | `0xc044560f` | Queue buffer for capture |
| DQBUF | `0xc0445609` | Dequeue completed frame |
| DQBUF (alt) | `0xc0445611` | Alternative dequeue |
| G_FMT | `0xc0051600` | Get format |
| S_FMT | `0xc0095601` | Set format |
| STREAMON | `0xc0105600` | Start streaming |
| STREAMOFF | `0xc0105601` | Stop streaming |

### 8.5 QBUF/DQBUF Flow

**QBUF** (user enqueues buffer):
```
1. User provides struct v4l2_buffer with index, type, memory model
2. For USERPTR: physical address from buffer.m.userptr
3. Fallback: phys = 0x6300000 + (index * buffer_size)
4. Write MSCA registers:
   - Y addr: core_regs + (channel << 8) + 0x996c
   - UV addr: core_regs + (channel << 8) + 0x9984
5. Track in state->queued_buffers list
```

**DQBUF** (user retrieves completed frame):
```
1. Block on frame completion (wait_event or completion object)
2. Pop from state->completed_buffers list
3. Return buffer metadata: index, bytesused, flags, sequence, timestamp
4. Pre-dequeue optimization reserves one frame for low-latency path
```

### 8.6 Frame Done Signaling (`tx_isp_frame_done.c`)

```c
static atomic64_t frame_done_cnt = ATOMIC64_INIT(0);
static int frame_done_cond = 0;
static DECLARE_WAIT_QUEUE_HEAD(frame_done_wait);
```

On VIC IRQ frame completion:
```
isp_frame_done_wakeup()
  |
  +-- atomic64_inc(&frame_done_cnt)
  +-- ourISPdev->frame_count++
  +-- tisp_defog_on_frame()          Per-frame defog hook
  +-- frame_done_cond = 1
  +-- wake_up(&frame_done_wait)      Unblock all waiters
```

Exported APIs:
- `isp_frame_done_wait(timeout_ms)` - Block until next frame
- `isp_frame_done_wait_ex(timeout_ms, out[2])` - Extended version with counters
- `isp_frame_done_get_count()` - Non-blocking counter read (u64)

---

## 9. Tuning Subsystem

### 9.1 Overview

The tuning subsystem (`tx_isp_tuning.c`, ~20,000 lines) is the largest component.
It manages all ISP image processing parameters through `/dev/isp-m0` and controls
the initialization, runtime adjustment, and mode switching of every ISP block.

### 9.2 M0 Device IOCTL Interface

**Device**: `/dev/isp-m0` (miscdevice)

**Command structure**: `0x?000?4XX` where XX encodes parameter family.

| Command | Value | Purpose |
|---------|------:|---------|
| GET_PARAM | `0x20007400` | Get tuning parameter config |
| SET_PARAM | `0x20007401` | Set tuning parameter config |
| GET_AE_INFO | `0x20007403` | Get AE statistics |
| SET_AE_INFO | `0x20007404` | Set AE parameters |
| GET_AWB_INFO | `0x20007406` | Get AWB statistics |
| SET_AWB_INFO | `0x20007407` | Set AWB parameters |
| GET_STATS | `0x20007408` | Get general statistics |
| GET_STATS2 | `0x20007409` | Get statistics v2 |

**V4L2 control codes** (via ISP_CORE_S_CTRL / ISP_CORE_G_CTRL):

| Control | Value | Purpose |
|---------|------:|---------|
| BRIGHTNESS | `0x980900` | Brightness adjustment |
| CONTRAST | `0x980901` | Contrast adjustment |
| SATURATION | `0x980902` | Saturation adjustment |
| HFLIP | `0x980914` | Horizontal flip |
| VFLIP | `0x980915` | Vertical flip |
| SHARPNESS | `0x98091b` | Sharpness control |
| DPC | `0x98091f` | Defect pixel correction |
| GAMMA | `0x9a091a` | Gamma correction |
| ANTIFLICKER | `0x980918` | Anti-flicker |
| BYPASS | `0x8000164` | ISP bypass mode |

### 9.3 Parameter Family Ranges

| Family | ID Range | Purpose |
|--------|----------|---------|
| GB | 0x3f5-0x3fe | Green Balance |
| LSC | 0x54-0x5e | Lens Shading |
| WDR | 0x3ff-0x431 | Wide Dynamic Range |
| DPC | 0xe6-0x104 | Defect Pixel |
| RDNS | 0x432-0x446 | Rearrangement Denoise |
| ADR | 0x380-0x3ab | Adaptive Dynamic Range |
| CCM | 0xa9-0xb4 | Color Correction |
| Gamma | 0x3c (linear), 0x3d (WDR) | Tone curve |

### 9.4 AE Algorithm

**Statistics collection**:
- AE0 buffer (0x6000 bytes) collected via `ae0_interrupt_static()`
- 256-zone grid parsing for luminance statistics
- Histogram modes: color and B/W

**Processing path**:
```
ISP core IRQ (bit 0x1000) -> queue_work(fs_workqueue)
  -> ispcore_irq_fs_work()
    -> Read AE statistics from hardware DMA buffer
    -> tisp_ae0_process() runs AE algorithm
    -> Write exposure/gain to sensor via notify events
```

**AE zone data** (`tx_isp_ae_zone.c`):
- 15x15 zone grid = 225 zones, 0x384 bytes
- Updated per-frame via `tisp_ae_update_zone_data()`
- Read by user-space via `tisp_g_ae_zone()` -> `copy_to_user()`

### 9.5 AWB Algorithm

**Zone statistics**:
- 15x15 grid = 225 zones
- Per-zone: R, G, B, IR channels (4 u32 per zone)
- Arrays: `awb_array_r[225]`, `awb_array_g[225]`, `awb_array_b[225]`, `awb_array_ir[225]`

**Mode control**:
- Auto mode: continuous AWB updates
- Manual mode: freeze flag locks current white balance
- CCT override for manual color temperature

### 9.6 Tuning Data Structure

**Tuning data offsets** (from `isp_tuning_data`):

| Offset | Field | Purpose |
|-------:|-------|---------|
| 0x68 | saturation | Saturation value |
| 0x6c | brightness | Brightness value |
| 0x70 | contrast | Contrast value |
| 0x74 | sharpness | Sharpness value |
| 0x78-0x7c | hflip/vflip/antiflicker | Flip and flicker controls |
| 0x80-0x8c | running_mode/ae_comp | Mode and exposure compensation |
| 0x90-0x9c | max_again/max_dgain/exposure | Gain controls |
| 0xa0-0xac | strength controls | DRC, DPC, defog, sinter |
| 0xb4-0xc0 | wb_gains/wb_temp | White balance |
| 0xc4-0xc7 | BCSH controls | BCSH sub-parameters |
| 0xc8-0xcc | fps_num/fps_den | Frame rate control |

### 9.7 Banked Modes and Runtime Switching

Several subsystems maintain separate linear and WDR banks:

- **Day/Night switching**: Swaps `tparams_day`, `tparams_night`, `tparams_active`
- **WDR transition**: `tisp_s_wdr_en()` triggers ordered re-initialization of
  multiple per-block parameter banks
- Mode changes are not single-bit toggles; they require re-initialization
  of multiple dependent blocks

### 9.8 DMA-Backed Statistics

The driver uses DMA-fed runtime statistics for:
- AE: Exposure histogram and zone luminance
- AWB: Per-zone color ratios
- ADR: Dynamic range distribution
- DPC: Defect pixel detection data

---

## 10. Interrupt Architecture

### 10.1 Dual-IRQ Model

```
IRQ 37 (isp-m0)                    IRQ 38 (isp-w02)
+-----------------------+           +-----------------------+
| ISP Core Handler      |           | VIC Handler           |
|                       |           |                       |
| 1. Read status 0xb4   |           | 1. Read status        |
| 2. Ack via 0xb8       |           |    (~0x1e8) & 0x1e0   |
| 3. Error check 0x3f8  |           |    (~0x1ec) & 0x1e4   |
| 4. Frame sync work    |           | 2. Ack via 0x1f0/0x1f4|
|    (bit 0x1000)       |           | 3. Frame done (bit 0) |
| 5. Bayer pattern write|           |    -> vic_framedone    |
|    (one-shot)         |           | 4. MDMA done           |
| 6. CH0/1/2 FIFO drain|           |    -> vic_mdma_irq     |
|    (read 0x9974)      |           | 5. Error tracking     |
| 7. Frame count++      |           |    (13 categories)    |
+-----------------------+           +-----------------------+
```

### 10.2 ISP Core Handler (`ispcore_interrupt_service_routine()`)

**Entry**: Triggered by IRQ 37. Uses global `ourISPdev` (NOT dev_id).

```
1. Read interrupt_status = readl(isp_regs + 0xb4)
2. Acknowledge: writel(interrupt_status, isp_regs + 0xb8)
3. Error check: if (status & 0x3f8) -> log error bits
4. Frame sync: if (status & 0x1000)
   -> queue_work_on(0, fs_workqueue, &fs_work)
5. Bayer pattern: if (bayer_write_pending)
   -> mbus_to_bayer_write()  (one-shot, clears flag)
6. Channel FIFO drain:
   -> Read Y addresses from isp_regs + 0x9974
   -> Signal: frame_chan_event(&frame_channels[0], 0x3000006, evt)
7. Increment ourISPdev->frame_count
```

### 10.3 VIC Handler (`isp_vic_interrupt_service_routine()`)

**Entry**: Triggered by IRQ 38.

```
1. Read pending interrupts:
   v1_7  = (~readl(vic_regs + 0x1e8)) & readl(vic_regs + 0x1e0)
   v1_10 = (~readl(vic_regs + 0x1ec)) & readl(vic_regs + 0x1e4)

2. Acknowledge:
   writel(v1_7,  vic_regs + 0x1f0)
   writel(v1_10, vic_regs + 0x1f4)

3. Frame done (v1_7 & 0x1):
   -> vic_dev->frame_count++
   -> vic_framedone_irq_function(vic_dev)

4. MDMA completion (v1_10):
   -> vic_mdma_irq_function(vic_dev)
      Non-streaming: cycle banks, complete after both channels
      Streaming: pop done_head, deliver via raw_pipe callback, refill banks

5. Error bits (v1_7):
   0x200      -> ASFIFO overflow
   0x400-2000 -> Horizontal errors per channel
   0x4000+    -> Vertical/MIPI errors
```

**Gating**: Handler checks `vic_start_ok != 0` before processing frames.

### 10.4 Frame Sync Work

The ISP core handler queues work on CPU 0 for frame synchronization:

```
fs_workqueue (single-threaded)
  -> ispcore_irq_fs_work()
    -> Process AE statistics from DMA buffer
    -> Run AE algorithm (tisp_ae0_process)
    -> Update sensor exposure/gain via notify events
    -> Process AWB if enabled
    -> Update tuning parameters for next frame
```

---

## 11. Buffer Management

### 11.1 Three-List Buffer Rotation

The VIC manages three linked lists for buffer lifecycle:

```
User QBUF                              User DQBUF
    |                                      ^
    v                                      |
+----------+     +----------+     +--------+--+
| queue_hd | --> | free_hd  | --> | done_hd   |
| (pending)|     | (hw slot)|     | (complete)|
+----------+     +----------+     +-----------+
    |                 |                 ^
    +---> rotate ---->+----> HW bank ->-+
                      |   (DMA write)   |
                      +-- MDMA IRQ -----+
```

### 11.2 QBUF Path (`ispvic_frame_channel_qbuf`)

Under `buffer_mgmt_lock` spinlock:

```
1. Add incoming buffer to queue_head (tail)
2. If both free_head and queue_head non-empty:
   a. Pop queued_buffer from queue_head
   b. Pop bank_buffer from free_head
   c. Copy address: bank_buffer->buffer_addr = queued_buffer->buffer_addr
   d. Write VIC MDMA bank register:
      writel(addr, vic_base + ((bank_buffer->buffer_index + 0xc6) << 2))
   e. Move bank_buffer to done_head (tail)
   f. Increment active_buffer_count
```

### 11.3 MDMA IRQ Delivery Path (`vic_mdma_irq_function`)

**Non-streaming mode** (snapshot/calibration):
```
1. Cycle through banks: next_idx = (cur_idx + 1) % 5
2. Update bank address
3. Decrement sub_get_num counters
4. When both channels done: complete(&frame_complete)
```

**Streaming mode** (normal operation):
```
1. Pop done_buf from done_head
2. Decrement active_buffer_count
3. Deliver via raw_pipe[1] callback (signals ISP core)
4. Recycle done_buf to free_head
5. Refill: pop queue -> pop free -> program hw bank -> push done
6. Increment active_buffer_count
```

### 11.4 MDMA Bank Layout

Five pre-allocated banks for ping-pong buffering:

```
Bank 0: vic_regs + 0x318  (Y-plane address)
Bank 1: vic_regs + 0x31c
Bank 2: vic_regs + 0x320
Bank 3: vic_regs + 0x324
Bank 4: vic_regs + 0x328

UV banks at 0x32c-0x33c (NV12) or 0x340-0x350 (dual-channel)
```

Hardware rotates through banks as frames complete. The driver refills from the
queue on each MDMA completion IRQ.

---

## 12. State Machines

### 12.1 Module State (`tx_isp_module_state`)

All subdevices share this four-state lifecycle:

```
UNDEFINE (0)  -->  SLAKE (1)  -->  ACTIVATE (2)  -->  INIT (3)  -->  RUNNING (4)
                      ^                                                    |
                      |                                                    |
                      +----------------------------------------------------+
                              (stream stop / deactivate)
```

| State | Value | Meaning |
|-------|------:|---------|
| UNDEFINE | 0 | Not initialized |
| SLAKE | 1 | Powered down / idle |
| ACTIVATE | 2 | Transitioning / activating |
| INIT | 3 | Initialized and ready |
| RUNNING | 4 | Actively streaming |

### 12.2 CSI State Machine

```
    init (0)
      |
      v
    IDLE (1) -------> ACTIVE (2) -------> STREAMING (4)
      ^                   |                     |
      |                   |                     |
      +-------------------+---------------------+
              (csi_core_ops_init(0))
```

### 12.3 VIC State Machine

```
    init (1)
      |
      v
    ready (2) -----> STREAMING (4)
      |                  |
      v                  |  (VIC RUN + MDMA enable)
    error (3)            |
                         v
                    Frame capture active
                    IRQ 38 delivers frames
```

### 12.4 Channel State Machine

```
    Uninitialized
      |
      v
    Active (1) --------> Ready (3) --------> Streaming (4)
      ^                    |                       |
      |                    |                       |
      +--------------------+-----------------------+
                     (STREAMOFF)
```

**Channel state flags**:
- `enabled`: Channel enabled in device
- `streaming`: Data actively flowing
- `state`: 3=ready, 4=streaming (at offset 0x2d0)
- `flags`: bit 0 = streaming active (at offset 0x230)

### 12.5 VIN State (`ourISPdev->vin_state`)

```
SLAKE (1)  -->  ACTIVATE (2)  -->  INIT (3)  -->  RUNNING (4)
                                     ^                 |
                                     |                 |
                                     +--(stream stop)--+
```

Set at key transitions:
- `vin_state = INIT` during core probe
- `vin_state = RUNNING` on stream start
- `vin_state = INIT` on stream stop

---

## 13. Register Reference

### 13.1 ISP Core Registers (`0x13300000`)

#### Control Block

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x00` | ISP_CTRL | Control: EN(0), RST(1), CAPTURE(2), WDR_EN(3) |
| `0x04` | ISP_STATUS | Status: IDLE, BUSY, ERROR |
| `0x08` | CFA_PATTERN | Bayer CFA pattern (written by `mbus_to_bayer_write()`) |
| `0x0c` | TOP_BYPASS | Block bypass control (see section 7.2) |

#### Interrupt Registers

| Offset | Name | Purpose |
|-------:|------|---------|
| `0xb4` | INT_STATUS | Interrupt status (read to check, write to ack) |
| `0xb8` | INT_ACK | Interrupt acknowledge |

#### MSCA Output

| Offset | Name | Purpose |
|-------:|------|---------|
| `0x996c` | MSCA_Y_ADDR | Channel 0 Y-plane output DMA address |
| `0x9974` | MSCA_Y_FIFO | Channel 0 Y-plane FIFO read |
| `0x9984` | MSCA_UV_ADDR | Channel 0 UV-plane output DMA address |

For channel N: add `(N << 8)` to base offset.

### 13.2 Tuning Register Windows

#### ADR/DRC Registers

| Range | Purpose |
|-------|---------|
| `0x4004-0x4068` | ADR control parameters |
| `0x406c-0x4080` | ADR knee points |
| `0x4084-0x428c` | ADR LUT main window (68 words) |
| `0x4294-0x433c` | ADR extra parameters |
| `0x4340-0x4458` | CTC/COC knee points |

#### YDNS Registers

| Range | Purpose |
|-------|---------|
| `0x7af0-0x7afc` | YDNS control parameters |

#### MDNS Registers

| Range | Purpose |
|-------|---------|
| `0x9a9d0` | MDNS interpolation key (0x10000) |
| `0x9ab00` | MDNS ratio (0x80) |

#### Gamma LUT

| Offset | Purpose |
|-------:|---------|
| `0xBB30+` | Gamma LUT entries (256 words) |

### 13.3 Sensor Info Blob (`tisp_sensor_info_blob`, 0x60 bytes)

| Word | Purpose |
|-----:|---------|
| 0 | Width |
| 1 | Height |
| 2 | Bayer pattern |
| 3 | FPS |
| 6 | Integration time |
| 7 | Analog gain |
| 8 | Digital gain |
| 9 | Max analog gain |
| 10 | Max digital gain |
| 11 | Line time (upper:lower) |
| 12 | Min integration time |
| 15 | Total size (height:width packed) |
| 16 | Max integration time |
| 22 | Mode |
| 23 | Flags |

---

## 14. Debug and Diagnostics

### 14.1 Proc Filesystem (`/proc/jz/isp/`)

| Entry | Purpose | Write Commands |
|-------|---------|----------------|
| `isp-w00` | VIN frame counter | - |
| `isp-w01` | CSI status/frame count | `snapraw`, `enable`, `disable` |
| `isp-w02` | VIC statistics (critical format) | `snapraw [N]`, `snapnv12 [N]`, `enable`, `disable` |
| `vic-mdma` | MDMA register snapshots (5 banks) | - |
| `isp-fs` | Frame source status | `enable`, `disable` |
| `isp-m0` | ISP core status | `enable`, `disable` |
| `clks` | Clock rates (cgu_isp, isp, csi) | - |

**isp-w02 output format** (OEM-compatible):
```
 <frame_count>, 0
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
```

### 14.2 Register Tracing (`tx-isp-trace.c`)

Writes to `/opt/trace.txt` with register change tracking:

- Monitors 4 regions: isp-w01 (0x10023000), isp-m0 (0x13300000),
  isp-w02 (0x133e0000), isp-csi (0x10022000)
- Snapshots last value per register
- Detects old->new transitions
- Groups sequential writes (>4 register sequences)
- Region classification: CONTROL, DATA, TUNING, UNKNOWN

### 14.3 VIC Debug (`tx_isp_vic_debug.c`)

**Functions**:
- `tx_isp_vic_start_streaming()`: Validates dimensions, configures VIC, enables IRQs
- `tx_isp_vic_stop_streaming()`: Disables interrupts without hardware reset
- `tx_isp_mipi_phy_status_check()`: PHY lane and clock status
- `tx_isp_vic_register_dump()`: Dump all VIC control registers
- `tx_isp_debug_frame_capture_status()`: Compare driver vs hardware frame counts

**Error counters** (13 categories, lifetime tracking):
```
data_b299c: Frame ASFIFO overflow
data_b29a0: General error
data_b2974: VER error CH0
data_b2978: HVF error
data_b297c: DVP hcomp error
data_b2980: DMA syfifo overflow
data_b2984: Control limit error
data_b2988: Image syfifo overflow
data_b298c: MIPI FID asfifo overflow
data_b2990: MIPI CH0 hcomp error
data_b2994: MIPI CH0 vcomp error
data_b2998: DMA chid overflow
```

### 14.4 Sysfs Attributes

Under `/sys/devices/.../<device>/`:

| Attribute | Access | Purpose |
|-----------|--------|---------|
| `sensor_info` | RO | Name, resolution, interface type |
| `wdr_mode` | RW | WDR mode get/set |
| `streaming` | RW | Streaming state control |
| `statistics` | RO | Frame count, AE state |

### 14.5 Debug Module Parameters

| Parameter | Default | Purpose |
|-----------|---------|---------|
| `print_level` | ISP_WARN_LEVEL | Logging verbosity |
| `isp_ch0_pre_dequeue_time` | (varies) | Channel timing |
| `isp_memopt` | (varies) | Memory optimization flags |
| `isp_day_night_switch_drop_frame_num` | (varies) | Frame drop on mode switch |

### 14.6 Hardware Reset (`tx_isp_reset.c`)

**Reset register**: `0xb00000c4`

| Bit | Purpose |
|----:|---------|
| 20 | Hardware ready flag |
| 21 | Reset trigger |
| 22 | Reset complete |

**Sequence**: Assert bit 21 -> poll bit 20 -> check bit 22 (1s timeout).

---

## 15. End-to-End Data Flow

### 15.1 Complete Frame Capture Example (1920x1080 RAW10 MIPI)

```
SENSOR OUTPUT
  4 MIPI lanes, 10-bit Bayer, 1920x1080

CSI INITIALIZATION
  ├─ Program N_LANES = 3 (4 lanes - 1)
  ├─ Set lane_mask = 0x3f (all 4 lanes)
  ├─ Enable MIPI mode
  ├─ Calculate and program rate_sel
  ├─ Wait W01 phase lock (0x200 -> 0x630)
  └─ Enable CSI

VIC INITIALIZATION
  ├─ Read sensor MIPI control attributes
  ├─ Set MIPI mode (standard or SONY)
  ├─ Program packed bits = ((10 * 1920) + 31) >> 5
  ├─ Set geometry = (1920 << 16) | 1080
  ├─ Program crop windows
  ├─ Set frame mode (0x4440 = linear)
  ├─ Arm: write 2, write 4, unlock
  ├─ Wait arm complete
  └─ VIC RUN (input engine starts)

MDMA ENABLE (separate step)
  ├─ Program stride = 1920 * 2 = 3840 bytes
  ├─ Program bank addresses (5 banks)
  ├─ Set frame dimensions
  └─ Enable MDMA (output engine starts)

PER-FRAME CAPTURE
  ├─ CSI deserializes MIPI data to parallel stream
  ├─ VIC FIFO absorbs and buffers incoming data
  ├─ MDMA writes frame to current bank in DRAM
  └─ Bank rotation: 0 -> 1 -> 2 -> 3 -> 4 -> 0...

IRQ 38 FIRES (VIC frame done)
  ├─ Read and acknowledge interrupt status
  ├─ Pop completed buffer from done_head
  ├─ Deliver via raw_pipe callback to ISP core
  ├─ Recycle buffer to free_head
  └─ Refill next bank from queue

IRQ 37 FIRES (ISP core)
  ├─ Read and acknowledge status at 0xb4/0xb8
  ├─ On first frame: mbus_to_bayer_write() + tisp_top_sel()
  ├─ Queue AE/AWB work on frame sync workqueue
  ├─ Drain CH0/1/2 FIFOs (read 0x9974)
  └─ Signal frame_chan_event for channel completion

ISP PROCESSING
  ├─ Raw Bayer -> DPC -> LSC -> GIB -> ADR
  ├─ -> DMSC (Bayer to RGB)
  ├─ -> Gamma -> Defog -> CLM/BCSH
  ├─ -> Sharpen -> SDNS -> MDNS -> YDNS
  └─ -> NV12 output to MSCA

USER RECEIVES FRAME
  ├─ QBUF: User provides buffer -> MSCA addresses programmed
  ├─ ISP writes processed NV12 to MSCA output
  ├─ Frame completion wakes DQBUF waiters
  ├─ DQBUF: Returns frame metadata (index, size, timestamp, sequence)
  └─ User reads NV12 frame data from mapped buffer
```

### 15.2 Tuning Parameter Update Path

```
User opens /dev/isp-m0
  |
  v
IOCTL: SET_PARAM (0x20007401)
  |
  v
tisp_code_tuning_ioctl() routes by parameter ID
  |
  +-> AE: Write exposure/gain to sensor registers
  +-> AWB: Write white balance gains to CCM
  +-> Gamma: Refresh LUT in hardware (tiziano_gamma_lut_parameter)
  +-> MDNS: Update motion denoise parameters (if enabled)
  +-> Sharpen/BCSH: Write directly to ISP registers
  |
  v
Next frame uses updated parameters
```

### 15.3 Key Design Invariants

1. **VIC MDMA and MSCA are independent**: VIC captures raw frames internally,
   MSCA routes ISP-processed output to user buffers. Never confuse the two paths.

2. **VIC RUN and MDMA enable are separate**: Input engine (VIC RUN) and output
   engine (MDMA) are controlled independently and started sequentially.

3. **IRQ mask polarity is inverted**: Bit=1 means MASKED (no interrupt), opposite
   of typical convention. `Pending = (~mask) & status`.

4. **Subdevice array order matters**: `tx_isp_video_link_stream()` walks linearly
   from index 0 (VIC). Wrong order breaks streaming.

5. **CFA pattern must be correct**: Wrong Bayer phase from `mbus_to_bayer_write()`
   causes visible demosaic corruption.

6. **Clock enable order matters**: CGU parent before children, with stabilization
   delay. Wrong order causes silent hardware failures.

7. **One-shot bayer write**: `bayer_write_pending` flag ensures the CFA pattern
   register is written exactly once on the first ISP core interrupt.

8. **Global singleton**: `ourISPdev` is used throughout for simplified access.
   All subdevices can reach the main device structure without context passing.

---

## Appendix A: Key Header Files

| Header | Purpose |
|--------|---------|
| `tx_isp.h` | Main device structure, CSI device, global declarations |
| `tx-isp-device.h` | Subdev, pad, link, channel, module structures |
| `tx-isp-common.h` | Sensor attributes, MIPI config, event definitions, macros |
| `tx-libimp.h` | Frame node, tuning state, ioctl commands, V4L2 controls |
| `tx_isp_vic.h` | VIC device structure and buffer management |
| `tx_isp_vin.h` | VIN device structure |
| `tx_isp_core_device.h` | Core device, FS device, platform data structures |
| `tx_isp_core.h` | Sensor info blob layout |
| `tx_isp_regmap.h` | ADR/YDNS register offsets and macros |
| `tx_isp_fixpt.h` | Fixed-point math (Q16/Q24) for gain/exposure |
| `tx_isp_subdev_helpers.h` | Subdev access helpers, pad stride, find functions |

## Appendix B: Fixed-Point Math

ISP gain and exposure calculations use fixed-point arithmetic to avoid
floating-point in kernel space:

| Function | Purpose |
|----------|---------|
| `fix_point_mult2_32(q, a, b)` | Q-format 2-operand multiply: `(a * b) >> q` |
| `fix_point_mult3_32(q, a, b, c)` | Q-format 3-operand multiply |
| `fix_point_div_32(q, num, den)` | Q-format division: `(num << q) / den` |

Common formats:
- Q16 (16 fractional bits) for sensor gain parameters
- Q24 for color/gain calculations

## Appendix C: Related Documents

| Document | Purpose |
|----------|---------|
| `CLAUDE.md` | Development workflow and MCP tool usage |
| `docs/T31_ISP_ARCHITECTURE.md` | High-level architecture notes |
| `driver/REGMAP_ADR_YDNS.md` | ADR/YDNS register windows and OEM constraints |
| `driver/TX_ISP_VIDEO_S_STREAM_VERIFIED.md` | Stream start verification notes |
| `external/ingenic-sdk/3.10/isp/t31/OEM_TUNING_BLOB_MANIFEST.md` | OEM tuning blob status |
