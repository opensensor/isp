# Open-Source ISP Driver for Ingenic T31 (SC2336)

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

