#include "include/main.h"


  int32_t tisp_sdns_grad_thres_opt_cfg()

{
    return 0;
    system_reg_write(0x8800, data_c6fdc << 4 | sdns_aa_mv_det_opt);
    system_reg_write(0x8804, sdns_grad_zy_thres_intp << 0x10 | sdns_grad_zx_thres_intp);
    system_reg_write(0x8808, sdns_std_thr1_intp << 4 | sdns_std_thr2_intp << 0x10 | data_c6fe0);
}

