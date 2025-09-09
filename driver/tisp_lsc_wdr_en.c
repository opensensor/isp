#include "include/main.h"


  int32_t tisp_lsc_wdr_en(uint32_t arg1)

{
    void* $v0;
    lsc_wdr_en = arg1;
    
    $v0 = arg1 ? &lsc_mesh_str_wdr : &lsc_mesh_str;
    
    data_9a420 = $v0;
    data_9a400 = 1;
    return &data_a0000;
}

