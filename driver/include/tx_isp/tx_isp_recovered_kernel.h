#ifndef TX_ISP_RECOVERED_KERNEL_H
#define TX_ISP_RECOVERED_KERNEL_H

/*
 * Kernel-tree compatibility glue shared by the monolithic recovered drivers.
 *
 * Include this after the Linux and architecture headers in a
 * REGTRACE_KERNEL_TREE_BUILD translation unit. Kernel-version-specific
 * includes and private_* wrapper ABIs deliberately remain in the SoC driver.
 */

#ifndef __regtrace_stringify
#define __regtrace_stringify_1(x) #x
#define __regtrace_stringify(x) __regtrace_stringify_1(x)
#endif
#ifndef str
#define str(x) __regtrace_stringify(x)
#endif

#ifndef REGTRACE_IOREMAP_UNCACHED
#ifdef _CACHE_UNCACHED
#define REGTRACE_IOREMAP_UNCACHED _CACHE_UNCACHED
#else
#define REGTRACE_IOREMAP_UNCACHED 0
#endif
#endif
#ifndef REGTRACE_IOREMAP_CACHED
#ifdef _CACHE_CACHABLE_NONCOHERENT
#define REGTRACE_IOREMAP_CACHED _CACHE_CACHABLE_NONCOHERENT
#else
#define REGTRACE_IOREMAP_CACHED 0
#endif
#endif
#ifndef IOREMAP_NORMAL
#define IOREMAP_NORMAL REGTRACE_IOREMAP_UNCACHED
#endif
#ifndef IOREMAP_NOCACHE
#define IOREMAP_NOCACHE REGTRACE_IOREMAP_UNCACHED
#endif
#ifndef IOREMAP_CACHE
#define IOREMAP_CACHE REGTRACE_IOREMAP_CACHED
#endif
#ifndef IOREMAP_WC
#define IOREMAP_WC REGTRACE_IOREMAP_UNCACHED
#endif
#ifndef IOREMAP_WT
#define IOREMAP_WT REGTRACE_IOREMAP_UNCACHED
#endif

extern int jzgpio_set_func(int port, int func, unsigned long pins, ...);

#if defined(__mips__) || defined(CONFIG_MIPS)
#ifndef c0_hwrena
#define c0_hwrena 7
#endif
#ifndef c0_status
#define c0_status 12
#endif
#ifndef mtc0
#define mtc0(val, reg) \
	__asm__ volatile ("mtc0 %0, $" str(reg) :: "r"(val) : "memory")
#endif
#ifndef mfc0
#define mfc0(reg) ({ \
	int __regtrace_mfc0_value; \
	__asm__ volatile ("mfc0 %0, $" str(reg) \
		: "=r"(__regtrace_mfc0_value)); \
	__regtrace_mfc0_value; \
})
#endif

static inline u32 regtrace_mips_rdhwr(unsigned int reg)
{
	u32 value = 0;

	if (reg == 4)
		__asm__ volatile ("rdhwr %0, $4" : "=r"(value));
	return value;
}

#ifndef __builtin_mips_rdhwr
#define __builtin_mips_rdhwr(reg) regtrace_mips_rdhwr(reg)
#endif

static inline void regtrace_write_c0_hwrena(u32 value)
{
	mtc0(value, c0_hwrena);
}

static inline void regtrace_write_c0_status(u32 value)
{
	mtc0(value, c0_status);
}

static inline u32 regtrace_read_c0_status(void)
{
	return (u32)mfc0(c0_status);
}

static inline u32 __wsbh(u32 value)
{
	return ((value & 0x00ff00ffU) << 8) |
	       ((value & 0xff00ff00U) >> 8);
}

#ifndef __ror
static inline u32 __ror(u32 value, unsigned int shift)
{
	shift &= 31U;
	return (value >> shift) | (value << ((32U - shift) & 31U));
}
#endif
#ifndef ror_d
#define ror_d(value, shift) __ror((u32)(value), (shift))
#endif
#ifndef read_c0_status
#define read_c0_status() regtrace_read_c0_status()
#endif
#ifndef write_c0_status
#define write_c0_status(value) regtrace_write_c0_status(value)
#endif
#ifndef read_c0_hwrena
#define read_c0_hwrena() ((u32)mfc0(c0_hwrena))
#endif
#ifndef write_c0_hwrena
#define write_c0_hwrena(value) regtrace_write_c0_hwrena(value)
#endif
#endif /* __mips__ || CONFIG_MIPS */

#ifndef REGTRACE_SYNC_IOB
#if defined(__mips__) || defined(CONFIG_MIPS)
#define REGTRACE_SYNC_IOB() \
	__asm__ volatile ("lui $3,0xa000\n\tsync\n\tlw $0,0($3)" \
			  ::: "memory", "$3")
#else
#define REGTRACE_SYNC_IOB() barrier()
#endif
#endif

static inline void _sync(void)
{
	REGTRACE_SYNC_IOB();
}

static inline void trap(int code)
{
	(void)code;
}

#define _hardwareRegister(reg) ((void)(reg), 0)
#define _setLLBit(value) ((void)(value))
#define _checkLLBit() (1)
#define get_gp() ((void *)current_thread_info())

typedef void *void_ptr;
typedef const char *const_char_ptr;
typedef char *char_ptr;
typedef void *void_iomem_ptr;

#endif /* TX_ISP_RECOVERED_KERNEL_H */
