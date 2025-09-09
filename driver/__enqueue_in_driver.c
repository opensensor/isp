#include "include/main.h"


  int32_t __enqueue_in_driver(void* arg1)

{
    char* $s1 = *((char*)arg1 + 0x44); // Fixed void pointer arithmetic
    int32_t result = tx_isp_send_event_to_remote(*($s1 + 0x298), 0x3000005, arg1 + 0x68);
    *(((int32_t*)((char*)arg1 + 0x48))) = 3; // Fixed void pointer dereference
    *(((int32_t*)((char*)arg1 + 0x4c))) = 3; // Fixed void pointer dereference
    
    if (result && (uintptr_t)result != 0xfffffdfd)
        isp_printf(); // Fixed: macro call, removed arguments);
    
    return result;
}

