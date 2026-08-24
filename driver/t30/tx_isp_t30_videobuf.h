#ifndef TX_ISP_T30_VIDEOBUF_H
#define TX_ISP_T30_VIDEOBUF_H

void isp_mem_init(void);
unsigned int isp_malloc_buffer(unsigned int size);
void isp_free_buffer(unsigned int addr);

#endif /* TX_ISP_T30_VIDEOBUF_H */
