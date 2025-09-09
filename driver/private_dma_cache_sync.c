#include "include/main.h"

void  private_dma_cache_sync(struct device *dev, void *vaddr, size_t size,
       enum dma_data_direction direction)
  {
    dma_cache_sync(dev, vaddr, size, direction);
  }
