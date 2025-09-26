#ifndef __TX_ISP_VIC_BUFFER_H__
#define __TX_ISP_VIC_BUFFER_H__

#include <linux/list.h>
#include <linux/types.h>

/**
 * VIC Buffer Entry Structure
 * 
 * This structure matches the Binary Ninja decompiled reference driver
 * buffer layout. The buffer address is at offset 0x8 from the structure
 * start, which corresponds to the buffer_addr field after the list_head.
 * 
 * CRITICAL: This structure must be properly aligned to prevent MIPS
 * unaligned access errors that cause kernel panics.
 */
struct vic_buffer_entry {
    struct list_head list;       /* 0x00: List linkage (8 bytes on 32-bit) */
    uint32_t buffer_addr;        /* 0x08: Buffer physical address (Binary Ninja offset) */
    uint32_t buffer_index;       /* 0x0C: Buffer index */
    uint32_t buffer_status;      /* 0x10: Buffer status */
    uint32_t reserved[13];       /* 0x14-0x47: Padding to match reference driver size */
} __attribute__((aligned(4)));

/**
 * Helper macros for safe buffer entry access
 */
#define VIC_BUFFER_ENTRY_FROM_LIST(list_ptr) \
    container_of(list_ptr, struct vic_buffer_entry, list)

#define VIC_BUFFER_ALLOC() \
    kzalloc(sizeof(struct vic_buffer_entry), GFP_KERNEL)

#define VIC_BUFFER_ALLOC_ATOMIC() \
    kzalloc(sizeof(struct vic_buffer_entry), GFP_ATOMIC)

/**
 * Buffer status values
 */
#define VIC_BUFFER_STATUS_FREE      0
#define VIC_BUFFER_STATUS_QUEUED    1
#define VIC_BUFFER_STATUS_ACTIVE    2
#define VIC_BUFFER_STATUS_DONE      3

#endif /* __TX_ISP_VIC_BUFFER_H__ */
