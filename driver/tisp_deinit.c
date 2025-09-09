#include "include/main.h"


  int32_t tisp_deinit()

{
    int32_t $a0 = data_b2f3c;
    tisp_param_operate_deinit();
    tisp_event_exit();
    int32_t $a0_1;
    
    if (!$a0)
        $a0_1 = data_b2f54;
    else
    {
        private_kfree($a0);
        data_b2f3c = 0;
        $a0_1 = data_b2f54;
    }
    
    int32_t $a0_2;
    
    if (!$a0_1)
        $a0_2 = data_b2f6c_1;
    else
    {
        private_kfree($a0_1);
        data_b2f54 = 0;
        $a0_2 = data_b2f6c;
    }
    
    int32_t $a0_3;
    
    if (!$a0_2)
        $a0_3 = data_b2f78_1;
    else
    {
        private_kfree($a0_2);
        data_b2f6c = 0;
        $a0_3 = data_b2f78;
    }
    
    int32_t $a0_4;
    
    if (!$a0_3)
        $a0_4 = data_b2f84_1;
    else
    {
        private_kfree($a0_3);
        data_b2f78 = 0;
        $a0_4 = data_b2f84;
    }
    
    int32_t $a0_5;
    
    if (!$a0_4)
        $a0_5 = data_b2f90_1;
    else
    {
        private_kfree($a0_4);
        data_b2f84 = 0;
        $a0_5 = data_b2f90;
    }
    
    int32_t $a0_6;
    
    if (!$a0_5)
        $a0_6 = data_b2f9c_1;
    else
    {
        private_kfree($a0_5);
        data_b2f90 = 0;
        $a0_6 = data_b2f9c;
    }
    
    if ($a0_6)
    {
        private_kfree($a0_6);
        data_b2f9c = 0;
    }
    
    uint32_t tparams_day_1 = tparams_day;
    
    if (tparams_day_1)
    {
        private_vfree(tparams_day_1);
        tparams_day = 0;
    }
    
    uint32_t tparams_night_1 = tparams_night;
    
    if (tparams_night_1)
    {
        private_vfree(tparams_night_1);
        tparams_night = 0;
    }
    
    uint32_t tparams_cust_1 = tparams_cust;
    
    if (tparams_cust_1)
    {
        private_vfree(tparams_cust_1);
        tparams_cust = 0;
    }
    
    tisp_deinit_free();
    return 0;
}

