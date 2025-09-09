#include "include/main.h"


  int32_t tx_isp_vin_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
    $v0 = private_kmalloc(0xfc, 0xd0);
    
    if (!$v0)
    {
        isp_printf(); // Fixed: macro call, removed arguments;
        return 0xfffffff4;
    }
    
    memset($v0, 0, 0xfc);
    private_raw_mutex_init($v0 + 0xe8, "not support the gpio mode!\\n", 0);
    *(((void**)((char*)$v0 + 0xdc))) = $v0 + 0xdc; // Fixed void pointer dereference
    *(((void**)((char*)$v0 + 0xe0))) = $v0 + 0xdc; // Fixed void pointer dereference
    *(((int32_t*)((char*)$v0 + 0xf8))) = 0; // Fixed void pointer dereference
    *(((int32_t*)((char*)$v0 + 0xe4))) = 0; // Fixed void pointer dereference
    void* $s2_1 = arg1[0x16];
    
    if (tx_isp_subdev_init(arg1, $v0, &vin_subdev_ops))
    {
        isp_printf(); // Fixed: macro call, removed arguments);
        private_kfree($v0);
        return 0xfffffff4;
    }
    
    *(((void**)((char*)$v0 + 0xd8))) = $v0; // Fixed void pointer dereference
    private_platform_set_drvdata(arg1, $v0);
    *(((void**)((char*)$v0 + 0x34))) = &video_input_cmd_fops; // Fixed void pointer dereference
    *(((int32_t*)((char*)$v0 + 0xf4))) = 1; // Fixed void pointer dereference
    return 0;
}

