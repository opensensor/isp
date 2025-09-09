#include "include/main.h"


  int32_t tisp_tgain_update(uint32_t arg1)

{
    tisp_gib_gain_interpolation(arg1);
    tisp_gb_blc_again_interp(arg1, 0);
    tisp_dmsc_refresh(arg1);
    tisp_sharpen_refresh(arg1);
    tisp_sdns_refresh(arg1);
    tisp_dpc_refresh(arg1);
    tisp_lsc_gain_update(arg1);
    tisp_ydns_gain_update(arg1);
    tisp_rdns_gain_update(arg1);
    tisp_mdns_refresh(arg1);
    return 0;
}

