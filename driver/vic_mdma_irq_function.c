#include "include/main.h"


  int32_t* vic_mdma_irq_function(void* arg1, int32_t arg2)

{
        int32_t $s0_2 = *(arg1 + 0xdc) * *(arg1 + 0xe0);
        int32_t $s0_3 = $s0_2 << 1;
            uint32_t vic_mdma_ch0_sub_get_num_1 = vic_mdma_ch0_sub_get_num;
                uint32_t vic_mdma_ch1_set_buff_index_1 = vic_mdma_ch1_set_buff_index;
                char* $a2_9 = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
                uint32_t $hi_2 = (vic_mdma_ch1_set_buff_index_1 + 1) % 5;
                int32_t $s0_5 = $s0_3 + *($a2_9 + ((vic_mdma_ch1_set_buff_index_1 + 0xc6) << 2));
    int32_t* vic_mdma_ch1_sub_get_num_1;
    
    if (!*(arg1 + 0x214))
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        
        if (arg2)
        {
            if (arg2 != 1)
                goto label_12898;
            
            
            if (vic_mdma_ch1_sub_get_num)
            {
                vic_mdma_ch1_set_buff_index = $hi_2;
                *($a2_9 + (($hi_2 + 0xc6) << 2)) = $s0_5;
                vic_mdma_ch1_sub_get_num -= 1;
            label_12898:
                vic_mdma_ch0_sub_get_num_1 = vic_mdma_ch0_sub_get_num;
            }
            
            vic_mdma_ch1_sub_get_num_1 = &data_b0000_1;
            
            if (!vic_mdma_ch0_sub_get_num_1)
                goto label_12770;
        }
        else if (vic_mdma_ch0_sub_get_num)
        {
            uint32_t vic_mdma_ch0_set_buff_index_1 = vic_mdma_ch0_set_buff_index;
            char* $a2_8 = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
            uint32_t $hi_1 = (vic_mdma_ch0_set_buff_index_1 + 1) % 5;
            int32_t $s0_4 = $s0_3 + *($a2_8 + ((vic_mdma_ch0_set_buff_index_1 + 0xc6) << 2));
            uint32_t $v0_28 = vic_mdma_ch0_sub_get_num - 1;
            char* $a0_16 = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
            vic_mdma_ch0_set_buff_index = $hi_1;
            *($a2_8 + (($hi_1 + 0xc6) << 2)) = $s0_4;
            vic_mdma_ch0_sub_get_num = $v0_28;
            
            if ($v0_28 != 7)
                goto label_12898;
            
            vic_mdma_ch1_sub_get_num_1 = (*($a0_16 + 0x300) & 0xfff0ffff) | 0x70000;
            *(((void**)((char*)$a0_16 + 0x300))) = vic_mdma_ch1_sub_get_num_1; // Fixed void pointer dereference
        }
        else
        {
        label_12770:
            vic_mdma_ch1_sub_get_num_1 = vic_mdma_ch1_sub_get_num;
            
            if (!vic_mdma_ch1_sub_get_num_1)
                /* tailcall */
                return private_complete(arg1 + 0x148);
        }
    }
    else
    {
            int32_t $s5_1 = *(*(arg1 + 0xb8) + 0x380);
            int32_t* $v0_2 = pop_buffer_fifo(arg1 + 0x204);
            int32_t $a2_1 = *(arg1 + 0x218);
        vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x210);
        
        if (vic_mdma_ch1_sub_get_num_1)
        {
            
            if (!$v0_2)
            {
                isp_printf(); // Fixed: macro call, removed arguments;
                vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x1fc);
            }
            else
            {
                uint32_t raw_pipe_1 = raw_pipe;
                    int32_t $v0_5 = *(arg1 + 0x218);
                    int32_t $s2_2 = 0;
                *(((void**)((char*)arg1 + 0x218))) = $a2_1 - 1; // Fixed void pointer dereference
                (*(raw_pipe_1 + 4))(*(raw_pipe_1 + 0x14), $v0_2);
                int32_t** $v0_3 = *(arg1 + 0x200);
                *(((void**)((char*)arg1 + 0x200))) = $v0_2; // Fixed void pointer dereference
                $v0_2[1] = $v0_3;
                *$v0_2 = arg1 + 0x1fc;
                *$v0_3 = $v0_2;
                
                if ($v0_2[2] == $s5_1)
                    vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x1fc);
                else
                {
                    
                    while (true)
                    {
                        if ($s2_2 == $v0_5)
                        {
                            isp_printf(2, "function: %s ; vic dma addrrss error!!!\n", 
                                "vic_mdma_irq_function");
                            isp_printf(2, "VIC_ADDR_DMA_CONTROL : 0x%x\n", 
                                *(*(arg1 + 0xb8) + 0x300));
                            vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x1fc);
                            break;
                        }
                        
                        int32_t* $v0_7 = pop_buffer_fifo(arg1 + 0x204);
                        int32_t $v0_8 = *(arg1 + 0x218);
                        
                        if (!$v0_7)
                        {
                            int32_t var_3c_1 = $v0_8;
                            int32_t var_40_2 = $v0_5;
                            isp_printf(); // Fixed: macro call, removed arguments;
                            $s2_2 += 1;
                        }
                        else
                        {
                            int32_t var_40_1 = $v0_7[2];
                            uint32_t raw_pipe_2 = raw_pipe;
                            *(((void**)((char*)arg1 + 0x218))) = $v0_8 - 1; // Fixed void pointer dereference
                            isp_printf(); // Fixed: macro call, removed arguments;
                            (*(raw_pipe_2 + 4))(*(raw_pipe_2 + 0x14), $v0_7);
                            int32_t** $v0_11 = *(arg1 + 0x200);
                            *(((void**)((char*)arg1 + 0x200))) = $v0_7; // Fixed void pointer dereference
                            $v0_7[1] = $v0_11;
                            *$v0_7 = arg1 + 0x1fc;
                            *$v0_11 = $v0_7;
                            
                            if ($v0_7[2] == $s5_1)
                            {
                                vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x1fc);
                                break;
                            }
                            
                            $s2_2 += 1;
                        }
                    }
                }
            }
            
            if (vic_mdma_ch1_sub_get_num_1 != arg1 + 0x1fc)
            {
                    char* $v1_1 = *((char*)arg1 + 0xb8); // Fixed void pointer arithmetic
                vic_mdma_ch1_sub_get_num_1 = *(arg1 + 0x1f4);
                
                if (arg1 + 0x1f4 != vic_mdma_ch1_sub_get_num_1)
                {
                    pop_buffer_fifo(arg1 + 0x1f4);
                    int32_t* $v0_15;
                    void* $a1_2;
                    $v0_15 = pop_buffer_fifo(arg1 + 0x1fc);
                    $v0_15[2] = *($a1_2 + 8);
                    int32_t** $a0_7 = *(arg1 + 0x208);
                    *(((void**)((char*)arg1 + 0x208))) = $v0_15; // Fixed void pointer dereference
                    *$v0_15 = arg1 + 0x204;
                    $v0_15[1] = $a0_7;
                    *$a0_7 = $v0_15;
                    *(arg1 + 0x218) += 1;
                    vic_mdma_ch1_sub_get_num_1 = $v1_1 + (($v0_15[4] + 0xc6) << 2);
                    *vic_mdma_ch1_sub_get_num_1 = $v0_15[2];
                }
            }
        }
    }
    
    return vic_mdma_ch1_sub_get_num_1;
}

