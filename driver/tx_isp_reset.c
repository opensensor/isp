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
    
    pr_debug("*** TX ISP RESET STATUS: 0x%08x ***\n", reg_val);
    pr_debug("  Reset Trigger (bit 21): %s\n",
            (reg_val & TX_ISP_RESET_TRIGGER) ? "SET" : "CLEAR");
    pr_debug("  Hardware Ready (bit 20): %s\n",
            (reg_val & TX_ISP_RESET_READY) ? "READY" : "NOT READY");
    pr_debug("  Reset Complete (bit 22): %s\n",
            (reg_val & TX_ISP_RESET_COMPLETE) ? "SET" : "CLEAR");
    
    return reg_val;
}
EXPORT_SYMBOL_GPL(tx_isp_check_reset_status);
