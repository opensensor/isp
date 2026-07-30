#define _GNU_SOURCE

#include <dlfcn.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static int trace_fd = -1;

static void trace_log(const char *format, ...)
{
	char buffer[192];
	va_list ap;
	int length;

	if (trace_fd < 0)
		return;
	va_start(ap, format);
	length = vsnprintf(buffer, sizeof(buffer), format, ap);
	va_end(ap);
	if (length <= 0)
		return;
	if (length > (int)sizeof(buffer))
		length = sizeof(buffer);
	write(trace_fd, buffer, length);
}

__attribute__((constructor))
static void trace_init(void)
{
	trace_fd = open("/dev/kmsg", O_WRONLY | O_CLOEXEC);
	trace_log("t41-imp-trace: loaded pid=%d\n", getpid());
}

#define WRAP_ZERO_ARG_INIT(name) \
	int name(void) \
	{ \
		typedef int (*function_type)(void); \
		static function_type function; \
		int result; \
		trace_log("t41-imp-trace: " #name " enter\n"); \
		if (!function) \
			function = (function_type)dlsym(RTLD_NEXT, #name); \
		if (!function) { \
			trace_log("t41-imp-trace: " #name " dlsym failed\n"); \
			return -1; \
		} \
		result = function(); \
		trace_log("t41-imp-trace: " #name " exit ret=%d\n", result); \
		return result; \
	}

WRAP_ZERO_ARG_INIT(IMP_System_Init)
WRAP_ZERO_ARG_INIT(DsystemInit)
WRAP_ZERO_ARG_INIT(FrameSourceInit)
WRAP_ZERO_ARG_INIT(IVSInit)
WRAP_ZERO_ARG_INIT(OSDInit)
WRAP_ZERO_ARG_INIT(EncoderInit)
WRAP_ZERO_ARG_INIT(FBInit)

#define WRAP_ZERO_ARG_WORD(name) \
	uintptr_t name(void) \
	{ \
		typedef uintptr_t (*function_type)(void); \
		static function_type function; \
		uintptr_t result; \
		trace_log("t41-imp-trace: " #name " enter\n"); \
		if (!function) \
			function = (function_type)dlsym(RTLD_NEXT, #name); \
		if (!function) { \
			trace_log("t41-imp-trace: " #name " dlsym failed\n"); \
			return (uintptr_t)-1; \
		} \
		result = function(); \
		trace_log("t41-imp-trace: " #name " exit ret=0x%08x\n", \
			  (unsigned int)result); \
		return result; \
	}

#define WRAP_THREE_ARG_WORD(name) \
	uintptr_t name(uintptr_t arg0, uintptr_t arg1, uintptr_t arg2) \
	{ \
		typedef uintptr_t (*function_type)(uintptr_t, uintptr_t, uintptr_t); \
		static function_type function; \
		uintptr_t result; \
		trace_log("t41-imp-trace: " #name " enter a0=%p a1=0x%x " \
			  "a2=%p\n", (void *)arg0, (unsigned int)arg1, \
			  (void *)arg2); \
		if (!function) \
			function = (function_type)dlsym(RTLD_NEXT, #name); \
		if (!function) { \
			trace_log("t41-imp-trace: " #name " dlsym failed\n"); \
			return (uintptr_t)-1; \
		} \
		result = function(arg0, arg1, arg2); \
		trace_log("t41-imp-trace: " #name " exit ret=0x%08x\n", \
			  (unsigned int)result); \
		return result; \
	}

WRAP_THREE_ARG_WORD(IMP_Alloc)
WRAP_THREE_ARG_WORD(allocMem)
WRAP_THREE_ARG_WORD(alloc_device)
WRAP_ZERO_ARG_WORD(ivdc_mmap)
WRAP_ZERO_ARG_WORD(jpeg_mmap)
WRAP_ZERO_ARG_WORD(AL_Codec_Create)

uintptr_t buddy_alloc(uintptr_t size)
{
	typedef uintptr_t (*function_type)(uintptr_t);
	static function_type function;
	uintptr_t result;

	trace_log("t41-imp-trace: buddy_alloc enter size=0x%x\n",
		  (unsigned int)size);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "buddy_alloc");
	if (!function) {
		trace_log("t41-imp-trace: buddy_alloc dlsym failed\n");
		return 0;
	}
	result = function(size);
	trace_log("t41-imp-trace: buddy_alloc exit ret=0x%08x\n",
		  (unsigned int)result);
	return result;
}

int buddy_init(void *base, uintptr_t size)
{
	typedef int (*function_type)(void *, uintptr_t);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: buddy_init enter base=%p size=0x%x\n",
		  base, (unsigned int)size);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "buddy_init");
	if (!function)
		return -1;
	result = function(base, size);
	trace_log("t41-imp-trace: buddy_init exit ret=%d\n", result);
	return result;
}

int alloc_kmem_init(void *manager)
{
	typedef int (*function_type)(void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: alloc_kmem_init enter manager=%p\n", manager);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "alloc_kmem_init");
	if (!function)
		return -1;
	result = function(manager);
	trace_log("t41-imp-trace: alloc_kmem_init exit ret=%d\n", result);
	return result;
}

uintptr_t ivdc_regw(uintptr_t address, uintptr_t value)
{
	typedef uintptr_t (*function_type)(uintptr_t, uintptr_t);
	static function_type function;
	uintptr_t result;

	trace_log("t41-imp-trace: ivdc_regw enter addr=0x%08x value=0x%08x\n",
		  (unsigned int)address, (unsigned int)value);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "ivdc_regw");
	if (!function)
		return (uintptr_t)-1;
	result = function(address, value);
	trace_log("t41-imp-trace: ivdc_regw exit ret=0x%08x\n",
		  (unsigned int)result);
	return result;
}

int IMP_FrameSource_SetFrameDepth(int channel, int depth)
{
	typedef int (*function_type)(int, int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: SetFrameDepth enter ch=%d depth=%d\n",
		  channel, depth);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_FrameSource_SetFrameDepth");
	if (!function) {
		trace_log("t41-imp-trace: SetFrameDepth dlsym failed\n");
		return -1;
	}
	result = function(channel, depth);
	trace_log("t41-imp-trace: SetFrameDepth exit ret=%d\n", result);
	return result;
}

int IMP_FrameSource_SetFrameDepthCopyType(int channel, int copy_type)
{
	typedef int (*function_type)(int, int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: SetFrameDepthCopyType enter ch=%d type=%d\n",
		  channel, copy_type);
	if (!function)
		function = (function_type)dlsym(
			RTLD_NEXT, "IMP_FrameSource_SetFrameDepthCopyType");
	if (!function) {
		trace_log("t41-imp-trace: SetFrameDepthCopyType dlsym failed\n");
		return -1;
	}
	result = function(channel, copy_type);
	trace_log("t41-imp-trace: SetFrameDepthCopyType exit ret=%d\n", result);
	return result;
}

int IMP_FrameSource_CreateChn(int channel, const void *attributes)
{
	typedef int (*function_type)(int, const void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: CreateChn enter ch=%d attr=%p\n",
		  channel, attributes);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_FrameSource_CreateChn");
	if (!function) {
		trace_log("t41-imp-trace: CreateChn dlsym failed\n");
		return -1;
	}
	result = function(channel, attributes);
	trace_log("t41-imp-trace: CreateChn exit ret=%d\n", result);
	return result;
}

int IMP_Encoder_CreateGroup(int group)
{
	typedef int (*function_type)(int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: Encoder_CreateGroup enter grp=%d\n", group);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_Encoder_CreateGroup");
	if (!function) {
		trace_log("t41-imp-trace: Encoder_CreateGroup dlsym failed\n");
		return -1;
	}
	result = function(group);
	trace_log("t41-imp-trace: Encoder_CreateGroup exit ret=%d\n", result);
	return result;
}

int IMP_Encoder_SetDefaultParam(void *attributes, int profile, int rc_mode,
				uint16_t width, uint16_t height,
				uint32_t fps_num, uint32_t fps_den,
				uint32_t gop_length, int max_same_scene,
				int initial_qp, uint32_t bitrate)
{
	typedef int (*function_type)(void *, int, int, uint16_t, uint16_t,
				     uint32_t, uint32_t, uint32_t, int, int,
				     uint32_t);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: Encoder_SetDefaultParam enter %ux%u "
		  "profile=%d rc=%d fps=%u/%u gop=%u qp=%d bitrate=%u\n",
		  width, height, profile, rc_mode, fps_num, fps_den,
		  gop_length, initial_qp, bitrate);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_Encoder_SetDefaultParam");
	if (!function) {
		trace_log("t41-imp-trace: Encoder_SetDefaultParam dlsym failed\n");
		return -1;
	}
	result = function(attributes, profile, rc_mode, width, height, fps_num,
			  fps_den, gop_length, max_same_scene, initial_qp,
			  bitrate);
	trace_log("t41-imp-trace: Encoder_SetDefaultParam exit ret=%d\n",
		  result);
	return result;
}

int IMP_Encoder_CreateChn(int channel, const void *attributes)
{
	typedef int (*function_type)(int, const void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: Encoder_CreateChn enter ch=%d attr=%p\n",
		  channel, attributes);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_Encoder_CreateChn");
	if (!function) {
		trace_log("t41-imp-trace: Encoder_CreateChn dlsym failed\n");
		return -1;
	}
	result = function(channel, attributes);
	trace_log("t41-imp-trace: Encoder_CreateChn exit ret=%d\n", result);
	return result;
}

int IMP_Encoder_RegisterChn(int group, int channel)
{
	typedef int (*function_type)(int, int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: Encoder_RegisterChn enter grp=%d ch=%d\n",
		  group, channel);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_Encoder_RegisterChn");
	if (!function) {
		trace_log("t41-imp-trace: Encoder_RegisterChn dlsym failed\n");
		return -1;
	}
	result = function(group, channel);
	trace_log("t41-imp-trace: Encoder_RegisterChn exit ret=%d\n", result);
	return result;
}

int IMP_OSD_CreateGroup(int group)
{
	typedef int (*function_type)(int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: OSD_CreateGroup enter grp=%d\n", group);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "IMP_OSD_CreateGroup");
	if (!function) {
		trace_log("t41-imp-trace: OSD_CreateGroup dlsym failed\n");
		return -1;
	}
	result = function(group);
	trace_log("t41-imp-trace: OSD_CreateGroup exit ret=%d\n", result);
	return result;
}

int IMP_OSD_CreateRgn(void *attributes)
{
	typedef int (*function_type)(void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: OSD_CreateRgn enter attr=%p\n", attributes);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "IMP_OSD_CreateRgn");
	if (!function) {
		trace_log("t41-imp-trace: OSD_CreateRgn dlsym failed\n");
		return -1;
	}
	result = function(attributes);
	trace_log("t41-imp-trace: OSD_CreateRgn exit handle=%d\n", result);
	return result;
}

int IMP_OSD_RegisterRgn(int handle, int group, void *attributes)
{
	typedef int (*function_type)(int, int, void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: OSD_RegisterRgn enter handle=%d grp=%d\n",
		  handle, group);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "IMP_OSD_RegisterRgn");
	if (!function) {
		trace_log("t41-imp-trace: OSD_RegisterRgn dlsym failed\n");
		return -1;
	}
	result = function(handle, group, attributes);
	trace_log("t41-imp-trace: OSD_RegisterRgn exit ret=%d\n", result);
	return result;
}

int IMP_OSD_Start(int group)
{
	typedef int (*function_type)(int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: OSD_Start enter grp=%d\n", group);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "IMP_OSD_Start");
	if (!function) {
		trace_log("t41-imp-trace: OSD_Start dlsym failed\n");
		return -1;
	}
	result = function(group);
	trace_log("t41-imp-trace: OSD_Start exit ret=%d\n", result);
	return result;
}

int IMP_System_Bind(void *source, void *destination)
{
	typedef int (*function_type)(void *, void *);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: System_Bind enter src=%p dst=%p\n",
		  source, destination);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT, "IMP_System_Bind");
	if (!function) {
		trace_log("t41-imp-trace: System_Bind dlsym failed\n");
		return -1;
	}
	result = function(source, destination);
	trace_log("t41-imp-trace: System_Bind exit ret=%d\n", result);
	return result;
}

int IMP_FrameSource_EnableChn(int channel)
{
	typedef int (*function_type)(int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: FrameSource_EnableChn enter ch=%d\n", channel);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_FrameSource_EnableChn");
	if (!function) {
		trace_log("t41-imp-trace: FrameSource_EnableChn dlsym failed\n");
		return -1;
	}
	result = function(channel);
	trace_log("t41-imp-trace: FrameSource_EnableChn exit ret=%d\n", result);
	return result;
}

int IMP_Encoder_StartRecvPic(int channel)
{
	typedef int (*function_type)(int);
	static function_type function;
	int result;

	trace_log("t41-imp-trace: Encoder_StartRecvPic enter ch=%d\n", channel);
	if (!function)
		function = (function_type)dlsym(RTLD_NEXT,
					       "IMP_Encoder_StartRecvPic");
	if (!function) {
		trace_log("t41-imp-trace: Encoder_StartRecvPic dlsym failed\n");
		return -1;
	}
	result = function(channel);
	trace_log("t41-imp-trace: Encoder_StartRecvPic exit ret=%d\n", result);
	return result;
}
