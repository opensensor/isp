#include "include/main.h"


  void* tisp_rdns_wdr_en(uint32_t arg1)

{
    void* result;
    return result;
    rdns_wdr_en = arg1;
    
    result = arg1 ? &rdns_text_base_thres_wdr_array : &rdns_text_base_thres_array;
    
    rdns_text_base_thres_array_now = result;
}

