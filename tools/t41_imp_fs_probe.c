#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <imp/imp_osd.h>
#include <imp/imp_system.h>

#define T41_PROBE_RUN_SYSTEM_INIT 0
#define T41_PROBE_RUN_MANUAL_SYSTEM_INIT 1
#define T41_PROBE_RUN_TUNING 0
#define T41_PROBE_PREWARM_ALLOC 0
#define T41_PROBE_USE_ALARM 0

extern int IMP_Alloc(void *allocation, unsigned int size, const char *name);
extern int DsystemInit(void);
extern int FrameSourceInit(void);
extern int IVSInit(void);
extern int OSDInit(void);
extern int EncoderInit(void);
extern int FBInit(void);

struct opaque_allocation {
	uint32_t words[64];
};

static int kmsg_fd = -1;

static void checkpoint(const char *name, int result)
{
	fprintf(stderr, "t41-imp-fs-probe: %s ret=%d (0x%08x)\n",
		name, result, (unsigned int)result);
	fflush(stderr);
	if (kmsg_fd >= 0)
		dprintf(kmsg_fd, "t41-imp-fs-probe: %s ret=%d (0x%08x)\n",
			name, result, (unsigned int)result);
}

static void timeout_handler(int signal_number)
{
	(void)signal_number;
	checkpoint("alarm-timeout", -1);
	_exit(124);
}

int main(void)
{
	IMPSensorInfo sensor;
	IMPFSChnAttr channel;
	IMPFrameInfo *frame = NULL;
	struct opaque_allocation prewarm = {{0}};
	int depth_result;
	int result;

	if (T41_PROBE_USE_ALARM) {
		signal(SIGALRM, timeout_handler);
		alarm(20);
	}
	kmsg_fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);

	memset(&sensor, 0, sizeof(sensor));
	strncpy(sensor.name, "os04d10", sizeof(sensor.name) - 1);
	sensor.cbus_type = TX_SENSOR_CONTROL_INTERFACE_I2C;
	strncpy(sensor.i2c.type, "os04d10", sizeof(sensor.i2c.type) - 1);
	sensor.i2c.addr = 0x3c;
	sensor.i2c.i2c_adapter_id = 0;
	sensor.rst_gpio = -1;
	sensor.pwdn_gpio = -1;
	sensor.power_gpio = -1;
	sensor.sensor_id = 0;
	sensor.video_interface = IMPISP_SENSOR_VI_MIPI_CSI0;
	sensor.mclk = IMPISP_SENSOR_MCLK1;
	sensor.default_boot = 0;

	result = IMP_ISP_Open();
	checkpoint("ISP_Open", result);
	if (result)
		return 1;
	result = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor);
	checkpoint("ISP_AddSensor", result);
	if (result)
		return 1;
	result = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor);
	checkpoint("ISP_EnableSensor", result);
	if (result)
		return 1;

	result = IMP_OSD_SetPoolSize(512 * 1024);
	checkpoint("OSD_SetPoolSize", result);
	result = IMP_ISP_Tuning_SetOsdPoolSize(512 * 1024);
	checkpoint("ISP_SetOsdPoolSize", result);
	if (T41_PROBE_PREWARM_ALLOC) {
		result = IMP_Alloc(&prewarm, 4096, "t41-prewarm");
		checkpoint("Alloc_Prewarm", result);
		if (result)
			return 1;
	}
	if (T41_PROBE_RUN_MANUAL_SYSTEM_INIT) {
		result = DsystemInit();
		checkpoint("DsystemInit", result);
		if (result)
			return 1;
		result = FrameSourceInit();
		checkpoint("FrameSourceInit", result);
		if (result)
			return 1;
		result = IVSInit();
		checkpoint("IVSInit", result);
		if (result)
			return 1;
		result = OSDInit();
		checkpoint("OSDInit", result);
		if (result)
			return 1;
		result = EncoderInit();
		checkpoint("EncoderInit", result);
		if (result)
			return 1;
		result = FBInit();
		checkpoint("FBInit", result);
		if (result)
			return 1;
	}
	if (T41_PROBE_RUN_SYSTEM_INIT) {
		result = IMP_System_Init();
		checkpoint("System_Init", result);
		if (result)
			return 1;
	} else {
		checkpoint("System_Init skipped", 0);
	}
	if (T41_PROBE_RUN_TUNING) {
		result = IMP_ISP_EnableTuning();
		checkpoint("ISP_EnableTuning", result);
		if (result)
			return 1;
	} else {
		checkpoint("ISP_EnableTuning skipped", 0);
	}

	memset(&channel, 0, sizeof(channel));
	channel.picWidth = 2560;
	channel.picHeight = 1440;
	channel.pixFmt = PIX_FMT_NV12;
	channel.outFrmRateNum = 25;
	channel.outFrmRateDen = 1;
	channel.nrVBs = 2;
	channel.type = FS_PHY_CHANNEL;

	result = IMP_FrameSource_CreateChn(0, &channel);
	checkpoint("FrameSource_CreateChn", result);
	if (result)
		return 1;
	result = IMP_FrameSource_SetChnAttr(0, &channel);
	checkpoint("FrameSource_SetChnAttr", result);
	if (result)
		return 1;
	depth_result = IMP_FrameSource_SetFrameDepth(0, 1);
	checkpoint("FrameSource_SetFrameDepth", depth_result);
	result = IMP_FrameSource_EnableChn(0);
	checkpoint("FrameSource_EnableChn", result);
	if (result)
		return 1;

	if (!depth_result) {
		result = IMP_FrameSource_GetFrame(0, &frame);
		checkpoint("FrameSource_GetFrame", result);
	}
	if (!depth_result && !result && frame) {
		fprintf(stderr,
			"t41-imp-fs-probe: frame index=%d pool=%d %ux%u "
			"pixfmt=%u size=%u phys=0x%08x virt=0x%08x "
			"direct=0x%08x timestamp=%lld\n",
			frame->index, frame->pool_idx, frame->width, frame->height,
			frame->pixfmt, frame->size, frame->phyAddr, frame->virAddr,
			frame->direct_phyAddr, (long long)frame->timeStamp);
		fflush(stderr);
		checkpoint("FrameSource_ReleaseFrame",
			   IMP_FrameSource_ReleaseFrame(0, frame));
	}

	checkpoint("FrameSource_DisableChn", IMP_FrameSource_DisableChn(0));
	checkpoint("FrameSource_DestroyChn", IMP_FrameSource_DestroyChn(0));
	if (T41_PROBE_RUN_TUNING)
		checkpoint("ISP_DisableTuning", IMP_ISP_DisableTuning());
	if (T41_PROBE_RUN_SYSTEM_INIT)
		checkpoint("System_Exit", IMP_System_Exit());
	checkpoint("ISP_DisableSensor", IMP_ISP_DisableSensor(IMPVI_MAIN));
	checkpoint("ISP_DelSensor", IMP_ISP_DelSensor(IMPVI_MAIN, &sensor));
	checkpoint("ISP_Close", IMP_ISP_Close());
	if (T41_PROBE_USE_ALARM)
		alarm(0);
	return result ? 1 : 0;
}
