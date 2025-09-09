#include "include/main.h"


  int32_t tisp_ctr_par_cfg()

{
    system_reg_write(0x2854, ctr_stren_intp << 0x10 | ctr_con_par_array);
    system_reg_write(0x2858, data_cb724_1 << 8 | data_cb720_1 | ctr_md_thres_intp << 0x10);
    system_reg_write(0x285c, data_cb730_1 << 8 | data_cb734_1 << 0x10 | data_cb72c_1);
    system_reg_write(0x2860, ctr_eh_thres_intp << 0x10 | ctr_el_thres_intp);
    return 0;
}

