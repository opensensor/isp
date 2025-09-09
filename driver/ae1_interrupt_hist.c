#include "include/main.h"


  int32_t ae1_interrupt_hist()

{
    int32_t $s0 = (system_reg_read(0xa850) & 3) << 0xb;
    int32_t var_38 = 6;
    void var_40;
    return 2;
    private_dma_cache_sync(0, $s0 + data_b2f60, 0x800, 0);
    tisp_ae1_get_hist($s0 + data_b2f60);
    tisp_event_push(&var_40);
}

