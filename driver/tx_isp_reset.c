/*
 * TX ISP Hardware Reset Implementation
 * Based on reference driver private_reset_tx_isp_module function
 * 
 * This provides the critical hardware reset sequence that must be
 * performed before ISP core initialization to ensure proper startup.
 */

#include <linux/delay.h>
#include <linux/io.h>
#include "tx_isp_core.h"
#include "tx-isp-debug.h"

/* Hardware reset control register */
#define TX_ISP_RESET_REG        0xb00000c4

/* Reset control bits */
#define TX_ISP_RESET_TRIGGER    0x200000  /* Bit 21: Trigger reset */
#define TX_ISP_RESET_READY      0x100000  /* Bit 20: Hardware ready flag */
#define TX_ISP_RESET_COMPLETE   0x400000  /* Bit 22: Complete reset */

/* Reset timeout (500 iterations * 2ms = 1 second) */
#define TX_ISP_RESET_TIMEOUT    500
#define TX_ISP_RESET_DELAY_MS   2

/**
 * tx_isp_hardware_reset - Perform hardware reset of TX ISP module
 * @reset_mode: Reset mode (0 = full reset, non-zero = skip reset)
 * 
 * This function implements the exact hardware reset sequence from the
 * reference driver as decompiled by Binary Ninja. It matches the
 * reference implementation exactly.
 * 
 * Returns: 0 on success, -1 (0xffffffff) on timeout
 */
int tx_isp_hardware_reset(int reset_mode)
{
    volatile u32 *reset_reg_ptr;
    u32 reg_val;
    int i;
    
    pr_info("*** TX ISP HARDWARE RESET: Starting (mode=%d) ***\n", reset_mode);
    
    /* Early exit for non-reset mode (matches reference: if (arg1 != 0) return 0) */
    if (reset_mode != 0) {
        pr_info("*** TX ISP HARDWARE RESET: Skip mode, returning success ***\n");
        return 0;
    }
    
    /* Direct memory access to reset register (matches reference: *0xb00000c4) */
    reset_reg_ptr = (volatile u32 *)ioremap(TX_ISP_RESET_REG, 4);
    if (!reset_reg_ptr) {
        pr_err("*** TX ISP HARDWARE RESET: Failed to map reset register 0x%08x ***\n", 
               TX_ISP_RESET_REG);
        return -ENOMEM;
    }
    
    /* Read initial register state */
    reg_val = *reset_reg_ptr;
    pr_info("*** TX ISP HARDWARE RESET: Initial register: 0x%08x ***\n", reg_val);
    
    /* Step 1: Set bit 21 (0x200000) - matches reference: *0xb00000c4 |= 0x200000 */
    *reset_reg_ptr |= 0x200000;
    reg_val = *reset_reg_ptr;
    pr_info("*** TX ISP HARDWARE RESET: Trigger set, reg=0x%08x ***\n", reg_val);
    
    /* Step 2: Loop 0x1f4 (500) times checking for ready bit - matches reference loop */
    for (i = 0x1f4; i != 0; i--) {
        reg_val = *reset_reg_ptr;
        
        /* Check if bit 20 (0x100000) is set - matches reference: (*0xb00000c4 & 0x100000) != 0 */
        if ((reg_val & 0x100000) != 0) {
            pr_info("*** TX ISP HARDWARE RESET: Ready bit detected, reg=0x%08x ***\n", reg_val);
            
            /* Step 3: Clear bit 21 and set bit 22 - matches reference: (*0xb00000c4 & 0xffdfffff) | 0x400000 */
            *reset_reg_ptr = (reg_val & 0xffdfffff) | 0x400000;
            reg_val = *reset_reg_ptr;
            pr_info("*** TX ISP HARDWARE RESET: Complete bit set, reg=0x%08x ***\n", reg_val);
            
            /* Step 4: Clear bit 22 - matches reference: *0xb00000c4 &= 0xffbfffff */
            *reset_reg_ptr &= 0xffbfffff;
            reg_val = *reset_reg_ptr;
            pr_info("*** TX ISP HARDWARE RESET: Final reg=0x%08x ***\n", reg_val);
            
            iounmap((void __iomem *)reset_reg_ptr);
            pr_info("*** TX ISP HARDWARE RESET: SUCCESS ***\n");
            return 0;
        }
        
        /* Sleep 2ms before next check - matches reference: private_msleep(2) */
        msleep(2);
    }
    
    /* Timeout - matches reference: return 0xffffffff */
    reg_val = *reset_reg_ptr;
    pr_err("*** TX ISP HARDWARE RESET: TIMEOUT - Final reg=0x%08x ***\n", reg_val);
    iounmap((void __iomem *)reset_reg_ptr);
    pr_err("*** TX ISP HARDWARE RESET: TIMEOUT ***\n");
    
    return -1;
}
EXPORT_SYMBOL_GPL(tx_isp_hardware_reset);

/**
 * tx_isp_check_reset_status - Check current hardware reset status
 * 
 * Returns the current value of the reset control register for debugging
 */
u32 tx_isp_check_reset_status(void)
{
    void __iomem *reset_reg;
    u32 reg_val;
    
    reset_reg = ioremap(TX_ISP_RESET_REG, 4);
    if (!reset_reg) {
        pr_err("tx_isp_check_reset_status: Failed to map reset register\n");
        return 0;
    }
    
    reg_val = readl(reset_reg);
    iounmap(reset_reg);
    
    pr_info("*** TX ISP RESET STATUS: 0x%08x ***\n", reg_val);
    pr_info("  Reset Trigger (bit 21): %s\n", 
            (reg_val & TX_ISP_RESET_TRIGGER) ? "SET" : "CLEAR");
    pr_info("  Hardware Ready (bit 20): %s\n", 
            (reg_val & TX_ISP_RESET_READY) ? "READY" : "NOT READY");
    pr_info("  Reset Complete (bit 22): %s\n", 
            (reg_val & TX_ISP_RESET_COMPLETE) ? "SET" : "CLEAR");
    
    return reg_val;
}
EXPORT_SYMBOL_GPL(tx_isp_check_reset_status);
