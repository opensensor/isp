#include "include/main.h"

int32_t ae1_interrupt_hist()
{
    int32_t buffer_offset;
    int32_t var_38 = 6;
    /* Stack space for event data - size based on typical event structures */
    /* Using a byte array to avoid assuming the struct type */
    uint8_t var_40[32];  /* Adjust size if needed based on actual event struct */

    /* Read system register to determine which buffer to use (double buffering) */
    /* Bits 0-1 indicate the buffer index, shift left by 11 (multiply by 2048) */
    buffer_offset = (system_reg_read(0xa850) & 3) << 0xb;

    /* Sync the DMA cache for the histogram buffer before reading */
    /* Parameters:
     * - dev: NULL (not needed for this implementation)
     * - vaddr: Virtual address of the histogram buffer
     * - size: 0x800 bytes (2048 bytes) - size of histogram data
     * - direction: 0 (DMA_FROM_DEVICE) - data is coming from device to CPU
     */
    private_dma_cache_sync(NULL,
                          (void*)(buffer_offset + data_b2f60),
                          0x800,
                          0);

    /* Read the histogram data from the synced buffer */
    tisp_ae1_get_hist((int32_t*)(buffer_offset + data_b2f60));

    /* Push an event - passing the address of stack variable */
    tisp_event_push(&var_40);

    /* Return success code */
    return 2;
}