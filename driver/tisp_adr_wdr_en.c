#include "include/main.h"


  int32_t tisp_adr_wdr_en(uint32_t arg1)

{
    void* $v0;
    adr_wdr_en = arg1;
    
    if (arg1)
    {
        adr_ctc_map2cut_y_now = &adr_ctc_map2cut_y_wdr;
        adr_light_end_now = &adr_light_end_wdr;
        adr_block_light_now = &adr_block_light_wdr;
        adr_map_mode_now = &adr_map_mode_wdr;
        adr_ev_list_now = &adr_ev_list_wdr;
        adr_ligb_list_now = &adr_ligb_list_wdr;
        adr_mapb1_list_now = &adr_mapb1_list_wdr;
        adr_mapb2_list_now = &adr_mapb2_list_wdr;
        adr_mapb3_list_now = &adr_mapb3_list_wdr;
        adr_mapb4_list_now = &adr_mapb4_list_wdr;
        $v0 = &adr_blp2_list_wdr;
    }
    else
    {
        adr_ctc_map2cut_y_now = &adr_ctc_map2cut_y;
        adr_light_end_now = &adr_light_end;
        adr_block_light_now = &adr_block_light;
        adr_map_mode_now = &adr_map_mode;
        adr_ev_list_now = &adr_ev_list;
        adr_ligb_list_now = &adr_ligb_list;
        adr_mapb1_list_now = &adr_mapb1_list;
        adr_mapb2_list_now = &adr_mapb2_list;
        adr_mapb3_list_now = &adr_mapb3_list;
        adr_mapb4_list_now = &adr_mapb4_list;
        $v0 = &adr_blp2_list;
    }
    
    adr_blp2_list_now = $v0;
    tiziano_adr_params_refresh();
    /* tailcall */
    return tiziano_adr_params_init();
}

