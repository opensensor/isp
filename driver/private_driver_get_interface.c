#include "include/main.h"


  int32_t private_driver_get_interface()

{
    private_get_driver_interface(&pfaces);
    return -((pfaces < 1 ? 1 : 0));
}

