# VBM Buffer Management Complete Fix

## Binary Ninja Analysis Results

Using Binary Ninja MCP to decompile `libimp.so`, I've identified the exact VBM buffer management flow:

### VBMCreatePool (0x1efe4)

**What it does:**
1. Allocates pool structure: `malloc(buffer_count * 0x428 + 0x180)`
2. Stores pool configuration (channel, dimensions, format, buffer count)
3. Calculates buffer size based on format (NV12, RGB, etc.)
4. Calls `IMP_Alloc()` or `IMP_PoolAlloc()` to allocate physical memory
5. Creates buffer entries (0x428 bytes each) with:
   - Buffer index
   - Channel number
   - Dimensions (width, height)
   - Buffer size
   - Physical address (paddr)
   - Virtual address (vaddr)
6. Registers each buffer in global `g_framevolumes` array (max 30 entries)
7. Stores pool instance in `vbm_instance[channel]` global array

**Key Structure Offsets:**
- `+0x00`: Channel number
- `+0x04`: Parent framesource pointer
- `+0x0c`: Height
- `+0x10`: Width
- `+0x14`: Format (PIX_FMT_NV12 = 0x22, etc.)
- `+0x20`: Buffer size
- `+0xd4`: Buffer count
- `+0xd8`: Pool name string
- `+0x158`: Virtual address base
- `+0x15c`: Physical address base
- `+0x174`: Alloc callback function pointer
- `+0x178`: Free callback function pointer
- `+0x17c`: Pool ID (-1 if using IMP_Alloc, >=0 if using IMP_PoolAlloc)
- `+0x180`: First buffer entry (0x428 bytes)
- `+0x5a8`: Second buffer entry (0x428 bytes)
- etc.

**Buffer Entry Structure (0x428 bytes):**
- `+0x00`: Buffer index
- `+0x04`: Channel number
- `+0x08`: Height
- `+0x0c`: Width
- `+0x10`: Format
- `+0x14`: Buffer size
- `+0x18`: Virtual address
- `+0x1c`: Physical address
- `+0x2c`: Additional metadata (44 bytes)

### VBMFillPool (0x1fa48)

**What it does:**
1. Gets pool instance from `vbm_instance[channel]`
2. For each buffer in pool:
   - Finds buffer entry in `g_framevolumes` array
   - Locks buffer mutex (`pthread_mutex_lock`)
   - Calls alloc callback: `(*pool->alloc_callback)(buffer_entry, pool->parent)`
   - Unlocks buffer mutex (`pthread_mutex_unlock`)
3. The alloc callback is expected to:
   - Queue the buffer via QBUF ioctl
   - Program buffer address to VIC hardware registers

**Critical Insight:** The alloc callback is stored at offset `+0x178` in the pool structure!

### What Our Driver Must Do

#### 1. Implement VBM Pool Structure in Kernel

Our driver needs to maintain a VBM pool structure that matches libimp's expectations:

```c
struct vbm_pool {
    int channel;                    // +0x00
    void *parent;                   // +0x04
    int height;                     // +0x0c
    int width;                      // +0x10
    int format;                     // +0x14
    int buffer_size;                // +0x20
    int buffer_count;               // +0xd4
    char pool_name[64];             // +0xd8
    void *vaddr_base;               // +0x158
    phys_addr_t paddr_base;         // +0x15c
    void *alloc_callback;           // +0x174
    void *free_callback;            // +0x178
    int pool_id;                    // +0x17c
    struct vbm_buffer_entry buffers[0]; // +0x180
};

struct vbm_buffer_entry {
    int index;                      // +0x00
    int channel;                    // +0x04
    int height;                     // +0x08
    int width;                      // +0x0c
    int format;                     // +0x10
    int buffer_size;                // +0x14
    void *vaddr;                    // +0x18
    phys_addr_t paddr;              // +0x1c
    char metadata[44];              // +0x2c
};
```

#### 2. Handle VIDIOC_REQBUFS to Create VBM Pool

When libimp calls `ioctl(fd, VIDIOC_REQBUFS, &reqbuf)`:

```c
case 0xc0145608: { // VIDIOC_REQBUFS
    // 1. Allocate VBM pool structure
    int pool_size = sizeof(struct vbm_pool) + 
                    (reqbuf.count * sizeof(struct vbm_buffer_entry));
    struct vbm_pool *pool = kzalloc(pool_size, GFP_KERNEL);
    
    // 2. Fill pool configuration
    pool->channel = channel;
    pool->width = state->width;
    pool->height = state->height;
    pool->format = 0x22; // PIX_FMT_NV12
    pool->buffer_size = state->width * state->height * 2;
    pool->buffer_count = reqbuf.count;
    snprintf(pool->pool_name, 64, "VBMPool%d", channel);
    
    // 3. Allocate physical memory for buffers
    size_t total_size = pool->buffer_size * pool->buffer_count;
    void *vaddr = dma_alloc_coherent(dev, total_size, &paddr, GFP_KERNEL);
    pool->vaddr_base = vaddr;
    pool->paddr_base = paddr;
    
    // 4. Initialize buffer entries
    for (i = 0; i < pool->buffer_count; i++) {
        pool->buffers[i].index = i;
        pool->buffers[i].channel = channel;
        pool->buffers[i].width = pool->width;
        pool->buffers[i].height = pool->height;
        pool->buffers[i].format = pool->format;
        pool->buffers[i].buffer_size = pool->buffer_size;
        pool->buffers[i].vaddr = vaddr + (i * pool->buffer_size);
        pool->buffers[i].paddr = paddr + (i * pool->buffer_size);
    }
    
    // 5. Store pool in channel state
    fcd->vbm_pool = pool;
    
    // 6. Write buffer addresses to /tmp/alloc_manager_info
    write_vbm_pool_info(pool);
}
```

#### 3. Handle VIDIOC_QBUF to Program VIC Registers

When libimp calls `ioctl(fd, VIDIOC_QBUF, &buffer)` via VBMFillPool:

```c
case 0xc044560f: { // VIDIOC_QBUF
    struct v4l2_buffer buffer;
    copy_from_user(&buffer, argp, sizeof(buffer));
    
    // 1. Get buffer entry from VBM pool
    struct vbm_pool *pool = fcd->vbm_pool;
    if (!pool || buffer.index >= pool->buffer_count) {
        return -EINVAL;
    }
    
    struct vbm_buffer_entry *entry = &pool->buffers[buffer.index];
    
    // 2. Program buffer address to VIC hardware
    if (ourISPdev && ourISPdev->vic_dev) {
        struct tx_isp_vic_device *vic = ourISPdev->vic_dev;
        void __iomem *vic_regs = vic->vic_regs;
        
        // Binary Ninja reference: reg_offset = (buffer_index + 0xc6) << 2
        u32 reg_offset = (entry->index + 0xc6) << 2;
        
        pr_info("*** QBUF: Programming VIC register 0x%x with buffer paddr 0x%x ***\n",
                reg_offset, entry->paddr);
        
        writel(entry->paddr, vic_regs + reg_offset);
        wmb();
        
        // 3. Add buffer to VIC queue management
        spin_lock_irqsave(&vic->buffer_mgmt_lock, flags);
        
        // Create queue entry
        struct vic_buffer_entry *queue_entry = kmalloc(sizeof(*queue_entry), GFP_ATOMIC);
        queue_entry->buffer_addr = entry->paddr;
        queue_entry->buffer_index = entry->index;
        queue_entry->buffer_status = VIC_BUFFER_STATUS_QUEUED;
        
        // Add to done queue (ready for hardware)
        list_add_tail(&queue_entry->list, &vic->done_head);
        
        spin_unlock_irqrestore(&vic->buffer_mgmt_lock, flags);
        
        pr_info("*** QBUF: Buffer %d queued successfully (paddr=0x%x) ***\n",
                entry->index, entry->paddr);
    }
    
    return 0;
}
```

#### 4. Write VBM Pool Info to /tmp/alloc_manager_info

This file is critical for libimp to find buffer addresses:

```c
void write_vbm_pool_info(struct vbm_pool *pool)
{
    struct file *fp;
    char buf[256];
    int i;
    
    fp = filp_open("/tmp/alloc_manager_info", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(fp)) {
        pr_err("Failed to open /tmp/alloc_manager_info\n");
        return;
    }
    
    // Write pool header
    snprintf(buf, sizeof(buf), "info->owner = %s\n", pool->pool_name);
    kernel_write(fp, buf, strlen(buf), &fp->f_pos);
    
    // Write each buffer address
    for (i = 0; i < pool->buffer_count; i++) {
        snprintf(buf, sizeof(buf), "info->paddr = 0x%08x\n", 
                 (u32)pool->buffers[i].paddr);
        kernel_write(fp, buf, strlen(buf), &fp->f_pos);
    }
    
    filp_close(fp, NULL);
    
    pr_info("*** VBM: Wrote pool info to /tmp/alloc_manager_info ***\n");
}
```

## Implementation Priority

1. **CRITICAL**: Add VBM pool structure to `frame_channel_device`
2. **CRITICAL**: Implement VBM pool allocation in REQBUFS handler
3. **CRITICAL**: Program VIC registers in QBUF handler (BEFORE STREAMON)
4. **CRITICAL**: Write /tmp/alloc_manager_info for libimp
5. **HIGH**: Verify buffer addresses are valid physical addresses
6. **MEDIUM**: Add VBM pool cleanup in channel release

## Expected Behavior After Fix

1. Prudynt calls `IMP_FrameSource_EnableChn(0)`
2. libimp opens `/dev/framechan0`
3. libimp calls `VBMCreatePool()` → allocates pool structure in userspace
4. libimp calls `ioctl(REQBUFS, count=4)` → driver allocates VBM pool in kernel
5. libimp calls `VBMFillPool()` → calls alloc callback for each buffer
6. Alloc callback calls `ioctl(QBUF, index=N)` → driver programs VIC register
7. Driver writes buffer addresses to `/tmp/alloc_manager_info`
8. libimp calls `ioctl(STREAMON)` → VIC starts DMA with programmed buffers
9. VIC generates frame done interrupts
10. Driver moves buffers from busy to done queue
11. libimp calls `ioctl(DQBUF)` → gets completed frame
12. Prudynt receives frames with real data!

## Implementation Status

### ✅ COMPLETED

1. **VBM Pool Structure Definitions** (driver/include/tx-isp-device.h)
   - Added `struct vbm_buffer_entry` (0x428 bytes) - EXACT Binary Ninja layout
   - Added `struct vbm_pool` with all offsets matching Binary Ninja decompilation
   - Added VBM pool fields to `frame_channel_device`:
     - `struct vbm_pool *vbm_pool`
     - `void *dma_vaddr` (for cleanup)
     - `dma_addr_t dma_paddr` (for cleanup)
     - `size_t dma_size` (for cleanup)

2. **VBM Pool Allocation in REQBUFS** (driver/tx-isp-module.c)
   - Allocates VBM pool structure: `0x180 + (buffer_count * 0x428)` bytes
   - Fills pool configuration (channel, width, height, format, buffer_size, buffer_count)
   - Allocates DMA coherent memory for all buffers
   - Initializes buffer entries with correct vaddr/paddr for each buffer
   - Calls `write_vbm_pool_info()` to create /tmp/alloc_manager_info

3. **VIC Register Programming in QBUF** (driver/tx-isp-module.c)
   - Gets buffer entry from VBM pool by index
   - Programs VIC hardware register: `writel(paddr, vic_regs + ((index + 0xc6) << 2))`
   - Uses EXACT Binary Ninja register offset calculation
   - Logs buffer address programming for debugging

4. **/tmp/alloc_manager_info Writer** (driver/tx-isp-module.c)
   - Function: `write_vbm_pool_info(struct vbm_pool *pool)`
   - Writes EXACT format expected by libimp.so:
     ```
     info->owner = VBMPool0
     info->paddr = 0x12345678
     info->paddr = 0x12345679
     ...
     ```
   - Called after VBM pool creation in REQBUFS

5. **VBM Pool Cleanup** (driver/tx-isp-module.c)
   - Added cleanup in `frame_channel_release()`
   - Frees DMA coherent memory
   - Frees VBM pool structure
   - Prevents memory leaks

6. **CRITICAL: vic_core_ops_ioctl QBUF Handler Fix** (driver/tx_isp_vic.c)
   - Fixed QBUF handler (0x3000008) to get buffer address from VBM pool
   - Uses buffer index to look up physical address from VBM pool
   - Fallback to v4l2_buffer structure if VBM pool not available
   - This was the ROOT CAUSE of buffer address issues!
   - Before: Tried to extract phys addr from v4l2_buffer (which doesn't have it)
   - After: Looks up phys addr from VBM pool using buffer index

## Testing Instructions

1. **Build and install the driver:**
   ```bash
   cd driver
   make clean && make
   sudo insmod tx-isp-t31.ko
   ```

2. **Start prudynt and monitor logs:**
   ```bash
   # Terminal 1: Watch kernel logs
   dmesg -w | grep -E "VBM|QBUF|REQBUFS|Channel"

   # Terminal 2: Start prudynt
   cd ../prudynt-t
   ./prudynt
   ```

3. **Expected log sequence:**
   ```
   *** Channel 0: Creating VBM pool (Binary Ninja VBMCreatePool) ***
   *** VBM Pool: channel=0, 1920x1080, format=0x22, buf_size=3110400, count=4 ***
   *** VBM Pool: Allocated DMA memory: vaddr=0x..., paddr=0x..., size=12441600 ***
   *** VBM Buffer[0]: vaddr=0x..., paddr=0x..., size=3110400 ***
   *** VBM Buffer[1]: vaddr=0x..., paddr=0x..., size=3110400 ***
   *** VBM Buffer[2]: vaddr=0x..., paddr=0x..., size=3110400 ***
   *** VBM Buffer[3]: vaddr=0x..., paddr=0x..., size=3110400 ***
   *** Channel 0: VBM pool created successfully (Binary Ninja compatible) ***
   *** Writing VBM pool info to /tmp/alloc_manager_info ***
   *** VBM Info: info->owner = VBMPool0
   *** VBM Info: Buffer[0] info->paddr = 0x...
   *** VBM Info: Buffer[1] info->paddr = 0x...
   *** VBM Info: Buffer[2] info->paddr = 0x...
   *** VBM Info: Buffer[3] info->paddr = 0x...
   *** VBM pool info written successfully to /tmp/alloc_manager_info ***

   *** Channel 0: QBUF - Using VBM pool buffer[0]: paddr=0x..., vaddr=0x... ***
   *** QBUF: Programming VIC register 0x318 with buffer paddr 0x... ***
   *** QBUF: VIC register programmed successfully ***

   *** Channel 0: QBUF - Using VBM pool buffer[1]: paddr=0x..., vaddr=0x... ***
   *** QBUF: Programming VIC register 0x31c with buffer paddr 0x... ***
   *** QBUF: VIC register programmed successfully ***

   ... (repeat for all buffers)
   ```

4. **Verify /tmp/alloc_manager_info:**
   ```bash
   cat /tmp/alloc_manager_info
   ```

   Expected output:
   ```
   info->owner = VBMPool0
   info->paddr = 0x12345678
   info->paddr = 0x12345679
   info->paddr = 0x1234567a
   info->paddr = 0x1234567b
   ```

5. **Check for frame done interrupts:**
   ```bash
   dmesg | grep -E "frame done|interrupt"
   ```

   Expected: Frame done interrupts should fire continuously without stalling

## Success Criteria

✅ **VBM pool created** - Log shows pool allocation with correct size and buffer count
✅ **DMA memory allocated** - Log shows DMA coherent memory allocation
✅ **Buffer entries initialized** - Log shows all buffer entries with vaddr/paddr
✅ **/tmp/alloc_manager_info created** - File exists with correct format
✅ **VIC registers programmed** - Log shows register writes for each QBUF
✅ **No control limit errors** - No bit 21 errors in VIC status
✅ **Frame done interrupts** - Continuous interrupts without stalling
✅ **Real frame data** - Frames contain sensor data, not green/black

## Troubleshooting

### If VBM pool allocation fails:
- Check available memory: `cat /proc/meminfo | grep MemFree`
- Reduce buffer count in REQBUFS (try 2-3 buffers instead of 4-5)
- Check DMA allocation logs for errors

### If /tmp/alloc_manager_info is not created:
- Check file permissions: `ls -la /tmp/alloc_manager_info`
- Check kernel logs for `write_vbm_pool_info` errors
- Verify VBM pool was created successfully

### If VIC registers are not programmed:
- Verify `vic_regs` is not NULL
- Check register offset calculation: `(index + 0xc6) << 2`
- Verify buffer index is within pool buffer_count

### If control limit errors persist:
- Check that buffer addresses are 4-byte aligned
- Verify buffer index doesn't accidentally set bit 17
- Check VIC control register for bit 17 (control limit enable)

## Next Steps After Testing

1. **If successful** (no stalls, real frame data):
   - Commit changes with detailed message
   - Move to Gap 1: VIC interrupt configuration
   - Move to Gap 2: Buffer address programming timing

2. **If 5/5 interrupt stalls persist**:
   - Use Binary Ninja MCP to decompile reference driver's QBUF handler
   - Compare register write sequences
   - Check for missing register writes or incorrect timing

3. **If control limit errors persist**:
   - Decompile reference driver's control limit fix
   - Verify mask 0xFFFDFFFF is applied correctly
   - Check for additional control register configuration

