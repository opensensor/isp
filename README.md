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
cd isp-driver
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
This driver is incomplete but works to a certain degree and 
does enable and communicate with the sensor and attempt to setup framebuffers.

There is a crash after the first stream on ioctl cmd, that we are currently debugging.

Current progress:

```angular2html
Loading ISP driver...
gISPdev allocated at 81b8bc00
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
  gISPdev = 81b8bc00
  dma_addr = 0x02a80000
  dma_buf = a2a80000
Memory regions:
  Params: a2a81000
  WDR: a2a82000
Starting tisp_init...
tisp-driver tisp-driver: tparams_day and tparams_night buffers allocated successfully
Loading parameters from file: /etc/sensor/sc2336-t31.bin
Copying 159736 bytes to destination
Copy successful
tisp-driver tisp-driver: Parameters loaded successfully from /etc/sensor/sc2336-t31.bin
tisp-driver tisp-driver: Applying isp_memopt settings
tisp-driver tisp-driver: tparams_day written to register successfully
Registering child devices
tisp-driver tisp-driver: Successfully registered 8 devices
Creating ISP graph with 8 devices
ISP graph creation completed successfully
Created device /dev/framechan0
Created device /dev/framechan1
Created device /dev/framechan2
tisp-driver tisp-driver: TISP device probed successfully
Platform device registered successfully
ISP driver loaded successfully
ISP device open called from pid 2157
ISP device open: file=80ce57d0 f_flags=0x82051400
Stored fd -2113596416 at offset 0x20
Initialized frame source 0 with default 1920x1080 buf_size=4147200
Set file->private_data to gISPdev=81b8bc00
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x774b0c80
file=820e7960 flags=0x2002 private_data=  (null)
SC2336: ID = 0xcb3a (retry 0)
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7faa4908
file=820e7960 flags=0x2002 private_data=  (null)
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7faa4908
file=820e7960 flags=0x2002 private_data=  (null)
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7faa4960
file=820e7960 flags=0x2002 private_data=  (null)
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7faa4958
file=820e7960 flags=0x2002 private_data=  (null)
tx_isp: SET_BUF request: method=0x203a726f phys=0x33326373 size=0xcab99f17
tx_isp: Magic allocation setup: phys=0x2a80000 virt=a2a80000 size=0x1580000
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7faa4958
file=820e7960 flags=0x2002 private_data=  (null)
tx_isp: Handling ioctl VIDIOC_SET_BUF_INFO
tx_isp: Buffer info configured: phys=0x2a80000 virt=a2a80000 size=22544384
tx_isp: Buffer setup completed successfully
ISP IOCTL called: cmd=0x40045626

=== IOCTL Debug ===
cmd=0x40045626 arg=0x7faa4970
file=820e7960 flags=0x2002 private_data=  (null)
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=820e7960 flags=0x2002 private_data=  (null)
Stream ON requested
Frame source setup: dev=81b8bc00 channel=0
dev offset 0x20=0x0
Frame source channel 0 initialized:
  buffer_base: a2b894d4
  dma_addr: 0x2b894d4
  buf_size: 4147200 x 4 buffers
Started streaming on channel 0 with:
  DMA addr: 0x02a80000
  Buffer size: 22544384
  State flags: 0x2
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7faa4970
file=820e7960 flags=0x2002 private_data=  (null)
TX_ISP_VIDEO_LINK_SETUP
Setting up video link 0
Pad flags: src=0x3 sink=0x3 link=0x3
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=820e7960 flags=0x2002 private_data=  (null)
Stream enable returned 0
do_page_fault() #2: sending SIGSEGV to prudynt for invalid write access to
00000000 
epc = 774e0dcc in prudynt[774b4000+5e000]
ra  = 774e29c8 in prudynt[774b4000+5e000]
```