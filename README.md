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
ourISPdev allocated at 80b10000
ISP probe called
Starting GPIO config init
tx-isp tx-isp: GPIO setup: reset=18 pwdn=-1
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
Initializing VIC control
Initial VIC register state:
0x000: 0x00060000
0x004: 0x00020000
0x008: 0x00c80000
0x00c: 0x1eff0000
0x010: 0x0000c83f
0x014: 0x00001e0a
0x018: 0x080f003f
0x01c: 0x141f0000
0x020: 0x003fff08
0x024: 0x00133200
0x028: 0x00010610
0x02c: 0xff00ff00
0x030: 0x0003ff00
0x034: 0x00000000
0x038: 0x00000000
0x03c: 0x00000000
0x040: 0x00000000
0x044: 0x00000000
0x048: 0x20202020
0x04c: 0x20202020
0x050: 0x20202020
0x054: 0x20202020
0x058: 0x20202020
0x05c: 0x20202020
0x060: 0x20202020
0x064: 0x20202020
0x068: 0x00380038
0x06c: 0x00380038
0x070: 0x00380038
0x074: 0x00380038
0x078: 0x00000000
0x07c: 0x00000000
0x080: 0x01000000
0x084: 0x00000000
0x088: 0x00080000
0x08c: 0x00010001
0x090: 0x00000000
0x094: 0x00000000
0x098: 0x00000000
0x09c: 0x00000000
0x0a0: 0x00000000
0x0a4: 0x00000000
0x0a8: 0x00000000
0x0ac: 0x00000000
0x0b0: 0x00000000
0x0b4: 0x00000000
0x0b8: 0x00000000
0x0bc: 0x00000000
0x0c0: 0x00000000
0x0c4: 0x00000000
0x0c8: 0x00000000
0x0cc: 0x00000000
0x0d0: 0x00000000
0x0d4: 0x00000000
0x0d8: 0x00000000
0x0dc: 0x00000000
0x0e0: 0x00000000
0x0e4: 0x00000000
0x0e8: 0x00000000
0x0ec: 0x00000000
0x0f0: 0x00000000
0x0f4: 0x00000000
0x0f8: 0x00000000
0x0fc: 0x00000000
0x100: 0x00000000
0x104: 0x00000000
0x108: 0x00000000
0x10c: 0x00000000
0x110: 0x00000000
0x114: 0x00000000
0x118: 0x00000000
0x11c: 0x00000000
0x120: 0x00000000
0x124: 0x00000000
0x128: 0x00000000
0x12c: 0x00000000
0x130: 0x00000000
0x134: 0x00000000
0x138: 0x00000000
0x13c: 0x00000000
0x140: 0x00000000
0x144: 0x00000000
0x148: 0x00000000
0x14c: 0x00000000
0x150: 0x00000000
0x154: 0x00000000
0x158: 0x00000000
0x15c: 0x00000000
0x160: 0x00000000
0x164: 0x00000000
0x168: 0x00000000
0x16c: 0x00000000
0x170: 0x00000000
0x174: 0x00000000
0x178: 0x00000000
0x17c: 0x00000000
0x180: 0x00000000
0x184: 0x00000000
0x188: 0x00000000
0x18c: 0x00000000
0x190: 0x00000000
0x194: 0x00000000
0x198: 0x00000000
0x19c: 0x00000000
0x1a0: 0x00000000
0x1a4: 0x00000000
0x1a8: 0x00000000
0x1ac: 0x00000000
0x1b0: 0x00000000
0x1b4: 0x00000000
0x1b8: 0x00000000
0x1bc: 0x00000000
0x1c0: 0x00000000
0x1c4: 0x00000000
0x1c8: 0x00000000
0x1cc: 0x00000000
0x1d0: 0x00000000
0x1d4: 0x00000000
0x1d8: 0x00000000
0x1dc: 0x00000000
0x1e0: 0x00000000
0x1e4: 0x00000000
0x1e8: 0x00000000
0x1ec: 0x00000000
0x1f0: 0x00000000
0x1f4: 0x00000000
0x1f8: 0x00000000
0x1fc: 0x00000000
0x200: 0x00000000
0x204: 0x00000000
0x208: 0x00000000
0x20c: 0x00000000
0x210: 0x00000000
0x214: 0x00000000
0x218: 0x00000000
0x21c: 0x00000000
0x220: 0x00000000
0x224: 0x00000000
0x228: 0x00000000
0x22c: 0x00000000
0x230: 0x00000000
0x234: 0x00000000
0x238: 0x00000000
0x23c: 0x00000000
0x240: 0x00000000
0x244: 0x00000000
0x248: 0x00000000
0x24c: 0x00000000
0x250: 0x00000000
0x254: 0x00000000
0x258: 0x00000000
0x25c: 0x00000000
0x260: 0x00000000
0x264: 0x00000000
0x268: 0x00000000
0x26c: 0x00000000
0x270: 0x00000000
0x274: 0x00000000
0x278: 0x00000000
0x27c: 0x00000000
0x280: 0x00000000
0x284: 0x00000000
0x288: 0x00000000
0x28c: 0x00000000
0x290: 0x00000000
0x294: 0x00000000
0x298: 0x00000000
0x29c: 0x00000000
0x2a0: 0x00000000
0x2a4: 0x00000000
0x2a8: 0x00000000
0x2ac: 0x00000000
0x2b0: 0x00000000
0x2b4: 0x00000000
0x2b8: 0x00000000
0x2bc: 0x00000000
0x2c0: 0x00000000
0x2c4: 0x00000000
0x2c8: 0x00000000
0x2cc: 0x00000000
0x2d0: 0x00000000
0x2d4: 0x00000000
0x2d8: 0x00000000
0x2dc: 0x00000000
0x2e0: 0x00000000
0x2e4: 0x00000000
0x2e8: 0x00000000
0x2ec: 0x00000000
0x2f0: 0x00000000
0x2f4: 0x00000000
0x2f8: 0x00000000
0x2fc: 0x00000000
0x300: 0x00000000
0x304: 0x00000000
0x308: 0x00000000
0x30c: 0x00000000
0x310: 0x00000000
0x314: 0x00000000
0x318: 0x00000000
0x31c: 0x00000000
0x320: 0x00000000
0x324: 0x00000000
0x328: 0x00000000
0x32c: 0x00000000
0x330: 0x00000000
0x334: 0x00000000
0x338: 0x00000000
0x33c: 0x00000000
0x340: 0x00000000
0x344: 0x00000000
0x348: 0x00000000
0x34c: 0x00000000
0x350: 0x00000000
0x354: 0x00000000
0x358: 0x00000000
0x35c: 0x00000000
0x360: 0x00000000
0x364: 0x00000000
0x368: 0x00000000
0x36c: 0x00000000
0x370: 0x00000000
0x374: 0x00000000
0x378: 0x00000000
0x37c: 0x00000000
0x380: 0x00000000
0x384: 0x00000000
0x388: 0x00000000
0x38c: 0x00000000
0x390: 0x00000000
0x394: 0x00000000
0x398: 0x00000000
0x39c: 0x00000000
0x3a0: 0x00000000
0x3a4: 0x00000000
0x3a8: 0x00000000
0x3ac: 0x00000000
0x3b0: 0x00000000
0x3b4: 0x00000000
0x3b8: 0x00000000
0x3bc: 0x00000000
0x3c0: 0x00000000
0x3c4: 0x00000000
0x3c8: 0x00000000
0x3cc: 0x00000000
0x3d0: 0x00000000
0x3d4: 0x00000000
0x3d8: 0x00000000
0x3dc: 0x00000000
0x3e0: 0x00000000
0x3e4: 0x00000000
0x3e8: 0x00000000
0x3ec: 0x00000000
0x3f0: 0x00000000
0x3f4: 0x00000000
0x3f8: 0x00000000
0x3fc: 0x00000000
0x400: 0x00000000
0x404: 0x00000000
0x408: 0x00000000
0x40c: 0x00000000
0x410: 0x00000000
0x414: 0x00000000
0x418: 0x00000000
0x41c: 0x00000000
0x420: 0x00000000
0x424: 0x00000000
0x428: 0x00000000
0x42c: 0x00000000
0x430: 0x00000000
0x434: 0x00000000
0x438: 0x00000000
0x43c: 0x00000000
0x440: 0x00000000
0x444: 0x00000000
0x448: 0x00000000
0x44c: 0x00000000
0x450: 0x00000000
0x454: 0x00000000
0x458: 0x00000000
0x45c: 0x00000000
0x460: 0x00000000
0x464: 0x00000000
0x468: 0x00000000
0x46c: 0x00000000
0x470: 0x00000000
0x474: 0x00000000
0x478: 0x00000000
0x47c: 0x00000000
0x480: 0x00000000
0x484: 0x00000000
0x488: 0x00000000
0x48c: 0x00000000
0x490: 0x00000000
0x494: 0x00000000
0x498: 0x00000000
0x49c: 0x00000000
0x4a0: 0x00000000
0x4a4: 0x00000000
0x4a8: 0x00000000
0x4ac: 0x00000000
0x4b0: 0x00000000
0x4b4: 0x00000000
0x4b8: 0x00000000
0x4bc: 0x00000000
0x4c0: 0x00000000
0x4c4: 0x00000000
0x4c8: 0x00000000
0x4cc: 0x00000000
0x4d0: 0x00000000
0x4d4: 0x00000000
0x4d8: 0x00000000
0x4dc: 0x00000000
0x4e0: 0x00000000
0x4e4: 0x00000000
0x4e8: 0x00000000
0x4ec: 0x00000000
0x4f0: 0x00000000
0x4f4: 0x00000000
0x4f8: 0x00000000
0x4fc: 0x00000000
0x500: 0x00000000
0x504: 0x00000000
0x508: 0x00000000
0x50c: 0x00000000
0x510: 0x00000000
0x514: 0x00000000
0x518: 0x00000000
0x51c: 0x00000000
0x520: 0x00000000
0x524: 0x00000000
0x528: 0x00000000
0x52c: 0x00000000
0x530: 0x00000000
0x534: 0x00000000
0x538: 0x00000000
0x53c: 0x00000000
0x540: 0x00000000
0x544: 0x00000000
0x548: 0x00000000
0x54c: 0x00000000
0x550: 0x00000000
0x554: 0x00000000
0x558: 0x00000000
0x55c: 0x00000000
0x560: 0x00000000
0x564: 0x00000000
0x568: 0x00000000
0x56c: 0x00000000
0x570: 0x00000000
0x574: 0x00000000
0x578: 0x00000000
0x57c: 0x00000000
0x580: 0x00000000
0x584: 0x00000000
0x588: 0x00000000
0x58c: 0x00000000
0x590: 0x00000000
0x594: 0x00000000
0x598: 0x00000000
0x59c: 0x00000000
0x5a0: 0x00000000
0x5a4: 0x00000000
0x5a8: 0x00000000
0x5ac: 0x00000000
0x5b0: 0x00000000
0x5b4: 0x00000000
0x5b8: 0x00000000
0x5bc: 0x00000000
0x5c0: 0x00000000
0x5c4: 0x00000000
0x5c8: 0x00000000
0x5cc: 0x00000000
0x5d0: 0x00000000
0x5d4: 0x00000000
0x5d8: 0x00000000
0x5dc: 0x00000000
0x5e0: 0x00000000
0x5e4: 0x00000000
0x5e8: 0x00000000
0x5ec: 0x00000000
0x5f0: 0x00000000
0x5f4: 0x00000000
0x5f8: 0x00000000
0x5fc: 0x00000000
0x600: 0x00000000
0x604: 0x00000000
0x608: 0x00000000
0x60c: 0x00000000
0x610: 0x00000000
0x614: 0x00000000
0x618: 0x00000000
0x61c: 0x00000000
0x620: 0x00000000
0x624: 0x00000000
0x628: 0x00000000
0x62c: 0x00000000
0x630: 0x00000000
0x634: 0x00000000
0x638: 0x00000000
0x63c: 0x00000000
0x640: 0x00000000
0x644: 0x00000000
0x648: 0x00000000
0x64c: 0x00000000
0x650: 0x00000000
0x654: 0x00000000
0x658: 0x00000000
0x65c: 0x00000000
0x660: 0x00000000
0x664: 0x00000000
0x668: 0x00000000
0x66c: 0x00000000
0x670: 0x00000000
0x674: 0x00000000
0x678: 0x00000000
0x67c: 0x00000000
0x680: 0x00000000
0x684: 0x00000000
0x688: 0x00000000
0x68c: 0x00000000
0x690: 0x00000000
0x694: 0x00000000
0x698: 0x00000000
0x69c: 0x00000000
0x6a0: 0x00000000
0x6a4: 0x00000000
0x6a8: 0x00000000
0x6ac: 0x00000000
0x6b0: 0x00000000
0x6b4: 0x00000000
0x6b8: 0x00000000
0x6bc: 0x00000000
0x6c0: 0x00000000
0x6c4: 0x00000000
0x6c8: 0x00000000
0x6cc: 0x00000000
0x6d0: 0x00000000
0x6d4: 0x00000000
0x6d8: 0x00000000
0x6dc: 0x00000000
0x6e0: 0x00000000
0x6e4: 0x00000000
0x6e8: 0x00000000
0x6ec: 0x00000000
0x6f0: 0x00000000
0x6f4: 0x00000000
0x6f8: 0x00000000
0x6fc: 0x00000000
0x700: 0x00000000
0x704: 0x00000000
0x708: 0x00000000
0x70c: 0x00000000
0x710: 0x00000000
0x714: 0x00000000
0x718: 0x00000000
0x71c: 0x00000000
0x720: 0x00000000
0x724: 0x00000000
0x728: 0x00000000
0x72c: 0x00000000
0x730: 0x00000000
0x734: 0x00000000
0x738: 0x00000000
0x73c: 0x00000000
0x740: 0x00000000
0x744: 0x00000000
0x748: 0x00000000
0x74c: 0x00000000
0x750: 0x00000000
0x754: 0x00000000
0x758: 0x00000000
0x75c: 0x00000000
0x760: 0x00000000
0x764: 0x00000000
0x768: 0x00000000
0x76c: 0x00000000
0x770: 0x00000000
0x774: 0x00000000
0x778: 0x00000000
0x77c: 0x00000000
0x780: 0x00000000
0x784: 0x00000000
0x788: 0x00000000
0x78c: 0x00000000
0x790: 0x00000000
0x794: 0x00000000
0x798: 0x00000000
0x79c: 0x00000000
0x7a0: 0x00000000
0x7a4: 0x00000000
0x7a8: 0x00000000
0x7ac: 0x00000000
0x7b0: 0x00000000
0x7b4: 0x00000000
0x7b8: 0x00000000
0x7bc: 0x00000000
0x7c0: 0x00000000
0x7c4: 0x00000000
0x7c8: 0x00000000
0x7cc: 0x00000000
0x7d0: 0x00000000
0x7d4: 0x00000000
0x7d8: 0x00000000
0x7dc: 0x00000000
0x7e0: 0x00000000
0x7e4: 0x00000000
0x7e8: 0x00000000
0x7ec: 0x00000000
0x7f0: 0x00000000
0x7f4: 0x00000000
0x7f8: 0x00000000
0x7fc: 0x00000000
0x804: 0x00000000
0x808: 0x00000000
0x810: 0x00010001
0x814: 0x00000000
0x818: 0x00000000
0x820: 0x00000000
0x824: 0x00000000
0x828: 0x00000000
0x82c: 0x00000000
0x830: 0x00000000
0x834: 0x00000000
0x838: 0x00000000
0x83c: 0x00000000
0x840: 0x00000000
0x844: 0x00000000
0x848: 0x00000000
0x84c: 0x00000000
0x850: 0x00000000
0x854: 0x00000000
0x858: 0x00000000
0x85c: 0x00000000
0x860: 0x00000000
0x864: 0x00000000
0x868: 0x00000000
0x86c: 0x00000000
0x870: 0x00000000
0x874: 0x00000000
0x878: 0x00000000
0x87c: 0x00000000
0x880: 0x00000000
0x884: 0x00000000
0x888: 0x00000000
0x88c: 0x00000000
0x890: 0x00000000
0x894: 0x00000000
0x898: 0x00000000
0x89c: 0x00000000
0x8a0: 0x00000000
0x8a4: 0x00000000
0x8a8: 0x00000000
0x8ac: 0x00000000
0x8b0: 0x00000000
0x8b4: 0x00000000
0x8b8: 0x00000000
0x8bc: 0x00000000
0x8c0: 0x00000000
0x8c4: 0x00000000
0x8c8: 0x00000000
0x8cc: 0x00000000
0x8d0: 0x00000000
0x8d4: 0x00000000
0x8d8: 0x00000000
0x8dc: 0x00000000
0x8e0: 0x00000000
0x8e4: 0x00000000
0x8e8: 0x00000000
0x8ec: 0x00000000
0x8f0: 0x00000000
0x8f4: 0x00000000
0x8f8: 0x00000000
0x8fc: 0x00000000
0x900: 0x00000000
0x904: 0x00000000
0x908: 0x00000000
0x90c: 0x00000000
0x910: 0x00000000
0x914: 0x00000000
0x918: 0x00000000
0x91c: 0x00000000
0x920: 0x00000000
0x924: 0x00000000
0x928: 0x00000000
0x92c: 0x00000000
0x930: 0x00000000
0x934: 0x00000000
0x938: 0x00000000
0x93c: 0x00000000
0x940: 0x00000000
0x944: 0x00000000
0x948: 0x00000000
0x94c: 0x00000000
0x950: 0x00000000
0x954: 0x00000000
0x958: 0x00000000
0x95c: 0x00000000
0x960: 0x00000000
0x964: 0x00000000
0x968: 0x00000000
0x96c: 0x00000000
0x970: 0x00000000
0x974: 0x00000000
0x978: 0x00000000
0x97c: 0x00000000
0x980: 0x00000000
0x984: 0x00000000
0x988: 0x00000000
0x98c: 0x00000000
0x990: 0x00000000
0x994: 0x00000000
0x998: 0x00000000
0x99c: 0x00000000
0x9a0: 0x00000000
0x9a4: 0x00000000
0x9a8: 0x00000000
0x9ac: 0x00000000
0x9b0: 0x00000000
0x9b4: 0x00000000
0x9b8: 0x00000000
0x9bc: 0x00000000
0x9c0: 0x00000000
0x9c4: 0x00000000
0x9c8: 0x00000000
0x9cc: 0x00000000
0x9d0: 0x00000000
0x9d4: 0x00000000
0x9d8: 0x00000000
0x9dc: 0x00000000
0x9e0: 0x00000000
0x9e4: 0x00000000
0x9e8: 0x00000000
VIC State:
0x7000: 0x00000000
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
0x793c: 0x000033fb
0x79e8: 0xffffffff
0x7b00: 0x04000010
Starting sub device init
Creating ISP subdevices...
tx-isp tx-isp: ISP subdevices registered successfully
Starting ISP IRQ init
tx-isp tx-isp: TX-ISP probe completed successfully
CSI probe starting
Starting subdev init for tx-isp-csi
No additional clocks needed for subdev tx-isp-csi
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b0022000
Module initialized: tx-isp-csi (in:1 out:1)
Subdev tx-isp-csi initialized successfully
CSI probe completed successfully
VIC probe called
Starting subdev init for tx-isp-vic
No additional clocks needed for subdev tx-isp-vic
Using register base: b3307000
Module initialized: tx-isp-vic (in:0 out:0)
Subdev tx-isp-vic initialized successfully
VIC probe completed successfully
VIN probe called
Starting subdev init for tx-isp-vin
No additional clocks needed for subdev tx-isp-vin
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: tx-isp-vin (in:1 out:1)
Subdev tx-isp-vin initialized successfully
VIN probe completed successfully
Core probe called
Starting subdev init for tx-isp-core
No additional clocks needed for subdev tx-isp-core
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Module initialized: tx-isp-core (in:1 out:1)
Subdev tx-isp-core initialized successfully
Initial ctrl reg value: 0x54560031
Read back value after enable: 0x54560031
Core probe complete: dev=80b10000 reg_base=b3300000
Frame source probe called
Starting subdev init for tx-isp-fs
No additional clocks needed for subdev tx-isp-fs
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
No additional clocks needed for subdev sc2336
Created channel data for input pad 0
Created channel 0 for pad 0
Using register base: b3300000
Unknown subdev name: sc2336
Module initialized: sc2336 (in:1 out:1)
Subdev sc2336 initialized successfully
ISP device open called from pid 1958
ISP opened: file=81feacf0 fd=0
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x77197ce0
file=80a67820 flags=0x2002
Initial sensor reset...
Detected sensor: sc2336 (ID: 0xcb3a) 1920x1080
Sensor registered: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f933d78
file=80a67820 flags=0x2002
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f933d78
file=80a67820 flags=0x2002
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7f933dd0
file=80a67820 flags=0x2002
Sensor command: 0xc0045627
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7f933dc8
file=80a67820 flags=0x2002
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7f933dc8
file=80a67820 flags=0x2002
tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO
Buffer info before update: method=0x33326373 phys=0x3633 size=0
Buffer info after update:
method=0x0
phys=0x2a80000
virt=a2e80000
size=8257536
flags=0x1
tx_isp: Buffer setup completed successfully
ISP IOCTL called: cmd=0x40045626

=== IOCTL Debug ===
cmd=0x40045626 arg=0x7f933de0
file=80a67820 flags=0x2002
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
Sensor info request: returning success (1)
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=80a67820 flags=0x2002
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Sensor streamon
ISP State:
Control: 0x54560031
Status: 0x00000000
Int mask: 0x00000000
VIC State:
0x7000: 0x00000000
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
0x793c: 0x000033fb
0x79e8: 0xffffffff
0x7b00: 0x04000010
Stream enabled!
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7f933de0
file=80a67820 flags=0x2002
Starting link setup IOCTL
Setting up video link 0
Found devices and pads:
subdev=80b22b10
num_inpads=1
num_outpads=1
Setting up link 1 of 2
Looking up pad: subdev=80b22b10 cfg=c02bad28
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found source pad 80b1ef00
Looking up pad: subdev=80b22b10 cfg=80a6bd48
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found sink pad 80b1ef00
Setting up link 2 of 2
Looking up pad: subdev=80b22b10 cfg=c02bad3c
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found source pad 80b1ef00
Looking up pad: subdev=80b22b10 cfg=80a6bd48
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found sink pad 80b1ef00
Link setup completed successfully
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=80a67820 flags=0x2002
Enabling stream ...
Configuring VIC for streaming...
VIC State:
0x7000: 0x00000000
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
0x7840: 0x00000002
0x793c: 0x000033fb
0x7b00: 0x04000010
VIC IRQ State:
Enable (0x93c): 0x000033fb
Mask (0x9e8): 0x00000000
Status (0xb4): 0x00000000
Status2 (0xb8): 0x00000000
ISP Register Space Dump:
Core ISP Registers:
Base+0x00: 0x54560031 (ID/Version)
Base+0x0c: 0x80700019 (Control)
Base+0xb0: 0xffffffff (IRQ Enable)
Base+0xb4: 0x00000000 (IRQ Status)

VIC Control/Status:
Base+0x110: 0x80007000 (VIC Control)
Base+0x114: 0x00777111 (VIC Status)
Base+0x840: 0x00010001 (VIC Config)

Complete Register Dump (non-zero values):
0x13300000: 0x54560031
0x13300004: 0x00000001
0x13300008: 0x00000001
0x1330000c: 0x80700019
0x13300010: 0x00000001
0x13300014: 0x00000001
0x13300028: 0x00000001
0x1330002c: 0x00400040
0x13300030: 0x00000001
0x13300090: 0x00000001
0x13300094: 0x00000001
0x13300098: 0x00030000
0x133000a8: 0x58050000
0x133000ac: 0x58050000
0x133000b0: 0xffffffff
0x133000c4: 0x00040000
0x133000c8: 0x00400040
0x133000cc: 0x00000100
0x133000d4: 0x0000000c
0x133000d8: 0x00ffffff
0x133000e0: 0x00000100
0x133000e4: 0x00400040
0x133000f0: 0xff808000
0x13300110: 0x80007000
0x13300114: 0x00777111
0x13300840: 0x00010001
ISP M0 device opened, tuning_data=81ee7e00
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 1
ISP m0 IOCTL called: cmd=0xc008561b
ISP_CORE_G_CTRL GET cmd=0xc008561b
Get format entry: arg=7f933d38
Channel attributes not initialized
ISP m0 IOCTL called: cmd=0xc008561b
ISP_CORE_G_CTRL GET cmd=0xc008561b
Get format entry: arg=7f933d38
Channel attributes not initialized
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980901
ISP set control: cmd=0x980901 value=128
Set contrast to 128
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x98091b
ISP set control: cmd=0x98091b value=128
Set sharpness to 128
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980902
ISP set control: cmd=0x980902 value=128
Set saturation to 128
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980900
ISP set control: cmd=0x980900 value=128
Set brightness to 128
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980901
ISP set control: cmd=0x980901 value=128
Set contrast to 128
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x98091b
ISP set control: cmd=0x98091b value=128
Set sharpness to 128
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 1
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980902
ISP set control: cmd=0x980902 value=128
Set saturation to 128
CPU 0 Unable to handle kernel paging request at virtual address 00000008, epc == c02a8b2c, ra == 800d96d4
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980900
ISP set control: cmd=0x980900 value=128
Set brightness to 128
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980914
ISP set control: cmd=0x980914 value=0
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980915
ISP set control: cmd=0x980915 value=0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP IOCTL called: cmd=0x800456d3

=== IOCTL Debug ===
cmd=0x800456d3 arg=0x0
file=80a67820 flags=0x2002
TODO Disabling ISP links
ISP IOCTL called: cmd=0x800456d1

=== IOCTL Debug ===
cmd=0x800456d1 arg=0x7f933de0
file=80a67820 flags=0x2002
TODO Destroying ISP links
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x8000164
ISP set control: cmd=0x8000164 value=1
Set bypass mode to 1
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7f933de0
file=80a67820 flags=0x2002
Starting link setup IOCTL
Setting up video link 0
Found devices and pads:
subdev=80b22b10
num_inpads=1
num_outpads=1
Setting up link 1 of 2
Looking up pad: subdev=80b22b10 cfg=c02bad28
Pad lookup for tx-isp-csi:
type=1 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found source pad 80b1ef00
Looking up pad: subdev=80b22b10 cfg=80a6bd48
Pad lookup for tx-isp-vic:
type=0 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found sink pad 80b1ef00
Setting up link 2 of 2
Looking up pad: subdev=80b22b10 cfg=c02bad3c
Pad lookup for tx-isp-vic:
type=1 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found source pad 80b1ef00
Looking up pad: subdev=80b22b10 cfg=80a6bd48
Pad lookup for tx-isp-ddr:
type=0 index=0
inpads=80b1ee80 (count=1)
outpads=80b1ef00 (count=1)
Found sink pad 80b1ef00
Link setup completed successfully
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=80a67820 flags=0x2002
Enabling stream ...
Configuring VIC for streaming...
VIC State:
0x7000: 0x00000000
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
0x7840: 0x00000002
0x793c: 0x000033fb
0x7b00: 0x04000010
VIC IRQ State:
Enable (0x93c): 0x000033fb
Mask (0x9e8): 0x00000000
Status (0xb4): 0x00000000
Status2 (0xb8): 0x00000000
ISP Register Space Dump:
Core ISP Registers:
Base+0x00: 0x54560031 (ID/Version)
Base+0x0c: 0x80700019 (Control)
Base+0xb0: 0xffffffff (IRQ Enable)
Base+0xb4: 0x00000000 (IRQ Status)

VIC Control/Status:
Base+0x110: 0x80007000 (VIC Control)
Base+0x114: 0x00777111 (VIC Status)
Base+0x840: 0x00010001 (VIC Config)

Complete Register Dump (non-zero values):
0x13300000: 0x54560031
0x13300004: 0x00000001
0x13300008: 0x00000001
0x1330000c: 0x80700019
0x13300010: 0x00000001
0x13300014: 0x00000001
0x13300028: 0x00000001
0x1330002c: 0x00400040
0x13300030: 0x00000001
0x13300090: 0x00000001
0x13300094: 0x00000001
0x13300098: 0x00030000
0x133000a8: 0x58050000
0x133000ac: 0x58050000
0x133000b0: 0xffffffff
0x133000c4: 0x00040000
0x133000c8: 0x00400040
0x133000cc: 0x00000100
0x133000d4: 0x0000000c
0x133000d8: 0x00ffffff
0x133000e0: 0x00000100
0x133000e4: 0x00400040
0x133000f0: 0xff808000
0x13300110: 0x80007000
0x13300114: 0x00777111
0x13300840: 0x00010001
ISP m0 IOCTL called: cmd=0xc008561c
SET cmd=0x980918
ISP set control: cmd=0x980918 value=2
Set antiflicker mode to 2
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 0
Oops[#1]:
CPU: 0 PID: 1962 Comm: isp_tuning_deam Tainted: G           O 3.10.14__isvp_swan_1.0__ #1
task: 82167620 ti: 815b0000 task.ti: 815b0000
$ 0   : 00000000 10001c00 c02a8b24 00000000
$ 4   : 80be7080 00280000 00000000 815b1f10
$ 8   : 00000001 80000008 800b86e8 fffffff8
$12   : 00000008 00008000 00010000 826dc7f0
$16   : 80be7080 80a79600 00000000 815b1f10
$20   : 815b1f10 7693a648 80a676e0 80be70a8
$24   : 826dc78c 77126b70
$28   : 815b0000 815b1e60 00000400 800d96d4
Hi    : 00000000
Lo    : 65656565
epc   : c02a8b2c 0xc02a8b2c
Tainted: G           O
ra    : 800d96d4 0x800d96d4
Status: 10001c03	KERNEL EXL IE
Cause : 40808008
BadVA : 00000008
PrId  : 00d00100 (Ingenic Xburst)
Modules linked in: sensor_sc2336_t31(O) tx_isp_t31(O) atbm6031(O) gpio_userkeys(O) avpu(O) exfat(O) motor(O) jzmmc_v12
Process isp_tuning_deam (pid: 1962, threadinfo=815b0000, task=82167620, tls=75320dd8)
Stack : 00000001 00000002 00000000 00000000 00000041 00000000 00000000 00000000
00000001 00000007 fffffffb 80a79600 00000001 7693a648 815b1f10 00000000
00000000 00000000 7693a5b0 801062c0 82002c80 800b16a4 00000010 80a673c0
80a676e0 00000400 80a676e0 800b8008 0000000b 80b58000 80a676e0 800d372c
80a676e8 800b73dc 80a676e0 fffffff7 7693a648 00000400 00000001 800b8748
...
Call Trace:[<801062c0>] 0x801062c0
[<800b16a4>] 0x800b16a4
[<800b8008>] 0x800b8008
[<800d372c>] 0x800d372c
[<800b73dc>] 0x800b73dc
[<800b8748>] 0x800b8748
[<80050000>] 0x80050000
[<8002257c>] 0x8002257c


Code: 00000000  8c83003c  3c050028 <8c630008> 3c19800e  00651821  8c634380  00801025  10600004
Setting up control block at ISP base + 0x9888 = 13309888
Framechan0 opened successfully
VIDIOC_S_FMT: arg=0x752b0828
Format entry
Size calculation for channel 0: base=3110400 metadata=23040 total=3133440
Updated format for channel 0:
Width: 1920 (aligned: 1920)
Height: 1080
Final size: 3133440
Stride: 1920
Allocated buffer 0 meta=80be7800 group=80ac0800
Registered event handler for channel 0
No event handler for pad 80b101d4 cmd 0x1000
Ch0 QBUF: buffer 0 queued (total=1)
Framechan Streamon command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Ch0 DQBUF start: ready=1 done=0 queued=1
Sensor streamon
Frame thread starting for channel 0
---[ end trace bdc6abdb09deb812 ]---
Ch0 DQBUF: returned buffer 0 seq=0
ISP m0 IOCTL called: cmd=0xc00c56c6
Tuning enable: 1
Ch0 QBUF: buffer 0 queued (total=0)
Ch0 DQBUF start: ready=1 done=0 queued=0
Frame thread initialized for channel 0 with VBM pool 77051f30 (mapped c02e0f30)
Enabled streaming on channel 0
ISP State:
Control: 0x54560031
Status: 0x00000001
Int mask: 0xffffffff
VIC State:
0x7000: 0x00000000
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
0x7840: 0x00000002
0x793c: 0x000033fb
0x7b00: 0x04000010
Stream enabled!
Setting up control block at ISP base + 0x9888 = 13309888
Framechan1 opened successfully
VIDIOC_S_FMT: arg=0x7519e828
Format entry
Size calculation for channel 1: base=345600 metadata=7680 total=353280
Updated format for channel 1:
Width: 640 (aligned: 640)
Height: 360
Final size: 353280
Stride: 640
Allocated buffer 0 meta=80ed1b00 group=80ac1800
Registered event handler for channel 1
No event handler for pad 80b10ed4 cmd 0x1000
Ch1 QBUF: buffer 0 queued (total=1)
Framechan Streamon command: 0x80045612
Sensor command: 0x80045612
Sensor command: 0x80045612
Ch1 DQBUF start: ready=1 done=0 queued=1
Sensor streamon
Frame thread starting for channel 1
...
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
