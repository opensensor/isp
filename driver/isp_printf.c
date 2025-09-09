#include "include/main.h"

void* vic_framedone_irq_function(void* arg1)
{
    char* result = (char*)(&data_b0000);

    if (!*(int32_t*)((char*)arg1 + 0x214))
    {
        label_123f4:

            if (gpio_switch_state)
            {
                gpio_switch_state = 0;

                for (int32_t i = 0; i < 10; i++)
                {
                    char* gpio_ptr = (char*)(&gpio_info) + (i * 8);
                    uint32_t gpio_num = *(uint32_t*)gpio_ptr;

                    if (gpio_num == 0xff)
                        break;

                    result = (char*)private_gpio_direction_output(gpio_num, *(uint32_t*)(gpio_ptr + 0x14));

                    if ((intptr_t)result < 0)
                    {
                        return result;
                    }
                }
            }
    }
    else
    {
        int32_t* base_ptr = (int32_t*)((char*)arg1 + 0xb8);
        int32_t count = 0;
        int32_t found_count = 0;
        int32_t found = 0;

        result = (char*)(*(intptr_t*)((char*)arg1 + 0x210));

        if (result)
        {
            void** list_head = *(void***)((char*)arg1 + 0x204);

            for (void** node = list_head; node != (void**)((char*)arg1 + 0x204); node = (void**)*node)
            {
                found_count += (found > 0) ? 1 : 0;
                count += 1;

                if (node[2] == (void*)*(base_ptr + 0x380))
                    found = 1;
            }

            int32_t value = found ? (found_count << 0x10) : (count << 0x10);

            *(base_ptr + 0x300) = value | (*(base_ptr + 0x300) & 0xfff0ffff);
            result = (char*)&data_b0000;
            goto label_123f4;
        }
    }

    return result;
}