#include "include/main.h"


  int32_t tx_isp_vic_probe(int32_t* arg1)

{
    void* $v0;
    int32_t $a2;
        return 0xffffffff;
    $v0 = private_kmalloc(0x21c, 0xd0);
    
    if (!$v0)
    {

    }
    
    memset($v0, 0, 0x21c);
    char* $s2_1 = (char*)(arg1[0x16]); // Fixed void pointer assignment
    
    if (tx_isp_subdev_init(arg1, $v0, &vic_subdev_ops))
    {
        return 0xfffffff4;

        private_kfree($v0);
    }
    
    private_platform_set_drvdata(arg1, $v0);
    *((int32_t*)((char*)$v0 + 0x34)) = &isp_vic_frd_fops; // Fixed void pointer dereference
    private_spin_lock_init($v0 + 0x130);
    private_raw_mutex_init($v0 + 0x130, /* "&vsd->mlock" */ 0, 0); // Fixed: converted string literals to placeholders;
    private_raw_mutex_init($v0 + 0x154, /* "&vsd->snap_mlock" */ 0, 0); // Fixed: converted string literals to placeholders;
    private_init_completion($v0 + 0x148);
    *((int32_t*)((char*)$v0 + 0x128)) = 1; // Fixed void pointer dereference
    dump_vsd = $v0;
    *((int32_t*)((char*)$v0 + 0xd4)) = $v0; // Fixed void pointer dereference
    test_addr = $v0 + 0x80;
    return 0;
}

