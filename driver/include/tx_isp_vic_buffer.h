#ifndef __TX_ISP_VIC_BUFFER_H__
#define __TX_ISP_VIC_BUFFER_H__

#include <linux/list.h>
#include <linux/types.h>

/*
 * VIC Buffer Entry Structure
 *
 * Matches Binary Ninja reference layout: buffer_addr at +0x08 from struct start
 * (after list_head). Keep 4-byte alignment for MIPS safety.
 */
struct vic_buffer_entry {
    struct list_head list;    /* 0x00: list linkage */
    u32 buffer_addr;          /* 0x08: physical buffer address */
    u32 buffer_index;         /* 0x0C: optional index */
    u32 buffer_status;        /* 0x10: status */
    u32 reserved[13];         /* padding to match reference size */
} __attribute__((aligned(4)));

#define VIC_BUFFER_ENTRY_FROM_LIST(list_ptr) \
    container_of(list_ptr, struct vic_buffer_entry, list)

#define VIC_BUFFER_ALLOC() \
    kzalloc(sizeof(struct vic_buffer_entry), GFP_KERNEL)

#define VIC_BUFFER_ALLOC_ATOMIC() \
    kzalloc(sizeof(struct vic_buffer_entry), GFP_ATOMIC)

/* Buffer status values */
#define VIC_BUFFER_STATUS_FREE      0
#define VIC_BUFFER_STATUS_QUEUED    1
#define VIC_BUFFER_STATUS_ACTIVE    2
#define VIC_BUFFER_STATUS_DONE      3

#endif /* __TX_ISP_VIC_BUFFER_H__ */

