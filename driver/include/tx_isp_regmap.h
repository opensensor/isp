#pragma once
/* TX-ISP T31 Register Map (ADR/DRC and YDNS) - OEM-aligned constants
 * Derived from Binary Ninja HLIL of tx-isp-t31.ko.
 *
 * Address units are absolute system_reg_write addresses.
 * Packing for most LUT writes: 32-bit word = (hi16 << 16) | (lo16).
 */

/* Helper: pack two 16-bit values into one 32-bit word (hi:lo) */
#ifndef PACK16_U32
#define PACK16_U32(hi, lo) ((((unsigned int)(hi) & 0xFFFF) << 16) | ((unsigned int)(lo) & 0xFFFF))
#endif

/* ADR/DRC register blocks */
#define ADR_CTRL_START       0x00004004  /* control params start */
#define ADR_CTRL_END         0x00004068
#define ADR_KNEE_START       0x0000406c  /* map/ctc kneepoints start */
#define ADR_KNEE_END         0x00004080
#define ADR_LUT_START        0x00004084  /* main LUT window (loop writes) */
#define ADR_LUT_END          0x00004294
#define ADR_EXTRA_START      0x00004294  /* additional params */
#define ADR_EXTRA_END        0x0000433c
#define ADR_CTC_START        0x00004340  /* CTC/COC kneepoints */
#define ADR_CTC_END          0x00004458

/* YDNS register window */
#define YDNS_REG_START       0x00007af0
#define YDNS_REG_END         0x00007afc

/* Derived counts (inclusive ranges) */
#define ADR_LUT_WORD_COUNT   ((((ADR_LUT_END) - (ADR_LUT_START)) / 4) + 1)
#define ADR_CTRL_WORD_COUNT  ((((ADR_CTRL_END) - (ADR_CTRL_START)) / 4) + 1)
#define ADR_KNEE_WORD_COUNT  ((((ADR_KNEE_END) - (ADR_KNEE_START)) / 4) + 1)
#define ADR_EXTRA_WORD_COUNT ((((ADR_EXTRA_END) - (ADR_EXTRA_START)) / 4) + 1)
#define ADR_CTC_WORD_COUNT   ((((ADR_CTC_END) - (ADR_CTC_START)) / 4) + 1)
#define YDNS_WORD_COUNT      ((((YDNS_REG_END) - (YDNS_REG_START)) / 4) + 1)

