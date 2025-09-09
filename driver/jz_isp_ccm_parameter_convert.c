#include "include/main.h"


  int32_t jz_isp_ccm_parameter_convert()

{
    int32_t $t9 = jz_isp_ccm_reg2par(&_ccm_a_parameter, tiziano_ccm_a_now);
    int32_t $t9_1 = $t9(&_ccm_t_parameter, tiziano_ccm_t_now);
    /* tailcall */
    return $t9_1(&_ccm_d_parameter, tiziano_ccm_d_now);
}

