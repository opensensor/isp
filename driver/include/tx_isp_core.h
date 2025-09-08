#ifndef __TX_ISP_CORE_H__
#define __TX_ISP_CORE_H__

/* Core Functions */
int tx_isp_core_remove(struct platform_device *pdev);
int tx_isp_core_probe(struct platform_device *pdev);

/* Core Operations */
int tx_isp_core_start(struct tx_isp_subdev *sd);
int tx_isp_core_stop(struct tx_isp_subdev *sd);
int tx_isp_core_set_format(struct tx_isp_subdev *sd, struct tx_isp_config *config);

/* Tiziano ISP Core Functions */
int tiziano_isp_init(struct tx_isp_sensor_attribute *sensor_attr, char *param_name);
int tiziano_sync_sensor_attr(struct tx_isp_sensor_attribute *attr);
int tiziano_channel_start(int channel_id, struct tx_isp_channel_attr *attr);

/* Hardware Reset Functions */
int tx_isp_hardware_reset(int reset_mode);
u32 tx_isp_check_reset_status(void);

/* ISP Device Management */
struct tx_isp_dev *tx_isp_get_device(void);
void tx_isp_set_device(struct tx_isp_dev *isp);

int tx_isp_create_graph_and_nodes(struct tx_isp_dev *isp);


/* Core States */
#define CORE_STATE_OFF       0
#define CORE_STATE_IDLE     1
#define CORE_STATE_ACTIVE   2
#define CORE_STATE_ERROR    3

/* Core Error Flags */
#define CORE_ERR_CONFIG     BIT(0)
#define CORE_ERR_TIMEOUT    BIT(1)
#define CORE_ERR_OVERFLOW   BIT(2)
#define CORE_ERR_HARDWARE   BIT(3)

/* Dynamic Register State Machine - Based on Binary Ninja Analysis */
struct isp_register_state {
    u32 b050_state_counter;         /* Register 0xb050 cycle counter */
    u32 b050_cycle[7];              /* Cycle pattern: 0x3,0x1,0x0,0x2,0x3,0x1,0x3 */
    u32 b050_current_index;         /* Current position in cycle */
    
    u32 counter_98cc;               /* Register 0x98cc counter value */
    u32 counter_98cc_sequence[18];  /* Sequence: 0x1,0x3,0x6,0x8,0xb,0xd,0xf,0x11,0x12,0x14,0x15,0x17,0x18 */
    u32 counter_98cc_index;         /* Current position in sequence */
    
    u32 toggle_987c_state;          /* Register 0x987c toggle state */
    u32 toggle_987c_idle;           /* 0xc0000000 (idle/disabled) */
    u32 toggle_987c_active;         /* 0xd0XXXXXX (active states) */
    
    spinlock_t state_lock;          /* Protection for state changes */
    struct delayed_work update_work; /* Work queue for timed updates */
    struct delayed_work timing_work_60ms;  /* 60ms timing coordination */
    struct delayed_work timing_work_90ms;  /* 90ms timing coordination */
    struct delayed_work timing_work_180ms; /* 180ms timing coordination */
    
    bool streaming_active;          /* Is streaming currently active */
    u32 frame_count;               /* Frame counter for synchronization */
};

/* CSI PHY Multi-Instance Configuration */
struct isp_csi_phy_instance {
    void __iomem *phy_regs;        /* PHY register base */
    u32 instance_id;               /* Instance ID (csi, w01, w02, m0) */
    u32 config_offset_start;       /* Configuration range start (0x0 for csi) */
    u32 config_offset_end;         /* Configuration range end (0x2fc for csi) */
    bool configured;               /* Instance configuration status */
};

/* Timing Coordination Framework */
struct isp_timing_coordinator {
    struct timer_list frame_sync_timer;    /* Frame synchronization timer */
    struct work_struct reg_update_work;    /* Register update work */
    u32 sync_delays[3];                    /* 60ms, 90ms, 180ms delays */
    u32 current_delay_index;               /* Current delay being processed */
    atomic_t update_in_progress;           /* Update operation flag */
};

/* ISP Register State Functions */
int isp_register_state_init(struct isp_register_state *state);
void isp_register_state_deinit(struct isp_register_state *state);
int isp_register_state_cycle(struct isp_register_state *state);
int isp_register_state_start_streaming(struct isp_register_state *state);
int isp_register_state_stop_streaming(struct isp_register_state *state);

/* CSI PHY Multi-Instance Functions */
int isp_csi_phy_init_all_instances(struct tx_isp_dev *isp);
int isp_csi_phy_configure_instance(struct isp_csi_phy_instance *instance, u32 pixel_clock);
void isp_csi_phy_deinit_all_instances(struct tx_isp_dev *isp);

/* Timing Coordination Functions */
int isp_timing_coordinator_init(struct isp_timing_coordinator *coordinator);
void isp_timing_coordinator_start(struct isp_timing_coordinator *coordinator);
void isp_timing_coordinator_stop(struct isp_timing_coordinator *coordinator);
void isp_timing_coordinator_deinit(struct isp_timing_coordinator *coordinator);

/* Frame-Synchronized Update Functions */
void isp_frame_sync_register_updates(struct tx_isp_dev *isp);
void isp_interrupt_driven_state_cycle(struct tx_isp_dev *isp);

#endif /* __TX_ISP_CORE_H__ */
