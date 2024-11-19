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
ISP Subsystem State:
Control: 0x00000000
Status:  0x00000000
Config:  0x00000000
ourISPdev allocated at 81fd3000
Registering platform device with resources
Registering platform driver
Starting ISP probe...
Initial control register: 0x00000000
Device list initialized: base=81fd3210 entry0=81fd3210 cb=80cb5300
Configuring ISP clocks using standard API
Clock rates after configuration:
  CSI: 125000000 Hz
  ISP Core: 250000000 Hz
  CGU ISP: 125000000 Hz
  IPU: 250000000 Hz
tx_isp: Reserved memory initialized:
  Physical address: 0x02a80000 (aligned)
  Virtual address: a2a80000
  Size: 22544384 bytes
  Alignment check: base=0x0 buf=0x0
Memory regions:
  Params: a2a81000
  WDR: a2a82000
Setting up I2C infrastructure for SC2336...
I2C infrastructure ready for SC2336 sensor at address 0x30
Starting tisp_driver_init...
tisp-driver tisp-driver: tparams_day and tparams_night buffers allocated successfully
Loading parameters from file: /etc/sensor/sc2336-t31.bin
Copying 159736 bytes to destination
Copy successful
tisp-driver tisp-driver: Parameters loaded successfully from /etc/sensor/sc2336-t31.bin
tisp-driver tisp-driver: Applying isp_memopt settings
tisp-driver tisp-driver: tparams_day written to register successfully
Starting ISP subsystem initialization
Initializing AE/AWB subsystems
Initializing image processing subsystems
Initializing event system
ISP subsystem initialization complete
Setting up ISP subdevices start
Allocated subdev state: 80c97300
Allocated source pad: 80c97d00
Allocated sink pad: 80c97d80
Subdev setup complete:
  sd=80c97300 src_pad=80c97d00 sink_pad=80c97d80
Creating frame channel devices...
Created device framechan0: major=251, minor=0
Created device framechan1: major=251, minor=1
Created device framechan2: major=251, minor=2
Frame channel devices created successfully
ISP probe completed successfully
ISP firmware process started
ISP driver loaded successfully
ISP device open called from pid 2408
Buffer calculations:
  RMEM base: 0x02a80000
  Base offset: 0x001094e0 (aligned from 0x1094d4)
  Width: 1920, Height: 1080
  Stride: 1920, Buffer size: 3110400
Final addresses (alignment check):
  DMA addr: 0x02b894e0 (mask: 0x0)
  Virtual: a2b894e0 (mask: 0x0)
  Buffer: stride=1920 size=3110400 count=4
ISP opened: file=80c1c7d0 fs=81fd32f5 instance=80bd5a00 fd=6
ISP IOCTL called: cmd=0x805056c1

=== IOCTL Debug ===
cmd=0x805056c1 arg=0x77160c80
file=808cab40 flags=0x2002 
SC2336: ID = 0xcb3a
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f979b28
file=808cab40 flags=0x2002 
Provided sensor info for index 0: sc2336
ISP IOCTL called: cmd=0xc050561a

=== IOCTL Debug ===
cmd=0xc050561a arg=0x7f979b28
file=808cab40 flags=0x2002 
ISP IOCTL called: cmd=0xc0045627

=== IOCTL Debug ===
cmd=0xc0045627 arg=0x7f979b80
file=808cab40 flags=0x2002 
Sensor command: 0xc0045627
Stored sensor name: sc2336
ISP IOCTL called: cmd=0x800856d5

=== IOCTL Debug ===
cmd=0x800856d5 arg=0x7f979b78
file=808cab40 flags=0x2002 
tx_isp: SET_BUF request: method=0x203a726f phys=0x33326373 size=0xfbc488f3
tx_isp: SET_BUF request: method=0x203a726f phys=0x33326373 size=0xfbc488f3
tx_isp: Magic allocation setup: phys=0x2a80000 virt=a2a80000 size=0x1580000
ISP IOCTL called: cmd=0x800856d4

=== IOCTL Debug ===
cmd=0x800856d4 arg=0x7f979b78
file=808cab40 flags=0x2002 
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
cmd=0x40045626 arg=0x7f979b90
file=808cab40 flags=0x2002 
ISP IOCTL called: cmd=VIDIOC_GET_SENSOR_INFO
Sensor info request: returning success (1)
ISP IOCTL called: cmd=0x80045612

=== IOCTL Debug ===
cmd=0x80045612 arg=0x0
file=808cab40 flags=0x2002 
Sensor command: 0x80045612
Sensor streamon
Preparing streaming configuration...
Setting up ISP NV12 buffers ch0:
  Dimensions: 1920x1080
  Y: addr=0x2b894e0 stride=1920
  UV: addr=0x2d838e0 stride=960
****>>>>> dump csi reg <<<<<****
VERSION = 00000000
N_LANES = 00000000
PHY_SHUTDOWNZ = 00000001
DPHY_RSTZ = 00000001
CSI2_RESETN = 00000001
PHY_STATE = 00000100
DATA_IDS_1 = 00000001
DATA_IDS_2 = 0000000c
ERR1 = 00ffffff
ERR2 = 00000000
MASK1 = 00000100
MASK2 = 00400040
PHY_TST_CTRL0 = 00000000
PHY_TST_CTRL1 = 00000000
ISP IOCTL called: cmd=0x800456d0

=== IOCTL Debug ===
cmd=0x800456d0 arg=0x7f979b90
file=808cab40 flags=0x2002 
Creating ISP links with enable=0
ISP IOCTL called: cmd=0x800456d2

=== IOCTL Debug ===
cmd=0x800456d2 arg=0x0
file=808cab40 flags=0x2002 
ISP state transition: 3 -> 4
Initial ctrl: 0x00000000
Stream enabled, count=2
ISP IOCTL called: cmd=0x800456d3

=== IOCTL Debug ===
cmd=0x800456d3 arg=0x0
file=808cab40 flags=0x2002 
Disabling ISP links
ISP IOCTL called: cmd=0x800456d1

=== IOCTL Debug ===
cmd=0x800456d1 arg=0x7f979b90
file=808cab40 flags=0x2002 
Destroying ISP links
Opened framechan0: fs=81fd32f5 base=a2b894e0
Frame channel IOCTL: cmd=0xc07056c3 arg=0x75279828
Channel attr request: enable=1 format=0x3231564e size=1920x1080
Channel 0 enabled: 1920x1080 fmt=0x3231564e size=3110400 state=2
Frame channel IOCTL: cmd=0xc0145608 arg=0x752798f8
Set frame depth: channel=1 depth=1
Frame depth set: channel=1 depth=1 buf_size=3110400
Channel 0 configured: 1920x1080 fmt=0x3231564e size=3110400 state=2
DMA config: addr=0x02b894e0 base=a2b894e0 size=3110400 flags=0x0
Frame channel IOCTL: cmd=0x80045612 arg=0x75279914
Stream ON request for channel 0
Channel 0 already streaming
Opened framechan1: fs=81fd33ad base=a3b5b4d4
Frame channel IOCTL: cmd=0xc07056c3 arg=0x7511a828
Channel attr request: enable=1 format=0x3231564e size=640x360
Channel 0 enabled: 640x360 fmt=0x3231564e size=345600 state=1
Frame channel IOCTL: cmd=0xc0145608 arg=0x7511a8f8
Set frame depth: channel=1 depth=1
Frame depth set: channel=1 depth=1 buf_size=345600
Channel 0 configured: 640x360 fmt=0x3231564e size=345600 state=1
DMA config: addr=0x03b5b4d4 base=a3b5b4d4 size=345600 flags=0x1
Frame channel IOCTL: cmd=0x80045612 arg=0x7511a914
Stream ON request for channel 0
Channel 0 streaming started:
  base=0x3b5b4d4 offset=0xfd2000
  buf_size=345600 count=4
  ctrl=0x1 start=0x1
cgu_set_rate, parent = 1008000000, rate = 4096000, n = 7875, reg val = 0x22001ec3
codec_codec_ctl: set sample rate...
codec_codec_ctl: set device...
codec_set_device: set device: MIC...

=== ISP Release Debug ===
file=808cab40 flags=0x2002 fd=-1
codec_codec_ctl: set CODEC_TURN_OFF...
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
