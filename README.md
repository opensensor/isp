# Open-Source ISP Driver for Ingenic T31 (SC2336)

![Ingenic ISP Logo](./ingenic_isp.webp)

## Overview
This project provides an open-source driver for the Ingenic T31 System on Chip (SoC) with support for the SC2336 image sensor. The primary goal of this driver is to interface with Ingenic's `libimp` library, enabling users to capture and process images through the Image Signal Processor (ISP) on Ingenic devices.

The driver is designed to replace the proprietary driver provided by Ingenic, offering a fully open-source solution that can be maintained and extended by the community. It supports key functionality such as sensor initialization, ISP control, and video device handling.

## Features
- Support for Ingenic T31 SoC with SC2336 sensor.
- Integration with the `libimp` library to enable camera functionalities.
- Handles low-level hardware control, including I2C communication, DMA, and interrupts.
- Provides `/proc` entries for runtime status and configuration.
    - Modular and extensible design, allowing developers to add support for additional sensors and functionalities.

## Requirements
- **Linux Kernel**: 4.x (Tested on 4.9)
- **Target SoC**: Ingenic T31
- **Sensor**: SC2336
- **libimp**: The driver interfaces with Ingenic's `libimp` for ISP functionalities.

## Building the Driver

1. **Clone the Repository**
```bash
git clone https://github.com/your-repository/isp-driver.git

./setup_submodule_and_patch.sh
```

2. **Build the Driver**

These steps needs to be documented but involve cloning `ingenic-sdk` repository and making some minimal modifications.

3. **Install the Driver**
```bash
insmod isp_driver.ko
```

## Usage
Once the driver is loaded, the `/proc/jz/isp` directory will be available with the following entries:

- `/proc/jz/isp/status` - Displays the current status of the ISP.
- `/proc/jz/isp/sensor` - Provides information about the connected sensor.
- `/proc/jz/isp/control` - Allows users to adjust ISP settings.

## Integration with `libimp`
The driver is designed to work seamlessly with Ingenic's `libimp`. Ensure that `libimp` is properly installed on your device and modify your application code to interact with the ISP driver through the provided ioctl interfaces.

## Troubleshooting
- **Issue**: `/proc/jz/isp` not appearing.
- Ensure the driver is loaded correctly using:
```bash
dmesg | grep isp
```
- Check if the `libimp` library is properly installed.

- **Issue**: Unable to communicate with the sensor.
- Confirm I2C bus and addresses in the Device Tree are correctly configured.
- Verify that the SC2336 sensor is properly connected.

## Contributing
Contributions are welcome! Please follow the guidelines below:

1. Fork the repository.
2. Create a new branch:
```bash
git checkout -b feature-branch
```
3. Commit your changes:
```bash
git commit -am 'Add new feature'
```
4. Push to the branch:
```bash
git push origin feature-branch
```
5. Open a pull request.

## License
This project is licensed under the GNU General Public License (GPLv3).

## Acknowledgments
Special thanks to the open-source community and contributors who made this project possible.
Specifically: `thingino-firmware`  https://github.com/themactep/thingino-firmware
and `ingenic-sdk` https://github.com/themactep/ingenic-sdk

Without these projects, there would be no point to trying to reverse engineer the ISP driver.

## Limitations
This driver is incomplete but works to a certain degree and enables the streamer, but we only get green frames.

There is a crash after the first stream on ioctl cmd, that we are currently debugging.

Current progress:

```angular2html
Loading ISP driver...
ourISPdev allocated at 80b30000
ISP probe called
tx-isp tx-isp: Reserved memory initialized:
Physical address: 0x02a80000 (aligned from 0x02a80000)
Virtual address: a2a80000
Size: 22544384 bytes (0x01580000)
Alignment check: base=0x0 buf=0x0
Starting GPIO config init
tx-isp tx-isp: GPIO setup: reset=18 pwdn=-1
Starting hw resources init
Starting IRQ handler init
IRQ initialized: number=36 handler=c02ac89c
Starting memory init
tx-isp tx-isp: Memory initialized:
Physical: 0x02a80000
Virtual: a2a80000
Size: 22544384 bytes
Params: a2a81000
Starting clocks init
Configuring ISP system clocks
Clock configuration completed. Rates:
ISP Core: 250000000 Hz
CGU ISP: 125000000 Hz
CSI: 125000000 Hz
IPU: 250000000 Hz
Starting proc entries init
Starting subsystems init
Starting ISP subsystem initialization
Initializing AE/AWB subsystems
TODO Initializing image processing subsystems
ISP subsystem initialization complete
Creating frame channel devices...
Initialized frame source 0:
width=1920 height=1080
Initialized frame source 1:
width=1920 height=1080
Initialized frame source 2:
width=1920 height=1080
Initialized frame source 3:
width=1920 height=1080
Initialized frame source 4:
width=1920 height=1080
Initialized frame source 5:
width=1920 height=1080
Created device framechan0: major=251, minor=0
Created device framechan1: major=251, minor=1
Created device framechan2: major=251, minor=2
Created device framechan3: major=251, minor=3
Created device framechan4: major=251, minor=4
Created device framechan5: major=251, minor=5
Frame channel devices created successfully
Mapping I/O regions:
MIPI PHY: 0x10022000
ISP W01: 0x10023000
Initial CPM state:
CLKGR: 0x094f5f80
CLKGR1: 0x000033a2
Initial register readings:
W01 0x00: 0x3130322a
W01 0x04: 0x00000001
W01 0x08: 0x00000000
PHY 0x00: 0x00000000
PHY 0x04: 0x00000000
After test write:
W01 0x04: 0x00000001
PHY 0x04: 0x00000000
After test write:
W01 0x04: 0x00000001
PHY 0x04: 0x00000000
T31 CSI: Configuring for SC2336...
T31 CSI configured:
PHY state: 0x00000630
ERR1: 0x00000000
ERR2: 0x00000000
Starting I2C init
Setting up I2C infrastructure for SC2336...
I2C sensor initialized: addr=0x30 adapter=8223ac10
Initial sensor reset...
Resetting sensor via GPIO 18
Initializing VIC control...
Disabling VIC IRQs
VIC IRQ 36 registered with handler c02ac73c
VIC Configuration:
Control:     0x00000001
IRQ Enable:  0x00000000
Frame Ctrl:  0x00000000
Int Mask:    0x00000000
IRQ Status:  0x00000000
Status:      0x00000000
Starting sub device init
Creating ISP subdevices...
tx-isp tx-isp: ISP subdevices registered successfully
tx-isp tx-isp: TX-ISP probe completed successfully
CSI probe starting
Mapping I/O regions:
MIPI PHY: 0x10022000
ISP W01: 0x10023000
MIPI PHY region already claimed
ISP W01 region already claimed
Initial CPM state:
CLKGR: 0x094f5f80
CLKGR1: 0x000033a2
Initial register readings:
W01 0x00: 0x3130322a
W01 0x04: 0x00000001
W01 0x08: 0x00000001
PHY 0x00: 0x00000001
PHY 0x04: 0x000000e3
After test write:
W01 0x04: 0x00000001
PHY 0x04: 0x000000aa
After test write:
W01 0x04: 0x00000001
PHY 0x04: 0x000000aa
T31 CSI: Configuring for SC2336...
T31 CSI configured:
PHY state: 0x00000630
ERR1: 0x00000000
ERR2: 0x00000000
****>>>>> dump csi reg <<<<<****
VERSION = 3130322a
N_LANES = 00000001
PHY_SHUTDOWNZ = 00000001
DPHY_RSTZ = 00000001
CSI2_RESETN = 00000001
PHY_STATE = 00000630
DATA_IDS_1 = 0000002b
DATA_IDS_2 = 00000000
ERR1 = 00000000
ERR2 = 00000000
MASK1 = 000f0000
MASK2 = 00ff0000
PHY_TST_CTRL0 = 00000000
PHY_TST_CTRL1 = 00000001
CSI probe completed successfully
VIC probe called for device 80b30000
VIC probe complete: regs=b3307800
VIN probe called for device 80b30000
VIN probe complete
Core probe called
Initial ctrl reg value: 0x54560031
Read back value after enable: 0x54560031
Core probe complete: dev=80b30000 reg_base=b3300000
Frame source probe called
Initialized input pad 0: 80b3e900
Initialized input pad 1: 80b3e924
Initialized input pad 2: 80b3e948
Initialized input pad 3: 80b3e96c
Initialized input pad 4: 80b3e990
Initialized input pad 5: 80b3e9b4
Initialized output pad 0: 80b3ea00
Initialized output pad 1: 80b3ea24
Initialized output pad 2: 80b3ea48
Initialized output pad 3: 80b3ea6c
Initialized output pad 4: 80b3ea90
Initialized output pad 5: 80b3eab4
Frame source probe complete: dev=80b30000 sd=80b3e800 fs_pdev=80b3e500
Initialized with 6 input pads and 6 output pads
ISP Subsystem State:
Control: 0x54560031
Status:  0x00000000
Config:  0x00000001
ISP driver and submodules loaded successfully
Starting subdev init for sc2336
No additional clocks needed for subdev sc2336
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: sc2336 (in:1 out:1)
Subdev initialized successfully
ISP device open called from pid 2132
ISP opened: file=818cd918 fd=0
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x77b95ce0
file=820a05a0 flags=0x2002
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7fa38ca8
file=820a05a0 flags=0x2002
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7fa38ca8
file=820a05a0 flags=0x2002
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7fa38d00
file=820a05a0 flags=0x2002
Sensor command: 0xc0045627
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7fa38cf8
file=820a05a0 flags=0x2002
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7fa38cf8
file=820a05a0 flags=0x2002
tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO
Buffer info before update: method=0x33326373 phys=0x3633 size=0
Buffer info after update:
method=0x0
phys=0x2e80000
virt=a2e80000
size=18350080
flags=0x1
tx_isp: Buffer setup completed successfully
ISP IOCTL called: cmd=0x40045626

=== IOCTL Debug ===
cmd=0x40045626 arg=0x7fa38d10
file=820a05a0 flags=0x2002
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
Sensor info request: returning success (1)
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=820a05a0 flags=0x2002
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor streamon
Lane Enable write: wrote 0x3, readback: 0x00000000
CSI Final State:
PHY state: 0x00000630
N_LANES: 0x00000001
Lane Enable: 0x00000000
Data Format: 0x0000002b
ERR1: 0x00000000
ERR2: 0x00000000
Resetting sensor via GPIO 18
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7fa38d10
file=820a05a0 flags=0x2002
Starting link setup IOCTL
Setting up video link 0 (subdev=80b3e800)
num_inpads=6 num_outpads=6
Setting up link 1 of 2
Looking up source pad for tx-isp-csi (type=1 index=0)
Looking up pad: subdev=80b3e800 cfg=c02b9964
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Found pad but no channel data, reinitializing...
Created missing channel data for pad 0
Looking up sink pad for tx-isp-vic (type=0 index=0)
Looking up pad: subdev=80b3e800 cfg=80b69d38
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Link established: src=80b3ea00 sink=80b3ea00
Setting up link 2 of 2
Looking up source pad for tx-isp-vic (type=1 index=0)
Looking up pad: subdev=80b3e800 cfg=c02b9978
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up sink pad for tx-isp-ddr (type=0 index=0)
Looking up pad: subdev=80b3e800 cfg=80b69d38
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Link established: src=80b3ea00 sink=80b3ea00
Video link setup completed successfully
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=820a05a0 flags=0x2002
Enabling stream
VIC state after link op 0x800456d2:
Route: 0x00000000
Enable: 0x20151120
Mask: 0x00000000
ISP M0 device opened, tuning_data=822d4e00
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
GET operation
Get control: cmd=0x80000e0 value=-9
Get FPS
ISP m0 IOCTL called: cmd=0xc008561b
Direct GET/SET operation
ISP m0 IOCTL called: cmd=0xc008561b
Direct GET/SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
GET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP IOCTL called: cmd=0x800456d3

=== IOCTL Debug ===
cmd=0x800456d3 arg=0x0
file=820a05a0 flags=0x2002
Processing link disable IOCTL
Looking up pad: subdev=80b3e800 cfg=c02b9964
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=c02b9978
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
ISP IOCTL called: cmd=0x800456d1

=== IOCTL Debug ===
cmd=0x800456d1 arg=0x7fa38d10
file=820a05a0 flags=0x2002
Link destroy entry: arg=7fa38d10 dev=80b30000 state=0
Destroying link configuration 0 with 2 links
Looking up pad: subdev=80b3e800 cfg=c02b9964
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=80b69d38
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=c02b9978
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=80b69d38
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Link configuration destroyed successfully
Link destroy result: ret=0 new_state=-1
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc008561c
Direct GET/SET operation
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
SET operation
Channel 0 buffers initialized:
Virtual base: a2a80000
Physical base: 0x02a80000
Buffer size: 3112960 bytes
Stride: 1920 bytes
Total size: 12451840 bytes
Number of buffers: 4
Looking up pad: subdev=80b3e800 cfg=c02b9964
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=81635ce0
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Linking pads for channel 0: src=80b3ea00 sink=80b3ea00
Found CSI->VIC link for channel 0
Looking up pad: subdev=80b3e800 cfg=c02b9978
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=81635ce0
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Linking pads for channel 0: src=80b3ea00 sink=80b3ea00
Channel 0 opened and configured successfully
VIDIOC_S_FMT: arg=0x75cae828
Format entry
Size calculation for channel 0: base=3110400 metadata=23040 total=3133440
Updated format for channel 0:
Width: 1920 (aligned: 1920)
Height: 1080
Final size: 3133440
Stride: 1920
Allocated 1 buffers for channel 0
Framechan Streamon command: 0x80045612
Streaming enabled for channel 0 (res: 1920x1080 state=4)
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
GET operation
Channel 1 buffers initialized:
Virtual base: a2bd4000
Physical base: 0x02bd4000
Buffer size: 348160 bytes
Stride: 640 bytes
Total size: 1392640 bytes
Number of buffers: 4
Looking up pad: subdev=80b3e800 cfg=c02b9928
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=81697ce0
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Linking pads for channel 1: src=80b3ea00 sink=80b3ea00
Found CSI->VIC link for channel 1
Looking up pad: subdev=80b3e800 cfg=c02b993c
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Looking up pad: subdev=80b3e800 cfg=81697ce0
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Linking pads for channel 1: src=80b3ea00 sink=80b3ea00
Looking up pad: subdev=80b3e800 cfg=c02b9950
Pad lookup for tx-isp-vic:
type=1 index=1
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Found pad but no channel data, reinitializing...
Created missing channel data for pad 1
Looking up pad: subdev=80b3e800 cfg=81697ce0
Pad lookup for tx-isp-ddr:
type=0 index=1
inpads=80b3e900 (count=6)
outpads=80b3ea00 (count=6)
Linking pads for channel 1: src=80b3ea24 sink=80b3ea24
Channel 1 opened and configured successfully
VIDIOC_S_FMT: arg=0x75b9d828
Format entry
Size calculation for channel 1: base=345600 metadata=7680 total=353280
Updated format for channel 1:
Width: 640 (aligned: 640)
Height: 360
Final size: 353280
Stride: 640
Allocated 1 buffers for channel 1
Framechan Streamon command: 0x80045612
Streaming enabled for channel 1 (res: 640x360 state=4)
```

And on the streamer side:
```
root@ing-cinnado-d1-98fc ~# prudynt
[INFO:Config.cpp]: Loaded configuration from /etc/prudynt.cfg
[INFO:main.cpp]: PRUDYNT-T Next-Gen Video Daemon: Nov  1 2024 01:17:03_c9dad9ce
[DEBUG:Logger.cpp]: Logger Init.
[INFO:main.cpp]: Starting Prudynt Video Server.
[DEBUG:IMPSystem.cpp]: IMPSystem::init()
[DEBUG:IMPSystem.cpp]: IMP_OSD_SetPoolSize(1048576)
[INFO:IMPSystem.cpp]: LIBIMP Version IMP-1.1.6
[INFO:IMPSystem.cpp]: SYSUTILS Version: SYSUTILS-1.1.6
[INFO:IMPSystem.cpp]: CPU Information: T31-L
[DEBUG:IMPSystem.cpp]: IMP_ISP_Open() = 0
[INFO:IMPSystem.cpp]: Sensor: sc2336
[DEBUG:IMPSystem.cpp]: IMP_ISP_AddSensor(&sinfo) = 0
[DEBUG:IMPSystem.cpp]: IMP_ISP_EnableSensor() = 0
---- FPGA board is ready ----
  Board UID : 30AB6E51
  Board HW ID : 72000460
  Board rev.  : 5DE5A975
  Board date  : 20190326
-----------------------------
[DEBUG:IMPSystem.cpp]: IMP_System_Init() = 0
[DEBUG:IMPSystem.cpp]: IMP_ISP_EnableTuning() = 0
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetContrast(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetSharpness(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetSaturation(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetBrightness(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetContrast(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetSharpness(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetSaturation(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetBrightness(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSinterStrength(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetTemperStrength(128)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPHflip(0)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPVflip(0)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPRunningMode(0)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPBypass(1)
[ERROR:IMPSystem.cpp]: IMP_ISP_Tuning_SetAntiFlickerAttr(2)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetAeComp(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetMaxAgain(160)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetMaxDgain(80)
[DEBUG:IMPSystem.cpp]: Set white balance. Mode: 0, rgain: 0, bgain: 0
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetBcshHue(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetDefog_Strength(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetDPC_Strength(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetDRC_Strength(128)
[DEBUG:IMPSystem.cpp]: ISP Tuning Defaults set
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSensorFPS(25, 1) = 0
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPRunningMode(0) = 0
[DEBUG:main.cpp]: create video[0] thread
[DEBUG:worker.cpp]: Start stream_grabber thread for stream 0
[DEBUG:IMPFramesource.cpp]: IMPFramesource::init()
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnRotate(0, rotation, rot_height, rot_width)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_CreateChn(0, &chnAttr)
[DEBUG:WS.cpp]: WS TOKEN::IpxPUTstwWEIVzF2cFnYtQcmTzBKCm9e
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnAttr(0, &chnAttr)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_GetChnFifoAttr(0, &fifo)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnFifoAttr(0, &fifo)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetFrameDepth(0, 0)
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(0, 0)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE ch0, fps:25, bps:3000, gop:20, profile:2, mode:0, 1920x1080
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_SetbufshareChn(2, 0) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(0, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(0, 0) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateGroup(0) = 0
[DEBUG:OSD.cpp]: OSD init for begin
[DEBUG:OSD.cpp]: IMP_OSD_SetPoolSize(1048576)
[DEBUG:OSD.cpp]: IMP_Encoder_GetChnAttr read. Stream resolution: 1920x1080
[DEBUG:OSD.cpp]: OSD::libschrift_init()
[INFO:WS.cpp]: Server started on port 8089
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&fs, &osd_cell) = 0
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&osd_cell, &enc) = 0
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_EnableChn(0) = 0
[DEBUG:main.cpp]: create video[1] thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(0)
[DEBUG:worker.cpp]: Start stream_grabber thread for stream 1
[DEBUG:IMPFramesource.cpp]: IMPFramesource::init()
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnRotate(0, rotation, rot_height, rot_width)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_CreateChn(1, &chnAttr)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnAttr(1, &chnAttr)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_GetChnFifoAttr(1, &fifo)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnFifoAttr(1, &fifo)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetFrameDepth(1, 0)
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(1, 1)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE ch1, fps:25, bps:1000, gop:20, profile:2, mode:8, 640x360
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(1, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(1, 1) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateGroup(1) = 0
[DEBUG:OSD.cpp]: OSD init for begin
[DEBUG:OSD.cpp]: IMP_OSD_SetPoolSize(1048576)
[DEBUG:OSD.cpp]: IMP_Encoder_GetChnAttr read. Stream resolution: 640x360
[DEBUG:OSD.cpp]: OSD::libschrift_init()
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&fs, &osd_cell) = 0
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&osd_cell, &enc) = 0
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_EnableChn(1) = 0
[DEBUG:main.cpp]: create jpeg thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(1)
[DEBUG:worker.cpp]: Start jpeg_grabber thread.
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(2, 0)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE 2, 0, JPEG, 24fps, profile:0, 1920x1080
set jpeg streamMngCtx suceess
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(2, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(0, 2) = 0
[DEBUG:main.cpp]: create osd thread
[DEBUG:main.cpp]: create audio thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(2)
[DEBUG:worker.cpp]: Start audio_grabber thread for device 1 and channel 0 and encoder 0
[DEBUG:IMPAudio.cpp]: IMPAudio::init()
[DEBUG:IMPAudio.cpp]: IMP_AENC_RegisterEncoder(&handle, &enc)
[DEBUG:worker.cpp]: start osd update thread.
[DEBUG:OSD.cpp]: IMP_OSD_Start(0)
[DEBUG:OSD.cpp]: IMP_OSD_SetPoolSize(1048576)
[INFO:Opus.cpp]: Encoder bitrate: 40000
[DEBUG:IMPAudio.cpp]: IMP_AENC_CreateChn(0, &encattr)
[DEBUG:IMPAudio.cpp]: IMP_AI_SetPubAttr(1)
[DEBUG:IMPAudio.cpp]: IMP_AI_GetPubAttr(1)
[DEBUG:IMPAudio.cpp]: IMP_AI_Enable(1)
[DEBUG:IMPAudio.cpp]: IMP_AI_SetChnParam(1, 0)
[DEBUG:IMPAudio.cpp]: IMP_AI_GetChnParam(1, 0)
warn: shm_init,53shm init already
[DEBUG:IMPAudio.cpp]: IMP_AI_EnableChn(1, 0)
[DEBUG:IMPAudio.cpp]: IMP_AI_SetVol(1, 0, 80)
[DEBUG:IMPAudio.cpp]: IMP_AI_GetVol(1, 0, &vol)
[DEBUG:IMPAudio.cpp]: IMP_AI_SetGain(1, 0, 25)
[DEBUG:IMPAudio.cpp]: IMP_AI_GetGain(1, 0, &gain)
[INFO:IMPAudio.cpp]: Audio In: format:OPUS, vol:80, gain:25, samplerate:16000, bitwidth:16, soundmode:1, frmNum:30, numPerFrm:640, chnCnt:1, usrFrmDepth:30
[DEBUG:IMPAudio.cpp]: IMP_AI_SetAlcGain(1, 0, 0)
[DEBUG:main.cpp]: create rtsp thread
[DEBUG:RTSP.cpp]: identify stream 0
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:0
[DEBUG:main.cpp]: main thread is going to sleep
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:0
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: Audio stream 0 added to session
[INFO:RTSP.cpp]: stream 0 available at: rtsp://192.168.50.101/ch0
[DEBUG:RTSP.cpp]: identify stream 1
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: Audio stream 1 added to session
[INFO:RTSP.cpp]: stream 1 available at: rtsp://192.168.50.101/ch1
```
And Logcat:
```
root@ing-cinnado-d1-98fc ~# logcat 
I/ao( 1857): AO Enable: 0
I/ao( 1857): AO Ch Enable: 0:0
I/ao( 1857): EXIT AO Ch Enable: 0:0
I/ao( 1857): AO Set Vol: 60
I/ao( 1857): AO Get Gain: 20
I/OSD( 2408): IMP_OSD_SetPoolSize:1048576
D/IMP-ISP( 2408): ~~~~~~ IMP_ISP_Open[331] ~~~~~~~
I/IMP-ISP( 2408): IMP_ISP_AddSensor,480: paddr = 0x1, size = 0x1
I/Alloc Manager( 2408): MEM Alloc Method is kmalloc
D/KMEM Method( 2408): CMD Line Rmem Size:22544384, Addr:0x02a80000
D/KMEM Method( 2408): alloc->mem_alloc.method = kmalloc
 			alloc->mem_alloc.vaddr = 0x7537a000
 			alloc->mem_alloc.paddr = 0x02a80000
 			alloc->mem_alloc.length = 22544384
I/Alloc Manager( 2408): MEM Manager Method is continuous
D/System( 2408): IMP_System_Init SDK Version:1.1.6-a6394f42-Mon Dec 5 14:39:51 2022 +0800, built: Dec 29 2022 15:38:51
D/System( 2408): system_init()
D/System( 2408): Calling DSystem
D/System( 2408): Calling FrameSource
D/System( 2408): [ignored]read /proc/cpuinfo ret is NULLD/System( 2408): Calling IVS
D/System( 2408): Calling OSD
D/System( 2408): Calling Encoder
D/System( 2408): Calling FB
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetContrast_internal(1461), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetSharpness_internal(1538), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetSaturation(1602), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetBrightness(1393), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetContrast_internal(1461), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetSharpness_internal(1538), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetSaturation(1602), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetBrightness(1393), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetISPHflip(1653), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): IMP_ISP_Tuning_SetISPVflip(1705), set VIDIOC_S_CTRL failed
E/IMP-ISP( 2408): VIDIOC_S_CTRL error !
E/Framesource( 2408): IMP_FrameSource_GetChnAttr(): chnAttr was not set yet
D/Encoder( 2408): IMP_Encoder_SetbufshareChn: encChn:2, shareChn:0
E/MemPool( 2408): IMP_Encoder_GetPool(64):chnNum: 0 not bind pool
I/Encoder( 2408): encChn=0,srcFrameCnt=3,srcFrameSize=3133440
I/Encoder( 2408): encChn=0,srcStreamCnt=2,enc_chn->stream_frame_size=949248
I/OSD( 2408): IMP_OSD_SetPoolSize:1048576
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=0 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=1 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=2 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=3 success
D/System( 2408): system_bind(): bind DST-OSD-0(4.0.0) to SRC-Framesource-0(0.0.0)
D/System( 2408): system_bind(): bind DST-Encoder-0(1.0.0) to SRC-OSD-0(4.0.0)
I/Framesource( 2408): [chn0]: width = 1920 height = 1080
I/VBM( 2408): VBMCreatePool()-0: w=1920 h=1080 f=842094158 nrVBs=1
I/VBM( 2408): VBMCreatePool()-0: pool->config.fmt.fmt.pix.sizeimage=0 sizeimage=3133440
E/Framesource( 2408): IMP_FrameSource_GetPool(3294):chnNum: 0 not bind pool
E/VBM( 2408): VBMCreatePool()-0: sizeimage=3133440
I/VBM( 2408): PoolId:0, frame=0x7513e250, frame->priv=0x7513e27c, frame[0].virAddr=75cdb200, frame[0].phyAddr=33e1200
E/Framesource( 2408): IMP_FrameSource_GetChnAttr(): chnAttr was not set yet
I/Encoder( 2408): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 2408): enc_chn->index=0, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=20, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=2, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 2408): enc_chn->index=0, enc_chn->chnFpsMask=1, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=25, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=25, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 2408): do_day_night_change(664):enc_chn->inStat.is_day=1
E/MemPool( 2408): IMP_Encoder_GetPool(64):chnNum: 1 not bind pool
I/Encoder( 2408): encChn=1,srcFrameCnt=3,srcFrameSize=353280
I/Encoder( 2408): encChn=1,srcStreamCnt=2,enc_chn->stream_frame_size=199808
I/OSD( 2408): IMP_OSD_SetPoolSize:1048576
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=4 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=5 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=6 success
I/OSD( 2408): IMP_OSD_CreateRgn(1227) create handle=7 success
D/System( 2408): system_bind(): bind DST-OSD-1(4.1.0) to SRC-Framesource-1(0.1.0)
D/System( 2408): system_bind(): bind DST-Encoder-1(1.1.0) to SRC-OSD-1(4.1.0)
I/Framesource( 2408): [chn1]: width = 640 height = 360
I/VBM( 2408): VBMCreatePool()-1: w=640 h=360 f=842094158 nrVBs=1
I/VBM( 2408): VBMCreatePool()-1: pool->config.fmt.fmt.pix.sizeimage=0 sizeimage=353280
E/Framesource( 2408): IMP_FrameSource_GetPool(3294):chnNum: 1 not bind pool
E/VBM( 2408): VBMCreatePool()-1: sizeimage=353280
I/VBM( 2408): PoolId:1, frame=0x74ffb1c0, frame->priv=0x74ffb1ec, frame[0].virAddr=76112300, frame[0].phyAddr=3818300
I/Encoder( 2408): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 2408): enc_chn->index=1, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=20, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=2, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 2408): enc_chn->index=1, enc_chn->chnFpsMask=1, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=25, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=25, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 2408): do_day_night_change(664):enc_chn->inStat.is_day=1
E/MemPool( 2408): IMP_Encoder_GetPool(64):chnNum: 2 not bind pool
I/Encoder( 2408): encChn=2,srcFrameCnt=2,srcFrameSize=3133440
I/Encoder( 2408): encChn=2,srcStreamCnt=0,enc_chn->stream_frame_size=0
I/Encoder( 2408): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 2408): enc_chn->index=2, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=0, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=1, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 2408): enc_chn->index=2, enc_chn->chnFpsMask=1fff7ff, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=24, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=24, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 2408): do_day_night_change(664):enc_chn->inStat.is_day=1
E/aenc( 2408): ai_buf size error 0
I/OSD( 2408): IMP_OSD_SetPoolSize:1048576
I/ai( 2408): AI Enable: 1
E/IMP-ISP( 2408): err:video drop 
I/ai( 2408): AI Enable Chn: 1-0
I/ai( 2408): EXIT AI Enable Chn: 1-0
I/ai( 2408): AI Set Vol: 80
I/ai( 2408): AI Get Vol: 80
I/ai( 2408): AI Set Gain: 25
I/ai( 2408): AI Get Gain: 25
I/ai( 2408): AI Set Pga Gain: 0
```
