#include "include/main.h"


  int32_t ispcore_interrupt_service_routine(void* arg1)

{
    void* $v0 = *(arg1 + 0xb8);
    void* $s0 = *(arg1 + 0xd4);
    int32_t $s1 = *($v0 + 0xb4);
    *($v0 + 0xb8) = $s1;
    int32_t $a0;
    
    if (!($s1 & 0x3f8))
        $a0 = *($s0 + 0x15c);
    else
    {
        int32_t var_44_1_5 = *(*(arg1 + 0xb8) + 0x84c);
        int32_t var_48_1_13 = 0x3f8;
        isp_printf(1, "ispcore: irq-status 0x%08x, err is 0x%x,0x%x,084c is 0x%x\\n", $s1);
        data_ca57c_2 += 1;
        $a0 = *($s0 + 0x15c);
    }
    
    if ($a0 == 1)
        return 1;
    
    int32_t $s3_1 = $s1 & 0x1000;
    int32_t $v0_5 = $s1 & 0x200;
    
    if ($s3_1)
    {
        private_schedule_work(&fs_work);
        $v0_5 = $s1 & 0x200;
    }
    
    int32_t $v0_9 = $s1 & 0x100;
    
    if ($v0_5)
    {
        if (*($s0 + 0x17c))
            exception_handle();
        
        data_ca578_2 += 1;
        $v0_9 = $s1 & 0x100;
    }
    
    if ($v0_9)
    {
        if (*($s0 + 0x17c))
            exception_handle();
        
        data_ca574_2 += 1;
    }
    
    int32_t $v0_14 = $s1 & 0x2000;
    
    if ($s3_1)
    {
        uint32_t $v1_1 = data_ca554_3;
        
        if ($v1_1 == 1)
        {
            private_do_gettimeofday(&data_ca568_2);
            data_ca554_4 = 2;
            $v1_1 = data_ca554_5;
        }
        
        if ($v1_1 == 3)
        {
            private_do_gettimeofday(&data_ca558_2);
            data_ca554_6 = 4;
        }
        
        $v0_14 = $s1 & 0x2000;
        
        if (isp_ch0_pre_dequeue_time)
        {
            private_schedule_work(&pre_frame_dequeue);
            $v0_14 = $s1 & 0x2000;
        }
    }
    
    int32_t $v0_17 = $s1 & 1;
    
    if ($v0_14)
    {
        data_ca580_1 += 1;
        $v0_17 = $s1 & 1;
    }
    
    int32_t $v0_36 = $s1 & 2;
    void var_40_62;
    
    if ($v0_17)
    {
        data_ca584_2 += 1;
        void* $s3_2 = *($s0 + 0x150);
        
        if (data_ca554_7 == 2)
        {
            private_do_gettimeofday(&data_ca560_1);
            data_ca554_8 = 3;
        }
        
        int32_t $v0_28 = *(arg1 + 0xb8);
        
        while (!(*($v0_28 + 0x997c) & 1))
        {
            int32_t var_38_1_19 = *($v0_28 + 0x9974);
            int32_t var_34_1_16 = *($v0_28 + 0x998c);
            int32_t var_30_1_20 = *($v0_28 + 0x9990);
            int32_t var_2c_1_15 = 0;
            int32_t var_28_1_9 = *($s0 + 0x128) << 0x10
                | *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + 0x9888) >> 0x10;
            tx_isp_send_event_to_remote(*($s3_2 + 0x78), 0x3000006, &var_40_63);
            $v0_28 = *(arg1 + 0xb8);
        }
        
        void* $a0_1 = *($s0 + 0x1bc);
        
        if ($a0_1)
            (*($a0_1 + 0x40cc))($a0_1, 0x4000002, 0);
        
        int32_t $v1_10;
        
        if (data_ca570_1 != 1)
            $v1_10 = *($s0 + 0x178);
        else
        {
            if (*(*($s0 + 0x1bc) + 0x40a4))
                data_ca570_2 = 0;
            else
            {
                system_reg_write(0x6030, 0xff00ff00);
                data_ca570_3 = 0;
            }
            
            $v1_10 = *($s0 + 0x178);
        }
        
        int32_t $v0_35;
        
        if ($v1_10 == 1)
        {
            uint8_t isp_day_night_switch_drop_frame_num_1 = *isp_day_night_switch_drop_frame_num;
            *isp_day_night_switch_drop_frame_cnt = isp_day_night_switch_drop_frame_num_1;
            (*(isp_day_night_switch_drop_frame_cnt + 1)) = isp_day_night_switch_drop_frame_num_1;
            (*(isp_day_night_switch_drop_frame_cnt + 2)) = isp_day_night_switch_drop_frame_num_1;
            isp_day_night_switch_drop_frame_cnt_pdq_interrupt =
                isp_day_night_switch_drop_frame_num_1;
            void* $a0_3;
            
            if (*(*($s0 + 0x1bc) + 0x40a4) != $v1_10)
                $a0_3 = *($s0 + 0x1bc);
            else
            {
                system_reg_write(0x6030, 0xff008080);
                $a0_3 = *($s0 + 0x1bc);
            }
            
            if ($a0_3)
                (*($a0_3 + 0x40cc))($a0_3, 0x4000003, 0);
            
            *($s0 + 0x178) = 0;
            data_ca570_4 = 1;
            $v0_35 = *($s0 + 0x134);
        }
        else
        {
            int32_t $a1_1;
            
            if ($v1_10 != 2)
            {
                if ($v1_10 == 3)
                {
                    $a1_1 = 0xff008080;
                    goto label_79b2c;
                }
                
                $v0_35 = *($s0 + 0x134);
            }
            else
            {
                $a1_1 = 0xff00ff00;
            label_79b2c:
                system_reg_write(0x6030, $a1_1);
                *($s0 + 0x178) = 0;
                $v0_35 = *($s0 + 0x134);
            }
        }
        
        uint32_t first_into_1;
        
        if ($v0_35 != 1)
            first_into_1 = first_into;
        else
        {
            first_into_1 = first_into;
            
            if (*($s0 + 0x11c) == $v0_35)
            {
                mbus_to_bayer_write(*($s0 + 0xf4));
                *($s0 + 0x11c) = 0;
                first_into_1 = first_into;
            }
        }
        
        $v0_36 = $s1 & 2;
        
        if (first_into_1 == 1)
        {
            tisp_top_sel();
            first_into = 0;
            $v0_36 = $s1 & 2;
        }
    }
    
    int32_t $v0_39 = $s1 & 4;
    
    if ($v0_36)
    {
        void* $s5_1 = *($s0 + 0x150);
        int32_t $v0_37 = *(arg1 + 0xb8);
        
        while (!(*($v0_37 + 0x9a7c) & 1))
        {
            int32_t var_38_2_4 = *($v0_37 + 0x9a74);
            int32_t var_34_2_1 = *($v0_37 + 0x9a8c);
            int32_t var_28_2_2 = 0;
            int32_t var_30_2_6 = *($v0_37 + 0x9a90);
            int32_t var_2c_2_2 = 0;
            
            if (!isp_ch1_dequeue_delay_time)
            {
                tx_isp_send_event_to_remote(*($s5_1 + 0x13c), 0x3000006, &var_40_64);
                $v0_37 = *(arg1 + 0xb8);
            }
            else
            {
                memcpy(&ch1_buf, &var_40_65, 0x1c);
                private_schedule_work(&ch1_frame_dequeue_delay);
                $v0_37 = *(arg1 + 0xb8);
            }
        }
        
        $v0_39 = $s1 & 4;
    }
    
    if ($v0_39)
    {
        void* $s4_1 = *($s0 + 0x150);
        int32_t $v0_42 = *(arg1 + 0xb8);
        
        while (!(*($v0_42 + 0x9b7c) & 1))
        {
            int32_t var_38_3_1 = *($v0_42 + 0x9b74);
            int32_t var_34_3_1 = *($v0_42 + 0x9b8c);
            int32_t var_28_3_1 = 0;
            int32_t var_30_3_1 = *($v0_42 + 0x9b90);
            int32_t var_2c_3_1 = 0;
            tx_isp_send_event_to_remote(*($s4_1 + 0x200), 0x3000006, &var_40_66);
            $v0_42 = *(arg1 + 0xb8);
        }
    }
    
    void* $s2_1 = &irq_func_cb;
    int32_t i = 0;
    int32_t result = 1;
    
    do
    {
        int32_t $v0_46 = 1 << (i & 0x1f) & $s1;
        i += 1;
        
        if ($v0_46)
        {
            int32_t $v0_47 = *$s2_1;
            
            if ($v0_47)
            {
                int32_t result_1 = $v0_47();
                
                if (result_1 != 1)
                    result = result_1;
            }
        }
        
        $s2_1 += 4;
    } while (i != 0x20);
    
    return result;
}

