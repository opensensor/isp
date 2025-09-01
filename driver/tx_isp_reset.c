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
 * reference driver. It must be called before any ISP initialization
 * to ensure the hardware is in a clean state.
 * 
 * Returns: 0 on success, -ETIMEDOUT on timeout
 */
int tx_isp_hardware_reset(int reset_mode)
{
    void __iomem *reset_reg;
    u32 reg_val;
    int timeout_count;
    
    pr_info("*** TX ISP HARDWARE RESET: Starting reset sequence (mode=%d) ***\n", reset_mode);
    
    /* Early exit for non-reset mode (matches reference behavior) */
    if (reset_mode != 0) {
        pr_info("*** TX ISP HARDWARE RESET: Skip mode, returning success ***\n");
        return 0;
    }
    
    /* Map the reset control register */
    reset_reg = ioremap(TX_ISP_RESET_REG, 4);
    if (!reset_reg) {
        pr_err("*** TX ISP HARDWARE RESET: Failed to map reset register 0x%08x ***\n", 
               TX_ISP_RESET_REG);
        return -ENOMEM;
    }
    
    /* Step 1: Trigger hardware reset by setting bit 21 */
    reg_val = readl(reset_reg);
    pr_info("*** TX ISP HARDWARE RESET: Initial register value: 0x%08x ***\n", reg_val);
    reg_val |= TX_ISP_RESET_TRIGGER;
    writel(reg_val, reset_reg);
    pr_info("*** TX ISP HARDWARE RESET: Reset triggered, reg=0x%08x, waiting for ready ***\n", reg_val);
    
    /* Step 2: Wait for hardware ready signal (bit 20) with timeout */
    for (timeout_count = TX_ISP_RESET_TIMEOUT; timeout_count > 0; timeout_count--) {
        reg_val = readl(reset_reg);
        
        /* Check if hardware signals ready */
        if (reg_val & TX_ISP_RESET_READY) {
            /* Step 3: Complete reset sequence */
            /* Clear trigger bit (21) and set complete bit (22) */
            reg_val = (reg_val & ~TX_ISP_RESET_TRIGGER) | TX_ISP_RESET_COMPLETE;
            writel(reg_val, reset_reg);
            
            /* Clear complete bit (22) to finish sequence */
            reg_val &= ~TX_ISP_RESET_COMPLETE;
            writel(reg_val, reset_reg);
            
            iounmap(reset_reg);
            
            mcp_log_info("tx_isp_hardware_reset: Hardware reset completed successfully", 
                         (struct mcp_data_payload){
                             .buffer_info = {.physical_addr = reg_val},
                             .operation = "hardware_reset_success"
                         });
            
            pr_info("*** TX ISP HARDWARE RESET COMPLETED SUCCESSFULLY ***\n");
            return 0;
        }
        
        /* Wait before next check */
        msleep(TX_ISP_RESET_DELAY_MS);
    }
    
    /* Timeout occurred */
    iounmap(reset_reg);
    
    pr_err("*** TX ISP HARDWARE RESET TIMEOUT - HARDWARE NOT RESPONDING ***\n");
    mcp_log_error("tx_isp_hardware_reset: Hardware reset timeout", 
                  (struct mcp_data_payload){
                      .error_info = "reset_timeout",
                      .operation = "hardware_reset_timeout"
                  });
    
    return -ETIMEDOUT;
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
