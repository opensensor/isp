# Open-Source ISP Driver for Ingenic T31

![Ingenic ISP Logo](./ingenic_isp.webp)

## Overview
This project provides an open-source driver for the Ingenic T31 System on Chip (SoC) that supports any T31 device equipped with a compatible image sensor (excluding the Zeratul series). Designed as a generic solution, the driver leverages Ingenic’s `libimp` library and the sensor source code from the [ingenic-sdk](https://github.com/themactep/ingenic-sdk) to enable image capture and processing through the ISP. By replacing the proprietary driver, this project empowers developers to customize, extend, and optimize the camera subsystem on a wide range of Ingenic T31 devices.

**Use rewrite branch for latest development focus!**

## Features
- **Generic Sensor Support:** Works with any T31 device sensor (non-Zeratul) by utilizing a modular sensor interface.
- **Integration with libimp:** Seamlessly integrates with Ingenic’s `libimp` library for advanced ISP functionalities.
- **Low-Level Hardware Control:** Manages I2C communication, DMA, interrupts, and other hardware-level operations.
- **Runtime Status and Configuration:** Exposes `/proc` entries for monitoring ISP status and sensor parameters.
- **Extensibility:** Designed to easily incorporate support for additional sensors and features by adapting the sensor source provided by `ingenic-sdk`.

## Requirements
- **Linux Kernel:** 3.10 (Some minimal testing on 4.4 but focus is 3.10 for now)
- **Target SoC:** Ingenic T31
- **Sensor:** Any supported sensor (non-Zeratul) whose source is provided by [ingenic-sdk](https://github.com/themactep/ingenic-sdk)
- **libimp:** The driver interfaces with Ingenic’s `libimp` for ISP functionalities

## Building the Driver

1. **Clone the Repository**
    ```bash
    git clone https://github.com/your-repository/isp-driver.git
    cd isp-driver
    ./setup_submodule_and_patch.sh
    ```

2. **Prepare Sensor Source**
  - Clone the [ingenic-sdk](https://github.com/themactep/ingenic-sdk) repository if it is not included as a submodule.
  - Make any minimal modifications needed to adapt the sensor source for your specific sensor. The modular design allows for adjustments to support different sensors without significant changes to the core driver.

3. **Build the Driver**
  - Follow your standard kernel module compilation process. Ensure that paths to the `ingenic-sdk` components are correctly set in your build configuration.
  - Example:
    ```bash
    make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
    ```

4. **Install the Driver**
    ```bash
    sudo insmod isp_driver.ko
    ```

## Usage
Once the driver is loaded, the `/proc/jz/isp` directory will be available with several entries:

- **`/proc/jz/isp/status`** – Displays the current status of the ISP.
- **`/proc/jz/isp/sensor`** – Provides details about the connected sensor.
- **`/proc/jz/isp/control`** – Allows users to adjust ISP settings dynamically.

Applications can interact with the driver via standard ioctl interfaces to configure the ISP and capture video streams.

## Integration with libimp
The driver is built to work in tandem with Ingenic’s `libimp` library. Make sure that `libimp` is correctly installed on your device. Update your application code to use the provided ioctl interfaces for controlling sensor parameters, initiating streaming, and processing captured images.

## Troubleshooting
- **Missing `/proc/jz/isp` Directory**
  - Verify the driver is loaded:
    ```bash
    dmesg | grep isp
    ```
  - Confirm that the `libimp` library and sensor source are properly integrated.

- **Sensor Communication Issues**
  - Check that the I2C bus and sensor addresses in the Device Tree or board configuration are correct.
  - Ensure that the sensor wiring is properly connected and that any required power sequences are followed.

## Contributing
Contributions to enhance sensor support, improve stability, or add new features are welcome. To contribute:
1. Fork the repository.
2. Create a new branch:
    ```bash
    git checkout -b feature-branch
    ```
3. Commit your changes:
    ```bash
    git commit -am 'Add new feature or fix'
    ```
4. Push your branch:
    ```bash
    git push origin feature-branch
    ```
5. Open a pull request describing your changes.

## License
This project is licensed under the GNU General Public License (GPLv3).

## Acknowledgments
Special thanks to the open-source community and the contributors of:
- [thingino-firmware](https://github.com/themactep/thingino-firmware)
- [ingenic-sdk](https://github.com/themactep/ingenic-sdk)

Their work has been instrumental in reverse-engineering and developing this open-source ISP driver.

## Limitations and Current Progress
- **Image Output:** The driver currently enables streaming; however, the output displays only green frames.
- **Stability Issues:** A crash occurs after the first stream when an ioctl command is issued. Debugging is in progress.
- **Future Work:** Ongoing efforts are focused on stabilizing the driver and extending sensor compatibility further.

---
*This driver is a work in progress. Community feedback and contributions are essential to improve functionality and stability.*

```angular2html
Loading ISP driver...
ourISPdev allocated at 80cc0000
ISP probe called
ISP M0 device opened, tuning_data=81f60a00
Starting GPIO config init
tx-isp tx-isp: GPIO setup complete: reset=18 pwdn=-1
tx-isp tx-isp: ISP Memory Initialization Complete:
ISP Registers: phys=0x13300000 virt=b3300000
CSI Registers: phys=0x10022000 virt=b0022000
DMA Buffer: phys=0x02a80000 virt=a2a80000 size=12451840 (for 4 frames)
Starting hw resources init
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
CLKGR: 0x094f5780
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
Starting sub device init
Creating ISP subdevices...
tx-isp tx-isp: ISP subdevices registered successfully
Starting ISP IRQ init
tx-isp tx-isp: Got ISP IRQ: 37
tx-isp tx-isp: TX-ISP probe completed successfully
CSI probe starting
Starting subdev init for tx-isp-csi
Initializing clocks for tx-isp-csi
CSI clock initialized: rate=125000000 Hz
Checking clocks for tx-isp-csi:
No clocks configured or invalid clock count: 0
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b0022000
Module initialized: tx-isp-csi (in:1 out:1)
Subdev tx-isp-csi initialized successfully
CSI probe completed successfully
VIC probe called
Starting subdev init for tx-isp-vic
Initializing clocks for tx-isp-vic
Trying clock avpu for VIC
Successfully got clock avpu with rate 250000000 Hz
Checking clocks for tx-isp-vic:
No clocks configured or invalid clock count: 0
Created channel data for input pad 0
Created channel 0 for pad 0
Created channel 1 for pad 1
Using register base: b3307000
Module initialized: tx-isp-vic (in:1 out:2)
Subdev tx-isp-vic initialized successfully
tx-isp-vic tx-isp-vic.0: VIC IRQ 38 registered successfully
VIC probe completed successfully
VIN probe called
Starting subdev init for tx-isp-vin
Initializing clocks for tx-isp-vin
Checking clocks for tx-isp-vin:
Number of clocks: 1
Clock 0: valid, rate=250000000 Hz
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: tx-isp-vin (in:1 out:1)
Subdev tx-isp-vin initialized successfully
VIN probe completed successfully
Core probe called
Starting subdev init for tx-isp-core
Initializing clocks for tx-isp-core
Checking clocks for tx-isp-core:
Number of clocks: 1
Clock 0: valid, rate=250000000 Hz
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: tx-isp-core (in:1 out:1)
Subdev tx-isp-core initialized successfully
Initial ctrl reg value: 0x54560031
Read back value after enable: 0x54560031
Core probe complete: dev=80cc0000 reg_base=b3300000
Frame source probe called
Starting subdev init for tx-isp-fs
Initializing clocks for tx-isp-fs
Checking clocks for tx-isp-fs:
Number of clocks: 1
Clock 0: valid, rate=250000000 Hz
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: tx-isp-fs (in:1 out:1)
Subdev tx-isp-fs initialized successfully
Frame source probe completed successfully
ISP Subsystem State:
Control: 0x54560031
Status:  0x00000000
Config:  0x00000001
ISP driver and submodules loaded successfully
Starting subdev init for sc2336
Initializing clocks for sc2336
Sensor clock initialized: rate=24000000 Hz
Checking clocks for sc2336:
Number of clocks: 1
Clock 0: valid, rate=24000000 Hz
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Unknown subdev name: sc2336
Module initialized: sc2336 (in:1 out:1)
Subdev sc2336 initialized successfully
ISP device open called from pid 2162
ISP opened: file=80cfe020 fd=0
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x778c4ce0
file=80cb6e60 flags=0x2002
Detected sensor: sc2336 (ID: 0xcb3a) 1920x1080
Sensor registered: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7fcc0c38
file=80cb6e60 flags=0x2002
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7fcc0c38
file=80cb6e60 flags=0x2002
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7fcc0c90
file=80cb6e60 flags=0x2002
Sensor command: 0xc0045627
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7fcc0c88
file=80cb6e60 flags=0x2002
Handling TX_ISP_SET_BUF
Copying buffer info from user address 7fcc0c88
Buffer info: method=0x203a726f phys=0x348fbaa533326373 size=32
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7fcc0c88
file=80cb6e60 flags=0x2002
Handling tx_isp_set_buf additional setup (0x800856d4)
ISP DMA registers configured
ISP IOCTL called: cmd=0x40045626

=== IOCTL Debug ===
cmd=0x40045626 arg=0x7fcc0ca0
file=80cb6e60 flags=0x2002
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
Sensor info request: returning success (1)
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=80cb6e60 flags=0x2002
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor streamon
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7fcc0ca0
file=80cb6e60 flags=0x2002
Starting link setup IOCTL
Setting up video link 0
Found devices and pads:
subdev=80cbeb10
num_inpads=1
num_outpads=1
Setting up link 1 of 2
Looking up pad: subdev=80cbeb10 cfg=c02eb168
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80ce8400 (count=1)
outpads=80ce8480 (count=1)
Found source pad 80ce8480
Looking up pad: subdev=80cbeb10 cfg=80cfbd48
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80ce8400 (count=1)
outpads=80ce8480 (count=1)
Found sink pad 80ce8480
Setting up link 2 of 2
Looking up pad: subdev=80cbeb10 cfg=c02eb17c
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80ce8400 (count=1)
outpads=80ce8480 (count=1)
Found source pad 80ce8480
Looking up pad: subdev=80cbeb10 cfg=80cfbd48
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80ce8400 (count=1)
outpads=80ce8480 (count=1)
Found sink pad 80ce8480
Link setup completed successfully
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=80cb6e60 flags=0x2002
Handling ISP stream enable request
ISP M0 device opened, tuning_data=81f60a00
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
GET operation
Get control: cmd=0x80000e0 value=2006024192
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
file=80cb6e60 flags=0x2002
TODO Disabling ISP links
ISP IOCTL called: cmd=0x800456d1

=== IOCTL Debug ===
cmd=0x800456d1 arg=0x7fcc0ca0
file=80cb6e60 flags=0x2002
TODO Destroying ISP links
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
Setting up control block at ISP base + 0x9888 = 13309888
Framechan0 opened successfully
VIDIOC_S_FMT: arg=0x759dd828
Format entry
Size calculation for channel 0: base=3110400 metadata=23040 total=3133440
Updated format for channel 0:
Width: 1920 (aligned: 1920)
Height: 1080
Final size: 3133440
Stride: 1920
Allocated buffer 0 meta=80cbfc80 group=81e99400
Registered event handler for channel 0
No event handler for pad 80cc01d4 cmd 0x1000
Ch0 QBUF: buffer 0 queued (total=1)
Framechan Streamon command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor streamon
Frame thread starting for channel 0
Frame thread initialized for channel 0 with VBM pool 7777ef30 (mapped c0300f30)
ISP State:
Control: 0x54560031
Status: 0x00000000
Int mask: 0xffffffff
VIC State:
0x7000: 0x00060003
0x7004: 0x00020000
0x7008: 0x00c80000
0x700c: 0x1eff0000
0x7010: 0x0000c83f
0x7014: 0x00001e0a
0x7018: 0x080f003f
0x701c: 0x141f0000
0x7020: 0x003fff08
0x7024: 0x00133200
0x7028: 0x00010610
0x702c: 0xff00ff00
0x7030: 0x0003ff00
0x7048: 0x20202020
0x704c: 0x20202020
0x7050: 0x20202020
0x7054: 0x20202020
0x7058: 0x20202020
0x705c: 0x20202020
0x7060: 0x20202020
0x7064: 0x20202020
0x7068: 0x00380038
0x706c: 0x00380038
0x7070: 0x00380038
0x7074: 0x00380038
0x7080: 0x01000000
0x7088: 0x00080000
0x708c: 0x00010001
0x7810: 0x00010001
0x7820: 0x00000001
0x7828: 0x00000001
0x7830: 0x00000001
0x7834: 0x00000020
0x783c: 0x00000001
0x793c: 0x000033fb
0x7b00: 0x04000010
Enabled streaming on channel 0
Ch0 DQBUF start: ready=0 done=1 queued=0
Ch0 DQBUF: returned buffer 0 seq=0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning IOCTL
GET operation
Setting up control block at ISP base + 0x9888 = 13309888
Framechan1 opened successfully
VIDIOC_S_FMT: arg=0x758cc828
Format entry
Size calculation for channel 1: base=345600 metadata=7680 total=353280
Updated format for channel 1:
Width: 640 (aligned: 640)
Height: 360
Final size: 353280
Stride: 640
Allocated buffer 0 meta=80db0400 group=81e98800
Registered event handler for channel 1
No event handler for pad 80cc0ed4 cmd 0x1000
Ch1 QBUF: buffer 0 queued (total=1)
Framechan Streamon command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor streamon
Frame thread starting for channel 1
Frame thread initialized for channel 1 with VBM pool 7777ef30 (mapped c0320f30)
Enabled streaming on channel 1
Ch1 DQBUF start: ready=0 done=1 queued=0
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-1
Ch1 QBUF: buffer 0 queued (total=0)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-2
Ch1 QBUF: buffer 0 queued (total=-1)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-3
Ch1 QBUF: buffer 0 queued (total=-2)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-4
Ch1 QBUF: buffer 0 queued (total=-3)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-5
Ch1 QBUF: buffer 0 queued (total=-4)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-6
Ch1 QBUF: buffer 0 queued (total=-5)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-7
Ch1 QBUF: buffer 0 queued (total=-6)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-8
Ch1 QBUF: buffer 0 queued (total=-7)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-9
Ch1 QBUF: buffer 0 queued (total=-8)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-10
Ch1 QBUF: buffer 0 queued (total=-9)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 DQBUF start: ready=0 done=0 queued=-11
Ch1 QBUF: buffer 0 queued (total=-10)
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-11)
Ch1 DQBUF start: ready=1 done=0 queued=-11
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-12)
Ch1 DQBUF start: ready=1 done=0 queued=-12
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-13)
Ch1 DQBUF start: ready=1 done=0 queued=-13
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-14)
Ch1 DQBUF start: ready=1 done=0 queued=-14
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-15)
Ch1 DQBUF start: ready=1 done=0 queued=-15
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-16)
Ch1 DQBUF start: ready=1 done=0 queued=-16
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-17)
Ch1 DQBUF start: ready=1 done=0 queued=-17
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-18)
Ch1 DQBUF start: ready=1 done=0 queued=-18
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-19)
Ch1 DQBUF start: ready=1 done=0 queued=-19
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-20)
Ch1 DQBUF start: ready=1 done=0 queued=-20
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-21)
Ch1 DQBUF start: ready=1 done=0 queued=-21
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-22)
Ch1 DQBUF start: ready=1 done=0 queued=-22
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-23)
Ch1 DQBUF start: ready=1 done=0 queued=-23
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-24)
Ch1 DQBUF start: ready=1 done=0 queued=-24
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-25)
Ch1 DQBUF start: ready=1 done=0 queued=-25
Ch1 DQBUF: returned buffer 0 seq=0
Ch1 QBUF: buffer 0 queued (total=-26)
Ch1 DQBUF start: ready=1 done=0 queued=-26
Ch1 DQBUF: returned buffer 0 seq=0
...
Note: latest build chn0 is getting stuck now
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
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetContrast(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSharpness(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSaturation(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetBrightness(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetContrast(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSharpness(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSaturation(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetBrightness(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetSinterStrength(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetTemperStrength(128)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPHflip(0)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPVflip(0)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPRunningMode(0)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetISPBypass(1)
[DEBUG:IMPSystem.cpp]: IMP_ISP_Tuning_SetAntiFlickerAttr(2)
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
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnAttr(0, &chnAttr)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_GetChnFifoAttr(0, &fifo)
[DEBUG:WS.cpp]: WS TOKEN::2sng4GjjjfbcYBYOU2oRFOayBrt1kp5P
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetChnFifoAttr(0, &fifo)
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_SetFrameDepth(0, 0)
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(0, 0)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE ch0, fps:25, bps:3000, gop:20, profile:2, mode:0, 1920x1080
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_SetbufshareChn(2, 0) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(0, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(0, 0) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateGroup(0) = 0
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&fs, &enc) = 0
[INFO:WS.cpp]: Server started on port 8089
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
[DEBUG:IMPEncoder.cpp]: IMP_System_Bind(&fs, &enc) = 0
[DEBUG:IMPFramesource.cpp]: IMP_FrameSource_EnableChn(1) = 0
[DEBUG:main.cpp]: create jpeg thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(1)
[DEBUG:worker.cpp]: Start jpeg_grabber thread.
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(2, 0)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE 2, 0, JPEG, 24fps, profile:0, 1920x1080
set jpeg streamMngCtx suceess
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(2, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(0, 2) = 0
[DEBUG:main.cpp]: create rtsp thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(2)
[DEBUG:RTSP.cpp]: identify stream 0
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:0
[DEBUG:main.cpp]: main thread is going to sleep
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:0
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: stream 0 available at: rtsp://192.168.50.101/ch0
[DEBUG:RTSP.cpp]: identify stream 1
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: stream 1 available at: rtsp://192.168.50.101/ch1
```
And Logcat:
```
root@ing-cinnado-d1-98fc ~# logcat 
[DEBUG:main.cpp]: create jpeg thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(1)
[DEBUG:worker.cpp]: Start jpeg_grabber thread.
[DEBUG:IMPEncoder.cpp]: IMPEncoder::init(2, 0)
[DEBUG:IMPEncoder.cpp]: STREAM PROFILE 2, 0, JPEG, 24fps, profile:0, 1920x1080
set jpeg streamMngCtx suceess
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_CreateChn(2, chnAttr) = 0
[DEBUG:IMPEncoder.cpp]: IMP_Encoder_RegisterChn(0, 2) = 0
[DEBUG:main.cpp]: create rtsp thread
[DEBUG:worker.cpp]: IMP_Encoder_StartRecvPic(2)
[DEBUG:RTSP.cpp]: identify stream 0
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:0
[DEBUG:main.cpp]: main thread is going to sleep
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:0
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: stream 0 available at: rtsp://192.168.50.101/ch0
[DEBUG:RTSP.cpp]: identify stream 1
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps constructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got SPS (H264)
[DEBUG:RTSP.cpp]: Got PPS (H264)
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video/pps/sps/vps destructed, encoder channel:1
[DEBUG:RTSP.cpp]: Got necessary NAL Units.
[INFO:RTSP.cpp]: stream 1 available at: rtsp://192.168.50.101/ch1
[DEBUG:IMPServerMediaSubsession.cpp]: Create Stream Source. 
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video constructed, encoder channel:0
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video destructed, encoder channel:0
[DEBUG:IMPServerMediaSubsession.cpp]: Create Stream Source. 
[DEBUG:IMPDeviceSource.cpp]: IMPDeviceSource video constructed, encoder channel:0
logcat
I/ao( 1900): AO Enable: 0
E/ao( 1900): open /dev/dsp error
E/ao( 1900): __ao_dev_init failed.
I/OSD( 1958): IMP_OSD_SetPoolSize:1048576
D/IMP-ISP( 1958): ~~~~~~ IMP_ISP_Open[331] ~~~~~~~
I/IMP-ISP( 1958): IMP_ISP_AddSensor,480: paddr = 0x1, size = 0x1
I/Alloc Manager( 1958): MEM Alloc Method is kmalloc
D/KMEM Method( 1958): CMD Line Rmem Size:22544384, Addr:0x02a80000
D/KMEM Method( 1958): alloc->mem_alloc.method = kmalloc
 			alloc->mem_alloc.vaddr = 0x753b1000
 			alloc->mem_alloc.paddr = 0x02a80000
 			alloc->mem_alloc.length = 22544384
I/Alloc Manager( 1958): MEM Manager Method is continuous
D/System( 1958): IMP_System_Init SDK Version:1.1.6-a6394f42-Mon Dec 5 14:39:51 2022 +0800, built: Dec 29 2022 15:38:51
D/System( 1958): system_init()
D/System( 1958): Calling DSystem
D/System( 1958): Calling FrameSource
D/System( 1958): [ignored]read /proc/cpuinfo ret is NULLD/System( 1958): Calling IVS
D/System( 1958): Calling OSD
D/System( 1958): Calling Encoder
D/System( 1958): Calling FB
E/Framesource( 1958): IMP_FrameSource_GetChnAttr(): chnAttr was not set yet
D/Encoder( 1958): IMP_Encoder_SetbufshareChn: encChn:2, shareChn:0
E/MemPool( 1958): IMP_Encoder_GetPool(64):chnNum: 0 not bind pool
I/Encoder( 1958): encChn=0,srcFrameCnt=3,srcFrameSize=3133440
I/Encoder( 1958): encChn=0,srcStreamCnt=2,enc_chn->stream_frame_size=949248
D/System( 1958): system_bind(): bind DST-Encoder-0(1.0.0) to SRC-Framesource-0(0.0.0)
I/Framesource( 1958): [chn0]: width = 1920 height = 1080
I/VBM( 1958): VBMCreatePool()-0: w=1920 h=1080 f=842094158 nrVBs=1
I/VBM( 1958): VBMCreatePool()-0: pool->config.fmt.fmt.pix.sizeimage=3133440 sizeimage=3133440
E/Framesource( 1958): IMP_FrameSource_GetPool(3294):chnNum: 0 not bind pool
E/VBM( 1958): VBMCreatePool()-0: sizeimage=3133440
I/VBM( 1958): PoolId:0, frame=0x76c6f820, frame->priv=0x76c6f84c, frame[0].virAddr=75d12200, frame[0].phyAddr=33e1200
E/Framesource( 1958): IMP_FrameSource_GetChnAttr(): chnAttr was not set yet
E/MemPool( 1958): IMP_Encoder_GetPool(64):chnNum: 1 not bind pool
I/Encoder( 1958): encChn=1,srcFrameCnt=3,srcFrameSize=353280
I/Encoder( 1958): encChn=1,srcStreamCnt=2,enc_chn->stream_frame_size=199808
D/System( 1958): system_bind(): bind DST-Encoder-1(1.1.0) to SRC-Framesource-1(0.1.0)
I/Framesource( 1958): [chn1]: width = 640 height = 360
I/VBM( 1958): VBMCreatePool()-1: w=640 h=360 f=842094158 nrVBs=1
I/VBM( 1958): VBMCreatePool()-1: pool->config.fmt.fmt.pix.sizeimage=353280 sizeimage=353280
E/Framesource( 1958): IMP_FrameSource_GetPool(3294):chnNum: 1 not bind pool
E/VBM( 1958): VBMCreatePool()-1: sizeimage=353280
I/VBM( 1958): PoolId:1, frame=0x76f0f230, frame->priv=0x76f0f25c, frame[0].virAddr=76149300, frame[0].phyAddr=3818300
I/Encoder( 1958): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 1958): enc_chn->index=0, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=20, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=2, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 1958): enc_chn->index=0, enc_chn->chnFpsMask=1, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=25, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=25, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 1958): do_day_night_change(664):enc_chn->inStat.is_day=1
E/MemPool( 1958): IMP_Encoder_GetPool(64):chnNum: 2 not bind pool
I/Encoder( 1958): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 1958): enc_chn->index=1, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=20, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=2, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 1958): enc_chn->index=1, enc_chn->chnFpsMask=1, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=25, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=25, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 1958): do_day_night_change(664):enc_chn->inStat.is_day=1
I/Encoder( 1958): encChn=2,srcFrameCnt=2,srcFrameSize=3133440
I/Encoder( 1958): encChn=2,srcStreamCnt=0,enc_chn->stream_frame_size=0
I/Encoder( 1958): framePriv->i_fps_num=25, framePriv->i_fps_den=1
D/Encoder( 1958): enc_chn->index=2, gopAttr->uGopCtrlMode=2, gopAttr->uGopLength=0, gopAttr->uNotifyUserLTInter=0, gopAttr->uMaxSameSenceCnt=1, gopAttr->bEnableLT=0, gopAttr->uFreqLT=0, gopAttr->bLTRC=0
D/Encoder( 1958): enc_chn->index=2, enc_chn->chnFpsMask=1fff7ff, enc_chn->inFrmRate.frmRateNum=25, enc_chn->inFrmRate.frmRateDen=1, enc_chn->setFrmRate.frmRateNum=24, enc_chn->setFrmRate.frmRateDen=1, rcAttr->outFrmRate.frmRateNum=24, rcAttr->outFrmRate.frmRateDen=1
D/Encoder( 1958): do_day_night_change(664):enc_chn->inStat.is_day=1
```
