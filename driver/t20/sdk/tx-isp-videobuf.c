/* The vendor default reserves 4 MiB for all MMAP capture buffers.  A single
 * page-aligned 1080p NV12 frame consumes roughly 3 MiB, leaving the legacy
 * queue unable to grant the two buffers required to overlap capture and
 * encoding.  Keep the SDK implementation and ABI, but size this driver's
 * sensor-neutral capture pool for two full-resolution frames plus the down-
 * scaled channels. */
#include "source/tx-isp-videobuf.h"
#undef TX_ISP_FRAME_CHANNEL_BUFFER_MAX
#define TX_ISP_FRAME_CHANNEL_BUFFER_MAX (8 * 1024 * 1024)
#include "source/tx-isp-videobuf.c"
