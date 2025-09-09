#include "include/main.h"


  int32_t tisp_wdr_expTime_updata()

{
    param_ratioPara_software_in_array = (data_c46f8 * dmsc_uu_thres_wdr_array + 0x200) >> 0xa;
    data_b1ee8 = (data_c46b8 * data_c46b0 + 0x200) >> 0xa;
    return 0;
}

