#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <imp/imp_isp.h>
#include <imp/imp_framesource.h>
#include <imp/imp_osd.h>

extern int IMP_Alloc(void *allocation, unsigned int size, const char *name);
extern int DsystemInit(void);
extern int FrameSourceInit(void);
extern int IVSInit(void);
extern int OSDInit(void);
extern int EncoderInit(void);
extern int FBInit(void);
extern int modify_cache(void);
extern int g_ismainprocess;
extern unsigned int IMP_Virt_to_Phys(void *address);

struct opaque_allocation {
	/* libimp's private IMPAlloc object is 0x94 bytes on T41. */
	uint32_t words[64];
};

#ifndef T41_ALLOC_STAGE_DEFAULT
#define T41_ALLOC_STAGE_DEFAULT 0
#endif

#ifndef T41_COMPONENT_DEPTH_DEFAULT
#define T41_COMPONENT_DEPTH_DEFAULT 0
#endif

#ifndef T41_POST_ENABLE_US_DEFAULT
#define T41_POST_ENABLE_US_DEFAULT 100000
#endif

#ifndef T41_POST_LINK_US_DEFAULT
#define T41_POST_LINK_US_DEFAULT 250000
#endif

#ifndef T41_RUN_FRAME_DEFAULT
#define T41_RUN_FRAME_DEFAULT 0
#endif

#ifndef T41_PREALLOC_DEFAULT
#define T41_PREALLOC_DEFAULT 0
#endif

#ifndef T41_EARLY_LINK_DEFAULT
#define T41_EARLY_LINK_DEFAULT 0
#endif

#ifndef T41_LINK_AFTER_ENABLE_DEFAULT
#define T41_LINK_AFTER_ENABLE_DEFAULT 0
#endif

#ifndef T41_LINK_OPTIONAL_DEFAULT
#define T41_LINK_OPTIONAL_DEFAULT 0
#endif

#ifndef T41_SKIP_LINK_ACTIVATION_DEFAULT
#define T41_SKIP_LINK_ACTIVATION_DEFAULT 0
#endif

#ifndef T41_MODIFY_CACHE_DEFAULT
#define T41_MODIFY_CACHE_DEFAULT 1
#endif

#ifndef T41_DIRECT_FRAME_DEFAULT
#define T41_DIRECT_FRAME_DEFAULT 0
#endif

#ifndef T41_FREEZE_BEFORE_SAVE_DEFAULT
#define T41_FREEZE_BEFORE_SAVE_DEFAULT 0
#endif

#ifndef T41_HARDWARE_FRAMES_DEFAULT
#define T41_HARDWARE_FRAMES_DEFAULT 1
#endif

static int kmsg_fd = -1;

static void sleep_us_uninterrupted(unsigned int usec)
{
	struct timespec delay;

	delay.tv_sec = usec / 1000000U;
	delay.tv_nsec = (long)(usec % 1000000U) * 1000L;
	while (nanosleep(&delay, &delay) < 0 && errno == EINTR)
		;
}

static int freeze_frame_channel(void)
{
	unsigned int type = 1;
	int fd;
	int result;

	fd = open("/dev/framechan0", O_RDWR);
	if (fd < 0)
		return -errno;
	result = ioctl(fd, 0xc0045458U, &type);
	if (result < 0)
		result = -errno;
	close(fd);
	return result;
}

static void report(const char *stage, int result,
		   const struct opaque_allocation *allocation)
{
	char line[192];
	int length;

	length = snprintf(line, sizeof(line),
			  "t41-alloc-probe: %s ret=%d words=%08x/%08x/%08x/%08x "
			  "errno=%d\n", stage, result,
			  allocation ? allocation->words[0] : 0,
			  allocation ? allocation->words[1] : 0,
			  allocation ? allocation->words[2] : 0,
			  allocation ? allocation->words[3] : 0, errno);
	if (length > 0) {
		(void)write(STDERR_FILENO, line, (size_t)length);
		if (kmsg_fd >= 0)
			(void)write(kmsg_fd, line, (size_t)length);
	}
}

static void inspect_frame(const char *stage, const IMPFrameInfo *frame,
			  int save_image)
{
	const uint8_t *pixels;
	uint64_t y_sum = 0;
	uint64_t uv_sum = 0;
	uint32_t hash = 2166136261U;
	uint32_t y_size;
	uint32_t i;
	uint32_t nonzero = 0;
	uint8_t minimum = 0xff;
	uint8_t maximum = 0;
	char line[320];
	int length;

	if (!frame) {
		report(stage, -EINVAL, NULL);
		return;
	}
	pixels = (const uint8_t *)(uintptr_t)frame->virAddr;
	y_size = frame->width * frame->height;
	length = snprintf(line, sizeof(line),
			  "t41-alloc-probe: %s index=%d pool=%d fmt=%u "
			  "size=%u phys=%08x virt=%08x direct=%08x ts=%lld\n",
			  stage, frame->index, frame->pool_idx, frame->pixfmt,
			  frame->size, frame->phyAddr, frame->virAddr,
			  frame->direct_phyAddr, (long long)frame->timeStamp);
	if (length > 0) {
		(void)write(STDERR_FILENO, line, (size_t)length);
		if (kmsg_fd >= 0)
			(void)write(kmsg_fd, line, (size_t)length);
	}
	if (!pixels || frame->size == 0 || y_size >= frame->size) {
		report("frame-memory-invalid", -EINVAL, NULL);
		return;
	}
	for (i = 0; i < frame->size; i++) {
		uint8_t value = pixels[i];

		hash = (hash ^ value) * 16777619U;
		nonzero += value != 0;
		if (value < minimum)
			minimum = value;
		if (value > maximum)
			maximum = value;
		if (i < y_size)
			y_sum += value;
		else
			uv_sum += value;
	}
	length = snprintf(line, sizeof(line),
			  "t41-alloc-probe: %s-content hash=%08x min=%u max=%u "
			  "nonzero=%u/%u y-mean-x100=%llu uv-mean-x100=%llu\n",
			  stage, hash, minimum, maximum, nonzero, frame->size,
			  (unsigned long long)(y_sum * 100 / y_size),
			  (unsigned long long)(uv_sum * 100 /
				(frame->size - y_size)));
	if (length > 0) {
		(void)write(STDERR_FILENO, line, (size_t)length);
		if (kmsg_fd >= 0)
			(void)write(kmsg_fd, line, (size_t)length);
	}
	if (save_image) {
		int fd = open("/tmp/t41-frame.nv12",
			      O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
		uint32_t written = 0;

		if (fd < 0) {
			report("frame-save-open", -errno, NULL);
			return;
		}
		while (written < frame->size) {
			ssize_t count = write(fd, pixels + written,
					      frame->size - written);

			if (count <= 0) {
				report("frame-save-write", -errno, NULL);
				break;
			}
			written += (uint32_t)count;
		}
		close(fd);
		report("frame-save-bytes", (int)written, NULL);
	}
}

static int allocate_one(struct opaque_allocation *allocation,
			unsigned int size, const char *name)
{
	int result;

	errno = 0;
	report(name, 0x7fffffff, allocation);
	result = IMP_Alloc(allocation, size, name);
	report(name, result, allocation);
	return result;
}

static int set_driver_parameter(const char *name, const char *value)
{
	char parameter[192];
	ssize_t value_length = (ssize_t)strlen(value);
	int parameter_fd;

	snprintf(parameter, sizeof(parameter),
		 "/sys/module/tx_isp_t41_recovered/parameters/%s", name);
	parameter_fd = open(parameter, O_WRONLY | O_CLOEXEC);
	if (parameter_fd < 0)
		return -errno;
	if (write(parameter_fd, value, (size_t)value_length) != value_length) {
		int saved_errno = errno;

		close(parameter_fd);
		return saved_errno ? -saved_errno : -EIO;
	}
	close(parameter_fd);
	return 0;
}

static int enable_video_link_writes(void)
{
	int result;

	result = set_driver_parameter("t41_video_link_dry_run", "0\n");
	if (result)
		return result;
	return set_driver_parameter("t41_defer_video_link_setup", "0\n");
}

static int activate_deferred_video_link(void)
{
	char fd_path[64];
	char target[128];
	int32_t link[2] = {1, 0};
	int fd;

	fd = enable_video_link_writes();
	if (fd)
		return fd;

	/* IMP_ISP_Open already owns the graph's misc fd.  Reuse it so this
	 * timing probe does not invoke the driver's graph open path twice. */
	for (fd = 0; fd < 128; fd++) {
		ssize_t length;

		snprintf(fd_path, sizeof(fd_path), "/proc/self/fd/%d", fd);
		length = readlink(fd_path, target, sizeof(target) - 1);
		if (length < 0)
			continue;
		target[length] = '\0';
		if (!strcmp(target, "/dev/tx-isp")) {
			report("late-video-link-fd", fd, NULL);
			if (ioctl(fd, 0x80085409UL, link) < 0)
				return -errno;
			return 0;
		}
	}

	return -ENODEV;
}

static int prepare_isp(int enable)
{
	IMPSensorInfo sensor;
	int result;

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
	report("ISP_Open", result, NULL);
	if (result)
		return result;
	result = IMP_ISP_AddSensor(IMPVI_MAIN, &sensor);
	report("ISP_AddSensor", result, NULL);
	if (result || !enable)
		return result;
	result = IMP_ISP_EnableSensor(IMPVI_MAIN, &sensor);
	report("ISP_EnableSensor", result, NULL);
	return result;
}

static int run_direct_frame_probe(void)
{
	struct opaque_allocation frame_allocation = {{0}};
	struct opaque_allocation diagnostic = {{0}};
	uint32_t format[0x74 / sizeof(uint32_t)] = {0};
	uint32_t request[5] = {2, 1, 2, 0, 0};
	uint32_t buffer[17] = {0};
	uint32_t type = 1;
	void *frames;
	uint32_t physical;
	int fd;
	int result;
	unsigned int index;

	/* Match libimp's normal channel-0 layout with one contiguous allocation,
	 * but avoid video_vbm_malloc: that wrapper waits for FrameSourceInit.  The
	 * generic allocator stores its mapped address in the private IMPAlloc
	 * object; locate that mapped member and translate it with the allocator's
	 * exported V2P operation. */
	result = allocate_one(&frame_allocation, 0xa8c000,
			      "direct-frame-vbm");
	if (result)
		return 27;
	frames = NULL;
	physical = 0;
	for (index = 0; index < sizeof(frame_allocation.words) /
				       sizeof(frame_allocation.words[0]); index++) {
		uint32_t candidate = frame_allocation.words[index];
		uint32_t candidate_physical;

		if (candidate < 0x70000000U || candidate >= 0x80000000U)
			continue;
		candidate_physical = IMP_Virt_to_Phys(
			(void *)(uintptr_t)candidate);
		diagnostic.words[0] = index;
		diagnostic.words[1] = candidate;
		diagnostic.words[2] = candidate_physical;
		report("direct-vbm-candidate", 0, &diagnostic);
		if (!physical && candidate_physical) {
			frames = (void *)(uintptr_t)candidate;
			physical = candidate_physical;
		}
	}
	diagnostic.words[0] = (uint32_t)(uintptr_t)frames;
	diagnostic.words[1] = physical;
	report("direct-vbm", frames && physical ? 0 : -ENOMEM, &diagnostic);
	if (!frames || !physical)
		return 34;

	result = activate_deferred_video_link();
	report("direct-video-link", result, NULL);
	if (result)
		return 28;
	fd = open("/dev/framechan0", O_RDWR | O_CLOEXEC);
	report("direct-frame-open", fd, NULL);
	if (fd < 0)
		return 29;

	format[0] = 1;
	format[1] = 2560;
	format[2] = 1440;
	format[3] = 0; /* T41 vendor NV12 enum. */
	format[4] = 4;
	format[5] = 2560;
	format[6] = 0x546000;
	format[7] = 8;
	result = ioctl(fd, 0xc0745451UL, format);
	report("direct-set-fmt", result < 0 ? -errno : result, NULL);
	if (result < 0)
		return 30;
	result = ioctl(fd, 0xc0145453UL, request);
	report("direct-reqbufs", result < 0 ? -errno : result, NULL);
	if (result < 0 || request[0] != 2)
		return 31;
	for (index = 0; index < 2; index++) {
		memset(buffer, 0, sizeof(buffer));
		buffer[0] = index;
		buffer[1] = 1;
		buffer[12] = 2;
		buffer[13] = physical + index * 0x546000U;
		buffer[14] = 0x546000;
		result = ioctl(fd, 0xc0445455UL, buffer);
		diagnostic.words[0] = index;
		diagnostic.words[1] = buffer[13];
		report("direct-qbuf", result < 0 ? -errno : result, &diagnostic);
		if (result < 0)
			return 32;
	}
	result = ioctl(fd, 0xc0045457UL, &type);
	report("direct-streamon", result < 0 ? -errno : result, NULL);
	if (result < 0)
		return 33;
	usleep(250000);
	return 0;
}

int main(int argc, char **argv)
{
	struct opaque_allocation osd0 = {{0}};
	struct opaque_allocation osd1 = {{0}};
	struct opaque_allocation encoder = {{0}};
	struct opaque_allocation early = {{0}};
	IMPFSChnAttr channel;
	IMPFrameInfo *frame = NULL;
	int result;
	int depth_result = 0;
	unsigned int hardware_frame;
	int isp_stage = T41_ALLOC_STAGE_DEFAULT;
	int component_depth = T41_COMPONENT_DEPTH_DEFAULT;
	int set_osd_pool = 0;
	int set_tuning_pool = 0;

	kmsg_fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
	report("start", 0, NULL);
	if (isp_stage == 3)
		set_osd_pool = 1;
	else if (isp_stage == 4)
		set_tuning_pool = 1;
	else if (isp_stage == 5) {
		set_osd_pool = 1;
		set_tuning_pool = 1;
	}
	if (argc > 1 && !strcmp(argv[1], "add"))
		isp_stage = 1;
	else if (argc > 1 && !strcmp(argv[1], "enable"))
		isp_stage = 2;
	else if (argc > 1 && !strcmp(argv[1], "pool-osd")) {
		isp_stage = 2;
		set_osd_pool = 1;
	} else if (argc > 1 && !strcmp(argv[1], "pool-tuning")) {
		isp_stage = 2;
		set_tuning_pool = 1;
	} else if (argc > 1 && !strcmp(argv[1], "pools")) {
		isp_stage = 2;
		set_osd_pool = 1;
		set_tuning_pool = 1;
	}
	else if (argc > 1) {
		report("usage: [add|enable|pool-osd|pool-tuning|pools]",
		       -EINVAL, NULL);
		return 2;
	}
	if (T41_PREALLOC_DEFAULT) {
		/* The ISP graph and TISP state can touch the reserved-memory window.
		 * Establish libimp's mapping/allocator before enabling the sensor so
		 * its one-time full-window initialization cannot race those users. */
		result = allocate_one(&early, 4096, "pre-isp-rmem-init");
		if (result)
			return 22;
		if (T41_RUN_FRAME_DEFAULT && T41_EARLY_LINK_DEFAULT) {
			result = enable_video_link_writes();
			report("early-video-link-enable", result, NULL);
			if (result)
				return 23;
		}
	}
	if (isp_stage) {
		result = prepare_isp(isp_stage >= 2);
		if (result)
			return 3;
		if (T41_POST_ENABLE_US_DEFAULT)
			sleep_us_uninterrupted(T41_POST_ENABLE_US_DEFAULT);
	}
	if (T41_DIRECT_FRAME_DEFAULT) {
		result = run_direct_frame_probe();
		_exit(result);
	}
	if (set_osd_pool) {
		result = IMP_OSD_SetPoolSize(512 * 1024);
		report("OSD_SetPoolSize", result, NULL);
		if (result)
			return 4;
	}
	if (set_tuning_pool) {
		result = IMP_ISP_Tuning_SetOsdPoolSize(512 * 1024);
		report("Tuning_SetOsdPoolSize", result, NULL);
		if (result)
			return 5;
	}
	if (component_depth >= 1) {
		result = DsystemInit();
		report("DsystemInit", result, NULL);
		if (result)
			return 6;
	}
	if (component_depth >= 2) {
		result = FrameSourceInit();
		report("FrameSourceInit", result, NULL);
		if (result)
			return 7;
	}
	if (component_depth >= 3) {
		result = IVSInit();
		report("IVSInit", result, NULL);
		if (result)
			return 8;
	}
	if (component_depth >= 4) {
		result = OSDInit();
		report("OSDInit", result, NULL);
		if (result)
			return 9;
	}
	if (component_depth >= 5) {
		result = EncoderInit();
		report("EncoderInit", result, NULL);
		if (result)
			return 13;
		result = FBInit();
		report("FBInit", result, NULL);
		if (result)
			return 14;
		if (T41_MODIFY_CACHE_DEFAULT) {
			result = modify_cache();
			report("modify_cache", result, NULL);
			if (result)
				return 20;
		} else {
			report("modify_cache-skipped", 0, NULL);
		}
		g_ismainprocess = 1;
		report("g_ismainprocess", g_ismainprocess, NULL);
	}
	if (T41_RUN_FRAME_DEFAULT) {
		if (!T41_SKIP_LINK_ACTIVATION_DEFAULT &&
		    !T41_LINK_AFTER_ENABLE_DEFAULT) {
			result = activate_deferred_video_link();
			report("late-video-link", result, NULL);
			if (result)
				return 21;
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
		report("FrameSource_CreateChn", result, NULL);
		if (result)
			return 15;
		result = IMP_FrameSource_SetChnAttr(0, &channel);
		report("FrameSource_SetChnAttr", result, NULL);
		if (result)
			return 16;
		result = IMP_FrameSource_EnableChn(0);
		report("FrameSource_EnableChn", result, NULL);
		if (result)
			return 18;
		/*
		 * T41 libimp records a newly-created channel as state 1 and rejects a
		 * positive frame depth with IMP_ERR_FS_CHN_NOT_ENABLE until EnableChn
		 * advances it to state 2.  Request the consumer FIFO only after that
		 * transition so this probe reaches GetFrame instead of mistaking its
		 * own call order for a kernel-driver failure.
		 */
		depth_result = IMP_FrameSource_SetFrameDepth(0, 1);
		report("FrameSource_SetFrameDepth", depth_result, NULL);
		if (!T41_SKIP_LINK_ACTIVATION_DEFAULT &&
		    T41_LINK_AFTER_ENABLE_DEFAULT) {
			result = activate_deferred_video_link();
			report("post-enable-video-link", result, NULL);
			if (result && !T41_LINK_OPTIONAL_DEFAULT)
				return 24;
		}
		if (depth_result) {
			if (T41_LINK_AFTER_ENABLE_DEFAULT) {
				sleep_us_uninterrupted(T41_POST_LINK_US_DEFAULT);
				report("post-link-observation-complete", 0, NULL);
			}
			_exit(0);
		}
		result = IMP_FrameSource_GetFrame(0, &frame);
		report("FrameSource_GetFrame", result,
		       (const struct opaque_allocation *)(const void *)frame);
		if (result || !frame)
			_exit(19);
		inspect_frame("FrameSource_GetFrame", frame, 0);
		result = IMP_FrameSource_ReleaseFrame(0, frame);
		report("FrameSource_ReleaseFrame", result, NULL);
		if (result)
			_exit(20);

		/* The first userspace FIFO entry is the seed whose release requeues
		 * the vb2 buffer and starts the deferred capture graph.  Every later
		 * acquisition must therefore come from an actual hardware completion.
		 * Keep the historical one-second gap before the first one: it also
		 * verifies that a live QBUF can re-arm a FIFO drained during startup. */
		sleep_us_uninterrupted(1000000);
		for (hardware_frame = 0;
		     hardware_frame < T41_HARDWARE_FRAMES_DEFAULT;
		     hardware_frame++) {
			frame = NULL;
			result = IMP_FrameSource_GetFrame(0, &frame);
			report("FrameSource_GetFrame-hardware", result,
			       (const struct opaque_allocation *)(const void *)frame);
			if (result || !frame)
				break;

			if (hardware_frame + 1 < T41_HARDWARE_FRAMES_DEFAULT) {
				report("FrameSource_ReleaseFrame-hardware",
				       IMP_FrameSource_ReleaseFrame(0, frame), NULL);
				continue;
			}
			if (T41_FREEZE_BEFORE_SAVE_DEFAULT) {
				int release_result;
				int streamoff_result;
				int freeze_result;

				release_result = IMP_FrameSource_ReleaseFrame(0, frame);
				report("FrameSource_ReleaseFrame-before-freeze",
				       release_result, NULL);
				streamoff_result = freeze_frame_channel();
				report("framechan-STREAMOFF-before-save",
				       streamoff_result, NULL);
				freeze_result = IMP_FrameSource_DisableChn(0);
				report("FrameSource_DisableChn-before-save",
				       freeze_result, NULL);
				sleep_us_uninterrupted(100000);
			}
			inspect_frame("FrameSource_GetFrame-hardware", frame, 1);
			if (!T41_FREEZE_BEFORE_SAVE_DEFAULT)
				report("FrameSource_ReleaseFrame-hardware",
				       IMP_FrameSource_ReleaseFrame(0, frame), NULL);
		}
		_exit(result ? 21 : 0);
	}
	result = allocate_one(&osd0, 0x80000, "probe-osd0");
	if (result)
		return 10;
	result = allocate_one(&osd1, 0x80000, "probe-osd1");
	if (result)
		return 11;
	result = allocate_one(&encoder, 0x1fa400, "probe-encoder");
	if (result)
		return 12;
	report("complete", 0, &encoder);
	/* Leave the loaded ISP graph intact for the harness to inspect/reboot. */
	if (isp_stage)
		_exit(0);
	return 0;
}
