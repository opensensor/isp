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
ourISPdev allocated at 81a9fc00
Registering platform driver
Platform driver registered successfully
Registering platform device with name 'tisp-driver'
Probing TISP device...
ISP register mapping: base=b3300000 test_read=0x0
Configuring ISP clocks using standard API
Clock rates after configuration:
ISP Core: 250000000 Hz
CGU ISP: 125000000 Hz
IPU: 250000000 Hz
tx_isp: Initializing reserved memory
tx_isp: Reserved memory initialized:
Physical address: 0x02a80000
Virtual address: a2a80000
Size: 22544384 bytes
tx_isp: Validating memory setup:
ourISPdev = 81a9fc00
dma_addr = 0x02a80000
dma_buf = a2a80000
Memory regions:
Params: a2a81000
WDR: a2a82000
Sensor initialized with resolution 1920x1080 @ 30 fps
Starting tisp_init...
tisp-driver tisp-driver: tparams_day and tparams_night buffers allocated successfully
Loading parameters from file: /etc/sensor/sc2336-t31.bin
Copying 159736 bytes to destination
Copy successful
tisp-driver tisp-driver: Parameters loaded successfully from /etc/sensor/sc2336-t31.bin
tisp-driver tisp-driver: Applying isp_memopt settings
tisp-driver tisp-driver: tparams_day written to register successfully
Setting up ISP subdevices
Subdev setup complete:
sd=80a3e300 src_pad=80a3e000 sink_pad=80a3e980
Created device /dev/framechan0
Created device /dev/framechan1
Created device /dev/framechan2
tisp-driver tisp-driver: TISP device probed successfully
Platform device registered successfully
ISP driver loaded successfully
ISP device open called from pid 2126
Initializing frame source channel 0
Channel 0 initialized: 1920x1080, buffers=4 size=4147200 type_flags=0x0
Channel 0: value at 0x58 offset: 0x0
ISP opened: file=80956cf0 fs=81a9fd5e instance=80947e80 fd=6
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x77cdec80
file=80cf7a00 flags=0x2002
SC2336: ID = 0xcb3a (retry 0)
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f87a528
file=80cf7a00 flags=0x2002
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f87a528
file=80cf7a00 flags=0x2002
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7f87a580
file=80cf7a00 flags=0x2002
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7f87a578
file=80cf7a00 flags=0x2002
tx_isp: SET_BUF request: method=0x203a726f phys=0x33326373 size=0xdecd7a17
tx_isp: SET_BUF request: method=0x203a726f phys=0x33326373 size=0xdecd7a17
tx_isp: Magic allocation setup: phys=0x2a80000 virt=a2a80000 size=0x1580000
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7f87a578
file=80cf7a00 flags=0x2002
tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO
Buffer info before update: method=0x33326373 phys=0x3633 size=0
Buffer info after update:
method=0x1
phys=0x2a80000
virt=a2a80000
size=22544384
flags=0x1
tx_isp: Buffer setup completed successfully
ISP IOCTL called: cmd=0x40045626

=== IOCTL Debug ===
cmd=0x40045626 arg=0x7f87a590
file=80cf7a00 flags=0x2002
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
Sensor info request: returning success (1)
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=80cf7a00 flags=0x2002
Setting up buffers for 1920x1080 frame at base b3300000
Buffer setup complete:
Main buffer: 0x02a80000 size=2073600
Second buffer: 0x02c7a400 size=2073600
Third buffer: 0x02e74800 line_size=64
Register verification:
BUF0: addr=0x02a80000 size=0x00000780
BUF1: addr=0x00000000 size=0x00000780
Streaming hardware configured:
Resolution: 1920x1080
Buffer config: addr=0x2b894d8 size=4147200 count=4
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7f87a590
file=80cf7a00 flags=0x2002
Setting up video link 0
Configuring link:
sd=80a3e300 src=80a3e000 sink=80a3e980
src flags=0x2 sink flags=0x1
Video link 0 configured successfully
Video link setup complete, wrote back 0
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=80cf7a00 flags=0x2002
Configuring stream enable=1
Stream enabled successfully
Stream enabled, count=2
ISP IOCTL called: cmd=0x800456d3

=== IOCTL Debug ===
cmd=0x800456d3 arg=0x0
file=80cf7a00 flags=0x2002
Opened framechan0: fs=81a9fd5e base=a2b894d8
Frame channel IOCTL: cmd=0xc07056c3 arg=0x75df7828
ISP-DBG: Channel attr request:
enable=1 format=0x3231564e
size=1920x1080
crop=0 (0,0) 8x0
scale=0 0x0
pic=0x0
Channel 0 configured: 1920x1080 fmt=0x3231564e size=3110400 state=1
Frame Source State:
Format: NV12 (0x3231564e)
Resolution: 1920x1080
Buffer: size=3110400 count=4
State: 1 Flags: 0x1
DMA: addr=0x02b894d8 base=a2b894d8
Frame channel IOCTL: cmd=0xc0145608 arg=0x75df78f8
Set frame depth: channel=1 depth=1
Frame depth set: channel=1 depth=1 buf_size=3110400
Channel 0 configured: 1920x1080 fmt=0x3231564e size=3110400 state=1
DMA config: addr=0x02b894d8 base=a2b894d8 size=3110400 flags=0x1
Frame channel IOCTL: cmd=0x80045612 arg=0x75df7914
Stream ON request for channel 0
Channel 0 streaming started:
base=0x2b894d8 offset=0x0
buf_size=3110400 count=4
ctrl=0x1 start=0x1
Opened framechan1: fs=81a9fe06 base=a3b5b4d4
Frame channel IOCTL: cmd=0xc07056c3 arg=0x75c98828
No channel data
cgu_set_rate, parent = 1008000000, rate = 4096000, n = 7875, reg val = 0x22001ec3
codec_codec_ctl: set sample rate...
codec_codec_ctl: set device...
codec_set_device: set device: MIC...

=== ISP Release Debug ===
file=80cf7a00 flags=0x2002 fd=-1
codec_codec_ctl: set CODEC_TURN_OFF...
```