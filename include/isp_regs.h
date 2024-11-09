#ifndef ISP_REGS_H
#define ISP_REGS_H

#include <stdint.h>

// Define base address of ISP registers (replace with actual address)
#define ISP_BASE_ADDR 0xA0000000

// Define register offsets
#define ISP_REG_CONFIG         0x00
#define ISP_REG_MODE           0x04
#define ISP_REG_MEM_OPT        0x08
#define ISP_REG_CSC_VERSION    0x1C
#define ISP_REG_INIT           0x30

// Register access macros
#define REG_WRITE(offset, value) (*(volatile uint32_t *)(ISP_BASE_ADDR + (offset)) = (value))
#define REG_READ(offset) (*(volatile uint32_t *)(ISP_BASE_ADDR + (offset)))

#endif // ISP_REGS_H
