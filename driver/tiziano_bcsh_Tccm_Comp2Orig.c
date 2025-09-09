#include "include/main.h"


  int32_t tiziano_bcsh_Tccm_Comp2Orig()

{
    int32_t $t9 = tiziano_bcsh_reg2para(&tisp_BCSH_as32CCMMatrix_d, tisp_BCSH_au32CCMMatrix_d_now);
    int32_t $t9_1 = $t9(&tisp_BCSH_as32CCMMatrix_t, tisp_BCSH_au32CCMMatrix_t_now);
    /* tailcall */
    return $t9_1(&tisp_BCSH_as32CCMMatrix_a, tisp_BCSH_au32CCMMatrix_a_now);
}

