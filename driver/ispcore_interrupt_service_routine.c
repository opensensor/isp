#include "include/main.h"


  int32_t ispcore_interrupt_service_routine(void* arg1)

{
    int32_t* $v0 = (int32_t*)((char*)arg1  + 0xb8); // Fixed void pointer arithmetic
    int32_t* $s0 = (int32_t*)((char*)arg1  + 0xd4); // Fixed void pointer arithmetic
    int32_t $s1 = *($v0 + 0xb4);
    int32_t $a0;
        int32_t var_44_1 = *(*(arg1 + 0xb8) + 0x84c);
        int32_t var_48_1 = 0x3f8;
    *((int32_t*)((char*)$v0 + 0xb8)) = $s1; // Fixed void pointer dereference
    
    if (!($s1 & 0x3f8))
        $a0 = *($s0 + 0x15c);
    else
    {
        isp_printf(); // Fixed: macro with no parameters, removed 6 arguments;
        data_ca57c += 1;
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
        
        data_ca578 += 1;
        $v0_9 = $s1 & 0x100;
    }
    
    if ($v0_9)
    {
        if (*($s0 + 0x17c))
            exception_handle();
        
        data_ca574 += 1;
    }
    
    int32_t $v0_14 = $s1 & 0x2000;
    
    if ($s3_1)
    {
        uint32_t $v1_1 = data_ca554;
        
        if ($v1_1 == 1)
        {
            private_do_gettimeofday(&data_ca568);
            data_ca554 = 2;
            $v1_1 = data_ca554;
        }
        
        if ($v1_1 == 3)
        {
            private_do_gettimeofday(&data_ca558);
            data_ca554 = 4;
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
        data_ca580 += 1;
        $v0_17 = $s1 & 1;
    }
    
    int32_t $v0_36 = $s1 & 2;
    void var_40_29;
    
    if ($v0_17)
    {
        int32_t* $s3_2 = (int32_t*)((char*)$s0  + 0x150); // Fixed void pointer arithmetic
        data_ca584 += 1;
        
        if (data_ca554 == 2)
        {
            private_do_gettimeofday(&data_ca560);
            data_ca554 = 3;
        }
        
        int32_t $v0_28 = *(arg1 + 0xb8);
        
        while (!(*($v0_28 + 0x997c) & 1))
        {
            int32_t var_38_1 = *($v0_28 + 0x9974);
            int32_t var_34_1 = *($v0_28 + 0x998c);
            int32_t var_30_1 = *($v0_28 + 0x9990);
            int32_t var_2c_1 = 0;
            int32_t var_28_1 = *($s0 + 0x128) << 0x10
                | *(*(mdns_y_pspa_cur_bi_wei0_array + 0xb8) + 0x9888) >> 0x10;
            tx_isp_send_event_to_remote(*($s3_2 + 0x78), 0x3000006, &var_40);
            $v0_28 = *(arg1 + 0xb8);
        }
        
        int32_t* $a0_1 = (int32_t*)((char*)$s0  + 0x1bc); // Fixed void pointer arithmetic
        
        if ($a0_1)
            (*($a0_1 + 0x40cc))($a0_1, 0x4000002, 0);
        
        int32_t $v1_10;
        
        if (data_ca570_1 != 1)
            $v1_10 = *($s0 + 0x178);
        else
        {
            if (*(*($s0 + 0x1bc) + 0x40a4))
                data_ca570 = 0;
            else
            {
                system_reg_write(0x6030, 0xff00ff00);
                data_ca570 = 0;
            }
            
            $v1_10 = *($s0 + 0x178);
        }
        
        int32_t $v0_35;
        
        if ($v1_10 == 1)
        {
            uint8_t isp_day_night_switch_drop_frame_num_1 = *isp_day_night_switch_drop_frame_num;
            void* $a0_3;
            *isp_day_night_switch_drop_frame_cnt = isp_day_night_switch_drop_frame_num_1;
            (*(isp_day_night_switch_drop_frame_cnt + 1)) = isp_day_night_switch_drop_frame_num_1;
            (*(isp_day_night_switch_drop_frame_cnt + 2)) = isp_day_night_switch_drop_frame_num_1;
            isp_day_night_switch_drop_frame_cnt_pdq_interrupt =
                isp_day_night_switch_drop_frame_num_1;
            
            if (*(*($s0 + 0x1bc) + 0x40a4) != $v1_10)
                $a0_3 = *($s0 + 0x1bc);
            else
            {
                system_reg_write(0x6030, 0xff008080);
                $a0_3 = *($s0 + 0x1bc);
            }
            
            if ($a0_3)
                (*($a0_3 + 0x40cc))($a0_3, 0x4000003, 0);
            
            *((int32_t*)((char*)$s0 + 0x178)) = 0; // Fixed void pointer dereference
            data_ca570_2 = 1;
            $v0_35 = *($s0 + 0x134);
        }
        else
        {
            int32_t $a1_1;
                    goto label_79b2c;
            
            if ($v1_10 != 2)
            {
                if ($v1_10 == 3)
                {
                    $a1_1 = 0xff008080;
                }
                
                $v0_35 = *($s0 + 0x134);
            }
            else
            {
                $a1_1 = 0xff00ff00;
            label_79b2c:
                system_reg_write(0x6030, $a1_1);
                *((int32_t*)((char*)$s0 + 0x178)) = 0; // Fixed void pointer dereference
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
                *((int32_t*)((char*)$s0 + 0x11c)) = 0; // Fixed void pointer dereference
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
        int32_t* $s5_1 = (int32_t*)((char*)$s0  + 0x150); // Fixed void pointer arithmetic
        int32_t $v0_37 = *(arg1 + 0xb8);
            int32_t var_38_2 = *($v0_37 + 0x9a74);
            int32_t var_34_2 = *($v0_37 + 0x9a8c);
            int32_t var_28_2 = 0;
            int32_t var_30_2 = *($v0_37 + 0x9a90);
            int32_t var_2c_2 = 0;
        
        while (!(*($v0_37 + 0x9a7c) & 1))
        {
            
            if (!isp_ch1_dequeue_delay_time)
            {
                tx_isp_send_event_to_remote(*($s5_1 + 0x13c), 0x3000006, &var_40);
                $v0_37 = *(arg1 + 0xb8);
            }
            else
            {
                memcpy(&ch1_buf, &var_40, 0x1c);
                private_schedule_work(&ch1_frame_dequeue_delay);
                $v0_37 = *(arg1 + 0xb8);
            }
        }
        
        $v0_39 = $s1 & 4;
    }
    
    if ($v0_39)
    {
        int32_t* $s4_1 = (int32_t*)((char*)$s0  + 0x150); // Fixed void pointer arithmetic
        int32_t $v0_42 = *(arg1 + 0xb8);
            int32_t var_38_3 = *($v0_42 + 0x9b74);
            int32_t var_34_3 = *($v0_42 + 0x9b8c);
            int32_t var_28_3 = 0;
            int32_t var_30_3 = *($v0_42 + 0x9b90);
            int32_t var_2c_3 = 0;
        
        while (!(*($v0_42 + 0x9b7c) & 1))
        {
            tx_isp_send_event_to_remote(*($s4_1 + 0x200), 0x3000006, &var_40);
            $v0_42 = *(arg1 + 0xb8);
        }
    }
    
    char* $s2_1 = (char*)(&irq_func_cb); // Fixed void pointer assignment
    int32_t i = 0;
    int32_t result = 1;
    
    do
    {
        int32_t $v0_46 = 1 << (i & 0x1f) & $s1;
            int32_t $v0_47 = *$s2_1;
                int32_t result_1 = $v0_47();
        i += 1;
        
        if ($v0_46)
        {
            
            if ($v0_47)
            {
                
                if (result_1 != 1)
                    result = result_1;
            }
        }
        
        $s2_1 += 4;
    } while ((uintptr_t)i != 0x20);
    
    return result;
}

