#include "include/main.h"


  int32_t tisp_ae_state_get(char* arg1)

{
    *(arg1 + 4) = _ae_stat;
    *arg1 = 0 < data_afcbc_1 ? 1 : 0;
    *(arg1 + 8) = data_afcc0_1;
    return 0;
}

