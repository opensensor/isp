#include "include/main.h"


  uint32_t sensor_get_id()

{
    return *(*(g_ispcore + 0x120) + 4);
}

