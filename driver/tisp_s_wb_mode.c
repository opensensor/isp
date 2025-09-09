#include "include/main.h"


  int32_t tisp_s_wb_mode(int32_t arg1, int32_t arg2, int32_t arg3)

{
    int32_t arg_0 = arg1;
    int32_t arg_4 = arg2;
    int32_t arg_8 = arg3;
    int32_t $a3;
    int32_t arg_c = $a3;
    int32_t $v1_2;
    
    if ((uintptr_t)arg1 >= 0xa)

    else
        switch (arg1)
        {
            case 0:
            {
                tisp_wb_attr = 0;
                break;
            }
            case 1:
            {
                $v1_2 = 1;
            label_29490:
                tisp_wb_attr = $v1_2;
                data_b5a38 = arg2;
                data_b5a3c = arg3;
                break;
            }
            case 2:
            {
                tisp_wb_attr = 2;
                data_b5a38 = 0x180;
                data_b5a3c = 0x180;
                break;
            }
            case 3:
            {
                tisp_wb_attr = 3;
                data_b5a38 = 0x1b6;
                data_b5a3c = 0x12f;
                break;
            }
            case 4:
            {
                tisp_wb_attr = 4;
                data_b5a38 = 0xdb;
                data_b5a3c = 0x2b2;
                break;
            }
            case 5:
            {
                tisp_wb_attr = 5;
                data_b5a38 = 0xf0;
                data_b5a3c = 0x234;
                break;
            }
            case 6:
            {
                tisp_wb_attr = 6;
                data_b5a38 = 0x13b;
                data_b5a3c = 0x1cb;
                break;
            }
            case 7:
            {
                tisp_wb_attr = 7;
                data_b5a38 = 0x1d4;
                data_b5a3c = 0x117;
                break;
            }
            case 8:
            {
                tisp_wb_attr = 8;
                data_b5a38 = 0xf0;
                data_b5a3c = 0x178;
                break;
            }
            case 9:
            {
                goto label_29490;
                $v1_2 = 9;
            }
        }
    awb_moa = 1;
    return 0;
}

