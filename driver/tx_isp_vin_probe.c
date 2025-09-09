#include "include/main.h"


  int32_t tx_isp_vin_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
        return 0xfffffff4;
    $v0 = private_kmalloc(0xfc, 0xd0);
    
    if (!$v0)
    {

    }
    
    memset($v0, 0, 0xfc);
    private_raw_mutex_init($v0 + 0xe8, /* "not support the gpio mode!\\n" */ 0, 0); // Fixed: converted string literals to placeholders;
    *((int32_t*)((char*)$v0 + 0xdc)) = $v0 + 0xdc; // Fixed void pointer dereference
    *((int32_t*)((char*)$v0 + 0xe0)) = $v0 + 0xdc; // Fixed void pointer dereference
    *((int32_t*)((char*)$v0 + 0xf8)) = 0; // Fixed void pointer dereference
    *((int32_t*)((char*)$v0 + 0xe4)) = 0; // Fixed void pointer dereference
    char* $s2_1 = (char*)(arg1[0x16]); // Fixed void pointer assignment
    
    if (tx_isp_subdev_init(arg1, $v0, &vin_subdev_ops))
    {
        return 0xfffffff4;

        private_kfree($v0);
    }
    
    *((int32_t*)((char*)$v0 + 0xd8)) = $v0; // Fixed void pointer dereference
    private_platform_set_drvdata(arg1, $v0);
    *((int32_t*)((char*)$v0 + 0x34)) = &video_input_cmd_fops; // Fixed void pointer dereference
    *((int32_t*)((char*)$v0 + 0xf4)) = 1; // Fixed void pointer dereference
    return 0;
}

