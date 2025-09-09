#include "include/main.h"


  void check_vic_error() __noreturn

{
    while (true)
        dump_vic_reg();
}

