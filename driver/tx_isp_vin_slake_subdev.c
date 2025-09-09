#include "include/main.h"


  int32_t tx_isp_vin_slake_subdev(void* arg1)

{
    int32_t $v0_1 = *(arg1 + 0xf8);
    
    if ($v0_1)
    {
        *((int32_t*)((char*)arg1 + 0xf8)) = $v0_1 - 1; // Fixed void pointer dereference
        $v0_1 = *(arg1 + 0xf8);
    }
    
    if ($v0_1)
        return 0;
    
    int32_t entry_a2_6;
    
    if (*(arg1 + 0xf4) == 4)
        entry_a2_7 = vin_s_stream(arg1, 0);
    
    int32_t $v0_2;
    
    if (*(arg1 + 0xf4) != 3)
        $v0_2 = *(arg1 + 0xe4);
    else
    {
        entry_$a2 = tx_isp_vin_init(arg1, 0);
        $v0_2 = *(arg1 + 0xe4);
    }
    
    if ($v0_2)
    {
        int32_t var_18 = 0xffffffff;
        subdev_sensor_ops_set_input(arg1, &var_18, entry_$a2);
    }
    
    if (*(arg1 + 0xdc) != arg1 + 0xdc)
        subdev_sensor_ops_release_all_sensor(arg1);
    
    private_mutex_lock(arg1 + 0xe8);
    
    if (*(arg1 + 0xf4) == 2)
        *((int32_t*)((char*)arg1 + 0xf4)) = 1; // Fixed void pointer dereference
    
    private_mutex_unlock(arg1 + 0xe8);
    return 0;
}

