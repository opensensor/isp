#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include "../include/tx_isp.h"
#include "../include/tx_isp_core.h"
#include "../include/tx-isp-debug.h"
#include "../include/tx_isp_vic.h"
#include "../include/tx_isp_csi.h"

/* Dynamic VIC Timing Negotiation - Based on Reference Driver Analysis */

/* Timing negotiation state machine states */
enum vic_timing_state {
    VIC_TIMING_INIT = 0,
    VIC_TIMING_CONSERVATIVE = 1,
    VIC_TIMING_SENSOR_DETECT = 2,
    VIC_TIMING_PHY_TRAIN = 3,
    VIC_TIMING_CALIBRATE = 4,
    VIC_TIMING_VALIDATE = 5,
    VIC_TIMING_ACTIVE = 6,
    VIC_TIMING_ERROR = 7
};

/* Timing parameter set for dynamic adjustment */
struct vic_timing_params {
    u32 csi_phy_config;     /* CSI PHY configuration */
    u32 core_control;       /* Core control register */
    u32 mipi_config;        /* MIPI configuration */
    u32 frame_timing;       /* Frame timing parameters */
    u32 integration_time;   /* Integration time */
    u32 clock_ratio;        /* Clock ratio settings */
    bool valid;             /* Parameter set validity */
};

/* Dynamic timing negotiation context */
struct vic_timing_context {
    enum vic_timing_state state;
    struct vic_timing_params active_params;
    struct vic_timing_params target;
    struct vic_timing_params safe_fallback;
    int negotiation_attempts;
    int max_attempts;
    bool sensor_responsive;
    bool phy_trained;
    u32 constraint_violations;
};

/* Initialize conservative timing parameters - known safe values */
static void vic_init_conservative_timing(struct vic_timing_context *ctx, 
                                        struct tx_isp_sensor_attribute *sensor_attr)
{
    pr_info("*** VIC DYNAMIC: Initializing conservative timing parameters ***\n");
    
    /* Conservative CSI PHY settings */
    ctx->active_params.csi_phy_config = 0x58090000;  /* Reference driver initial value */
    
    /* Conservative core control - start with minimal functionality */
    ctx->active_params.core_control = 0x41190000;    /* Reference driver initial value */
    
    /* Conservative MIPI config based on sensor interface */
    if (sensor_attr->dbus_type == 2) { /* MIPI */
        ctx->active_params.mipi_config = 0x20000;    /* Basic MIPI mode */
        if (sensor_attr->mipi.clk_pol == 2) ctx->active_params.mipi_config |= 2;
        if (sensor_attr->mipi.data_pol == 2) ctx->active_params.mipi_config |= 1;
    } else {
        ctx->active_params.mipi_config = 0;
    }
    
    /* Conservative frame timing - use sensor minimums */
    ctx->active_params.frame_timing = (sensor_attr->min_integration_time_native << 16) | 
                               sensor_attr->total_width;
    
    /* Conservative integration time */
    ctx->active_params.integration_time = sensor_attr->min_integration_time_native;
    
    /* Conservative clock ratio */
    ctx->active_params.clock_ratio = 0x100010;      /* 1:1 ratio initially */
    
    ctx->active_params.valid = true;
    
    /* Store as safe fallback */
    ctx->safe_fallback = ctx->active_params;
    
    pr_info("VIC DYNAMIC: Conservative parameters - CSI=0x%x, Core=0x%x, MIPI=0x%x\n",
            ctx->active_params.csi_phy_config, ctx->active_params.core_control, ctx->active_params.mipi_config);
}

/* Apply timing parameters to hardware with validation */
static int vic_apply_timing_params(struct tx_isp_vic_device *vic_dev, 
                                  struct vic_timing_params *params)
{
    void __iomem *vic_regs = vic_dev->vic_regs;
    u32 status;
    int timeout = 1000;
    
    if (!params->valid) {
        pr_err("VIC DYNAMIC: Attempted to apply invalid timing parameters\n");
        return -EINVAL;
    }
    
    pr_info("*** VIC DYNAMIC: Applying timing parameters ***\n");
    
    /* Enter configuration state */
    writel(2, vic_regs + 0x0);  /* CONFIG state */
    wmb();
    
    /* Wait for config state acceptance */
    while (timeout > 0 && (readl(vic_regs + 0x0) & 0xF) != 2) {
        udelay(1);
        timeout--;
    }
    
    if (timeout == 0) {
        pr_err("VIC DYNAMIC: Failed to enter CONFIG state for timing update\n");
        return -ETIMEDOUT;
    }
    
    /* Apply CSI PHY configuration */
    writel(params->csi_phy_config, vic_regs + 0xac);
    wmb();
    
    /* Apply core control configuration */
    writel(params->core_control, vic_regs + 0xb054);
    wmb();
    
    /* Apply MIPI configuration */
    writel(params->mipi_config, vic_regs + 0x10);
    wmb();
    
    /* Apply frame timing */
    writel(params->frame_timing, vic_regs + 0x18);
    wmb();
    
    /* Apply integration time */
    writel(params->integration_time, vic_regs + 0x104);
    wmb();
    
    /* Apply clock ratio */
    writel(params->clock_ratio, vic_regs + 0x1a4);
    wmb();
    
    pr_info("VIC DYNAMIC: Hardware registers updated with new timing parameters\n");
    
    /* Validate hardware acceptance */
    status = readl(vic_regs + 0x0);
    if ((status & 0xF) != 2) {
        pr_err("VIC DYNAMIC: Hardware rejected timing parameters - status=0x%x\n", status);
        return -EINVAL;
    }
    
    return 0;
}

/* Check for hardware constraint violations */
static u32 vic_check_constraints(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_regs = vic_dev->vic_regs;
    u32 violations = 0;
    u32 status, error_flags;
    
    /* Read hardware status registers */
    status = readl(vic_regs + 0x0);
    error_flags = readl(vic_regs + 0x1e0);  /* Interrupt status */
    
    /* Check for control limit error */
    if (error_flags & 0x200000) {
        violations |= BIT(0);
        pr_warn("VIC CONSTRAINT: Control limit error detected\n");
    }
    
    /* Check for timing violations */
    if (error_flags & 0x400) {
        violations |= BIT(1);
        pr_warn("VIC CONSTRAINT: Horizontal timing error\n");
    }
    
    if (error_flags & 0x4000) {
        violations |= BIT(2);
        pr_warn("VIC CONSTRAINT: Vertical timing error\n");
    }
    
    /* Check for FIFO overflows */
    if (error_flags & 0x200) {
        violations |= BIT(3);
        pr_warn("VIC CONSTRAINT: FIFO overflow\n");
    }
    
    /* Check for MIPI errors */
    if (error_flags & 0x1000000) {
        violations |= BIT(4);
        pr_warn("VIC CONSTRAINT: MIPI horizontal completion error\n");
    }
    
    if (violations) {
        pr_info("VIC CONSTRAINT: Total violations detected: 0x%x\n", violations);
        
        /* Clear error flags */
        writel(error_flags, vic_regs + 0x1f0);
        wmb();
    }
    
    return violations;
}

/* Sensor communication test */
static bool vic_test_sensor_communication(struct tx_isp_vic_device *vic_dev)
{
    void __iomem *vic_regs = vic_dev->vic_regs;
    u32 frame_count_before, frame_count_after;
    int timeout = 100;
    
    pr_info("*** VIC DYNAMIC: Testing sensor communication ***\n");
    
    /* Get initial frame count */
    frame_count_before = vic_dev->frame_count;
    
    /* Enable minimal streaming to test sensor response */
    writel(1, vic_regs + 0x0);  /* STREAMING state */
    wmb();
    
    /* Wait for sensor response */
    while (timeout > 0) {
        frame_count_after = vic_dev->frame_count;
        if (frame_count_after > frame_count_before) {
            pr_info("VIC DYNAMIC: Sensor responsive - frame count increased %d -> %d\n",
                    frame_count_before, frame_count_after);
            return true;
        }
        msleep(10);
        timeout--;
    }
    
    pr_warn("VIC DYNAMIC: Sensor not responsive - no frame count increase\n");
    return false;
}

/* CSI PHY training sequence */
static int vic_train_csi_phy(struct tx_isp_vic_device *vic_dev, 
                            struct vic_timing_context *ctx)
{
    void __iomem *vic_regs = vic_dev->vic_regs;
    u32 phy_config = ctx->active_params.csi_phy_config;
    int training_cycles = 0;
    bool training_success = false;
    
    pr_info("*** VIC DYNAMIC: Starting CSI PHY training ***\n");
    
    /* PHY training sequence - oscillate between configurations like reference */
    for (training_cycles = 0; training_cycles < 10 && !training_success; training_cycles++) {
        /* Alternate PHY configurations */
        if (training_cycles % 2 == 0) {
            phy_config = 0x58090000;  /* Configuration A */
        } else {
            phy_config = 0x59010000;  /* Configuration B */
        }
        
        pr_info("VIC PHY TRAINING: Cycle %d - trying config 0x%x\n", 
                training_cycles, phy_config);
        
        /* Apply PHY configuration */
        writel(phy_config, vic_regs + 0xac);
        wmb();
        
        /* Allow PHY to settle */
        msleep(10);
        
        /* Test communication */
        if (vic_test_sensor_communication(vic_dev)) {
            /* Check for constraint violations */
            u32 violations = vic_check_constraints(vic_dev);
            if (violations == 0) {
                training_success = true;
                ctx->active_params.csi_phy_config = phy_config;
                pr_info("VIC PHY TRAINING: Success with config 0x%x after %d cycles\n",
                        phy_config, training_cycles + 1);
                break;
            } else {
                pr_info("VIC PHY TRAINING: Config 0x%x has violations 0x%x, continuing\n",
                        phy_config, violations);
            }
        }
    }
    
    if (!training_success) {
        pr_err("VIC PHY TRAINING: Failed after %d cycles\n", training_cycles);
        return -ETIMEDOUT;
    }
    
    ctx->phy_trained = true;
    return 0;
}

/* Core control calibration - multiple register updates like reference */
static int vic_calibrate_core_control(struct tx_isp_vic_device *vic_dev,
                                     struct vic_timing_context *ctx)
{
    void __iomem *vic_regs = vic_dev->vic_regs;
    u32 core_configs[] = {
        0x41190000,  /* Initial conservative */
        0xb2f9d612,  /* Reference driver progression */
        0xb2f1e6d0,  /* Further adjustment */
        0xa1f8c5b2,  /* Calibrated value */
        0x91e7b4a3   /* Final optimized */
    };
    int config_count = ARRAY_SIZE(core_configs);
    int i;
    
    pr_info("*** VIC DYNAMIC: Starting core control calibration ***\n");
    
    for (i = 0; i < config_count; i++) {
        pr_info("VIC CORE CALIBRATION: Testing config %d/5 - 0x%x\n", 
                i + 1, core_configs[i]);
        
        /* Apply core control configuration */
        writel(core_configs[i], vic_regs + 0xb054);
        wmb();
        
        /* Allow hardware to settle */
        msleep(5);
        
        /* Test for constraint violations */
        u32 violations = vic_check_constraints(vic_dev);
        
        if (violations == 0) {
            /* Test sensor communication with this config */
            if (vic_test_sensor_communication(vic_dev)) {
                ctx->current.core_control = core_configs[i];
                pr_info("VIC CORE CALIBRATION: Success with config 0x%x\n", 
                        core_configs[i]);
                return 0;
            }
        } else {
            pr_info("VIC CORE CALIBRATION: Config 0x%x has violations 0x%x\n",
                    core_configs[i], violations);
        }
    }
    
    pr_warn("VIC CORE CALIBRATION: No optimal config found, using conservative\n");
    ctx->current.core_control = core_configs[0];  /* Fall back to conservative */
    return 0;
}

/* Dynamic timing negotiation main function */
int vic_dynamic_timing_negotiation(struct tx_isp_vic_device *vic_dev)
{
    struct vic_timing_context ctx;
    struct tx_isp_sensor_attribute *sensor_attr = &vic_dev->sensor_attr;
    int ret = 0;
    
    pr_info("*** VIC DYNAMIC TIMING NEGOTIATION: Starting state machine ***\n");
    
    /* Initialize negotiation context */
    memset(&ctx, 0, sizeof(ctx));
    ctx.state = VIC_TIMING_INIT;
    ctx.max_attempts = 5;
    ctx.negotiation_attempts = 0;
    
    while (ctx.state != VIC_TIMING_ACTIVE && ctx.state != VIC_TIMING_ERROR) {
        ctx.negotiation_attempts++;
        
        if (ctx.negotiation_attempts > ctx.max_attempts) {
            pr_err("VIC DYNAMIC: Max negotiation attempts exceeded\n");
            ctx.state = VIC_TIMING_ERROR;
            break;
        }
        
        pr_info("VIC DYNAMIC: State machine - attempt %d, state %d\n",
                ctx.negotiation_attempts, ctx.state);
        
        switch (ctx.state) {
        case VIC_TIMING_INIT:
            pr_info("VIC DYNAMIC: STATE 1 - Initialize conservative timing\n");
            vic_init_conservative_timing(&ctx, sensor_attr);
            ctx.state = VIC_TIMING_CONSERVATIVE;
            break;
            
        case VIC_TIMING_CONSERVATIVE:
            pr_info("VIC DYNAMIC: STATE 2 - Apply conservative parameters\n");
            ret = vic_apply_timing_params(vic_dev, &ctx.current);
            if (ret == 0) {
                ctx.state = VIC_TIMING_SENSOR_DETECT;
            } else {
                pr_err("VIC DYNAMIC: Failed to apply conservative parameters\n");
                ctx.state = VIC_TIMING_ERROR;
            }
            break;
            
        case VIC_TIMING_SENSOR_DETECT:
            pr_info("VIC DYNAMIC: STATE 3 - Sensor detection and communication test\n");
            ctx.sensor_responsive = vic_test_sensor_communication(vic_dev);
            if (ctx.sensor_responsive) {
                ctx.state = VIC_TIMING_PHY_TRAIN;
            } else {
                pr_warn("VIC DYNAMIC: Sensor not responsive, retrying with different params\n");
                /* Try different conservative parameters */
                ctx.current.csi_phy_config = 0x59010000;  /* Alternative PHY config */
                ctx.state = VIC_TIMING_CONSERVATIVE;
            }
            break;
            
        case VIC_TIMING_PHY_TRAIN:
            pr_info("VIC DYNAMIC: STATE 4 - CSI PHY training\n");
            ret = vic_train_csi_phy(vic_dev, &ctx);
            if (ret == 0) {
                ctx.state = VIC_TIMING_CALIBRATE;
            } else {
                pr_err("VIC DYNAMIC: PHY training failed\n");
                ctx.state = VIC_TIMING_ERROR;
            }
            break;
            
        case VIC_TIMING_CALIBRATE:
            pr_info("VIC DYNAMIC: STATE 5 - Core control calibration\n");
            ret = vic_calibrate_core_control(vic_dev, &ctx);
            if (ret == 0) {
                ctx.state = VIC_TIMING_VALIDATE;
            } else {
                pr_err("VIC DYNAMIC: Core calibration failed\n");
                ctx.state = VIC_TIMING_ERROR;
            }
            break;
            
        case VIC_TIMING_VALIDATE:
            pr_info("VIC DYNAMIC: STATE 6 - Final validation\n");
            ctx.constraint_violations = vic_check_constraints(vic_dev);
            if (ctx.constraint_violations == 0) {
                pr_info("VIC DYNAMIC: Validation successful - no constraint violations\n");
                ctx.state = VIC_TIMING_ACTIVE;
            } else {
                pr_warn("VIC DYNAMIC: Validation failed - violations 0x%x, retrying\n",
                        ctx.constraint_violations);
                /* Fall back to safe parameters and retry */
                ctx.current = ctx.safe_fallback;
                ctx.state = VIC_TIMING_CONSERVATIVE;
            }
            break;
            
        default:
            pr_err("VIC DYNAMIC: Unknown state %d\n", ctx.state);
            ctx.state = VIC_TIMING_ERROR;
            break;
        }
        
        /* Small delay between state transitions */
        msleep(1);
    }
    
    if (ctx.state == VIC_TIMING_ACTIVE) {
        pr_info("*** VIC DYNAMIC TIMING NEGOTIATION: SUCCESS ***\n");
        pr_info("  Final CSI PHY config: 0x%x\n", ctx.current.csi_phy_config);
        pr_info("  Final core control: 0x%x\n", ctx.current.core_control);
        pr_info("  Final MIPI config: 0x%x\n", ctx.current.mipi_config);
        pr_info("  Negotiation attempts: %d\n", ctx.negotiation_attempts);
        pr_info("  Sensor responsive: %s\n", ctx.sensor_responsive ? "YES" : "NO");
        pr_info("  PHY trained: %s\n", ctx.phy_trained ? "YES" : "NO");
        return 0;
    } else {
        pr_err("*** VIC DYNAMIC TIMING NEGOTIATION: FAILED ***\n");
        pr_err("  Final state: %d\n", ctx.state);
        pr_err("  Constraint violations: 0x%x\n", ctx.constraint_violations);
        pr_err("  Attempts made: %d\n", ctx.negotiation_attempts);
        return -EINVAL;
    }
}

/* Export the dynamic timing negotiation function */
EXPORT_SYMBOL(vic_dynamic_timing_negotiation);
