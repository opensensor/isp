#include "include/main.h"


  int32_t defog_itp(int32_t arg1, int32_t arg2, int32_t arg3) __attribute__((pure))

{
        int32_t $lo_4;
        int32_t $hi_3;
    if ((uintptr_t)arg1 < 0x80)
    {
        $hi_3 = HIGHD(arg1 * arg3 + (0x80 - arg1) * 0x64);
        $lo_4 = LOWD(arg1 * arg3 + (0x80 - arg1) * 0x64);
        return $lo_4 >> 7;
    }
    
    int32_t $lo_3;
    int32_t $hi_2;
    $hi_2 = HIGHD((0x180 - arg1) * arg3 + arg2 * 0x64 / 0xff * (arg1 - 0x80));
    $lo_3 = LOWD((0x180 - arg1) * arg3 + arg2 * 0x64 / 0xff * (arg1 - 0x80));
    return $lo_3 >> 8;
}

