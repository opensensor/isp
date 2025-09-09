#include "include/main.h"


  int32_t tisp_ae_state_get(char* arg1)

{
    *(((void**)((char*)arg1 + 4))) = _ae_stat; // Fixed void pointer dereference
    *arg1 = 0 < data_afcbc ? 1 : 0;
    *(((void**)((char*)arg1 + 8))) = data_afcc0; // Fixed void pointer dereference
    return 0;
}

