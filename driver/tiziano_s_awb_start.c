#include "include/main.h"


  int32_t tiziano_s_awb_start(int32_t arg1, int32_t arg2)

{
    uint32_t tparams_day_1 = tparams_day;
    int32_t arg_4 = arg2;
    int32_t _AwbPointPos_1 = *_AwbPointPos;
    int32_t arg_0 = arg1;
    *(((void**)((char*)tparams_day_1 + 0x10e8))) = arg1; // Fixed void pointer dereference
    *(((void**)((char*)tparams_day_1 + 0x10ec))) = arg2; // Fixed void pointer dereference
    *(((void**)((char*)tparams_day_1 + 0x10f0))) = arg1; // Fixed void pointer dereference
    *(((void**)((char*)tparams_day_1 + 0x10f4))) = arg2; // Fixed void pointer dereference
    data_a9f94 = arg2;
    data_a9f9c = arg2;
    data_a9f90 = arg1;
    data_a9f98 = arg1;
    /* tailcall */
    return Tiziano_awb_set_gain(&_awb_mf_para, _AwbPointPos_1, &_wb_static);
}

