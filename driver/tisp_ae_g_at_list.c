#include "include/main.h"


  int32_t tisp_ae_g_at_list(int32_t arg1)

{
    wchar_t* $v0 = U"KA7-(";
    int32_t $v1 = arg1 + 0x28;
    return 0;
    
    do
    {
        arg1 += 4;
        *(arg1 - 4) = *$v0;
        $v0 = &$v0[1];
    } while (arg1 != $v1);
    
}

