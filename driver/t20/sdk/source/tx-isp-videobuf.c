#include <linux/module.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/dma-buf.h>
#include <linux/miscdevice.h>
#include <linux/scatterlist.h>
#include <linux/uaccess.h>

#include <media/videobuf2-memops.h>
#include "tx-isp-debug.h"
#include "tx-isp-videobuf.h"

#define TX_ISP_T20_GET_DMA_PHY \
	_IOWR('q', 18, struct tx_isp_t20_dma_info)

struct tx_isp_t20_dma_info {
	u32 fd;
	u32 size;
	u32 physical_address;
};

struct frame_channel_dmabuf_attachment {
	struct sg_table table;
	enum dma_data_direction direction;
};

static int frame_channel_dmabuf_attach(struct dma_buf *dma_buffer,
				       struct device *device,
				       struct dma_buf_attachment *attachment)
{
	struct vb2_dc_buf *buf = dma_buffer->priv;
	struct frame_channel_dmabuf_attachment *private;
	int ret;

	private = kzalloc(sizeof(*private), GFP_KERNEL);
	if (!private)
		return -ENOMEM;
	ret = dma_get_sgtable(buf->conf->dev, &private->table, buf->vaddr,
			      buf->paddr, buf->size);
	if (ret) {
		kfree(private);
		return ret;
	}
	private->direction = DMA_NONE;
	attachment->priv = private;
	return 0;
}

static void frame_channel_dmabuf_detach(struct dma_buf *dma_buffer,
					struct dma_buf_attachment *attachment)
{
	struct frame_channel_dmabuf_attachment *private = attachment->priv;

	if (!private)
		return;
	if (private->direction != DMA_NONE)
		dma_unmap_sg(attachment->dev, private->table.sgl,
			     private->table.orig_nents, private->direction);
	sg_free_table(&private->table);
	kfree(private);
	attachment->priv = NULL;
}

static struct sg_table *
frame_channel_dmabuf_map(struct dma_buf_attachment *attachment,
			 enum dma_data_direction direction)
{
	struct frame_channel_dmabuf_attachment *private = attachment->priv;

	if (private->direction == direction)
		return &private->table;
	if (private->direction != DMA_NONE)
		dma_unmap_sg(attachment->dev, private->table.sgl,
			     private->table.orig_nents, private->direction);
	private->table.nents = dma_map_sg(attachment->dev, private->table.sgl,
					 private->table.orig_nents, direction);
	if (!private->table.nents) {
		private->direction = DMA_NONE;
		return ERR_PTR(-EIO);
	}
	private->direction = direction;
	return &private->table;
}

static void frame_channel_dmabuf_unmap(struct dma_buf_attachment *attachment,
				       struct sg_table *table,
				       enum dma_data_direction direction)
{
}

static void *frame_channel_dmabuf_kmap(struct dma_buf *dma_buffer,
				       unsigned long page)
{
	struct vb2_dc_buf *buf = dma_buffer->priv;

	return buf->vaddr + page * PAGE_SIZE;
}

static void *frame_channel_dmabuf_vmap(struct dma_buf *dma_buffer)
{
	struct vb2_dc_buf *buf = dma_buffer->priv;

	return buf->vaddr;
}

static int frame_channel_dmabuf_mmap(struct dma_buf *dma_buffer,
				     struct vm_area_struct *area);

static void frame_channel_dmabuf_release(struct dma_buf *dma_buffer);

static const struct dma_buf_ops frame_channel_dmabuf_ops = {
	.attach = frame_channel_dmabuf_attach,
	.detach = frame_channel_dmabuf_detach,
	.map_dma_buf = frame_channel_dmabuf_map,
	.unmap_dma_buf = frame_channel_dmabuf_unmap,
	.kmap_atomic = frame_channel_dmabuf_kmap,
	.kmap = frame_channel_dmabuf_kmap,
	.vmap = frame_channel_dmabuf_vmap,
	.mmap = frame_channel_dmabuf_mmap,
	.release = frame_channel_dmabuf_release,
};

static void frame_channel_vb2_put(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;
	struct vb2_dc_conf *conf = buf->conf;

	if (atomic_dec_and_test(&buf->refcount)) {
		mutex_lock(&conf->mlock);
		conf->mmap.used -= PAGE_ALIGN(buf->size);
		mutex_unlock(&conf->mlock);
		kfree(buf);
	}
}

static void *frame_channel_vb2_alloc(void *alloc_ctx, unsigned long size, gfp_t gfp_flags)
{
	struct vb2_dc_conf *conf = alloc_ctx;
	struct vb2_mmap_conf *mmap = &conf->mmap;
	struct vb2_dc_buf *buf;

	mutex_lock(&conf->mlock);
	if(conf->mmap_enable == 0){
		mutex_unlock(&conf->mlock);
		return NULL;
	}

//	printk("~~~~~~~~ %s[%d] size = %d ~~~~~~~~~~\n",__func__,__LINE__,size);
	/* Reject the allocation before advancing the shared coherent-pool cursor.
	 * The vendor check only compared the old cursor with the pool size, which
	 * allowed a final oversized buffer to run beyond the reserved mapping. */
	if (PAGE_ALIGN(size) > mmap->size - mmap->used) {
		mutex_unlock(&conf->mlock);
		return ERR_PTR(-ENOMEM);
	}
	mutex_unlock(&conf->mlock);

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (!buf)
		return ERR_PTR(-ENOMEM);
	mutex_lock(&conf->mlock);
	buf->vaddr = mmap->vaddr + mmap->used;
	buf->paddr = mmap->paddr + mmap->used;
	mmap->used += PAGE_ALIGN(size);
	mutex_unlock(&conf->mlock);

	buf->conf = conf;
	buf->size = size;
	buf->handler.refcount = &buf->refcount;
	buf->handler.put = frame_channel_vb2_put;
	buf->handler.arg = buf;

	atomic_inc(&buf->refcount);

	return buf;
}

static void *frame_channel_vb2_cookie(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	return (void *)buf->paddr;
}

static void *frame_channel_vb2_vaddr(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;
	if (!buf)
		return 0;

	return buf->vaddr;
}

static unsigned int frame_channel_vb2_num_users(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;

	return atomic_read(&buf->refcount);
}

static int frame_channel_vb2_mmap(void *buf_priv, struct vm_area_struct *vma)
{
	struct vb2_dc_buf *buf = buf_priv;
	int ret = 0;
	unsigned long size = min_t(unsigned long, vma->vm_end - vma->vm_start, buf->size);
	if (!buf) {
		ISP_PRINT(ISP_ERROR_LEVEL,KERN_ERR "No buffer to map\n");
		return -EINVAL;
	}
#if 0
	struct vb2_dc_conf *conf = buf->conf;
	/*
	 * dma_mmap_* uses vm_pgoff as in-buffer offset, but we want to
	 * map whole buffer
	 */
	vma->vm_pgoff = 0;

	ret = dma_mmap_coherent(conf->dev, vma, buf->vaddr,
		buf->paddr, buf->size);

	if (ret) {
		pr_err("Remapping memory failed, error: %d\n", ret);
		return ret;
	}
#else

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	ret = remap_pfn_range(vma, vma->vm_start, ((unsigned long)buf->paddr) >> PAGE_SHIFT,
					size, vma->vm_page_prot);
	if(ret){
		printk(KERN_ERR "Remapping memory failed, error: %d\n", ret);
		return ret;
	}
#endif
	vma->vm_flags		|= VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_private_data	= &buf->handler;
	vma->vm_ops		= &vb2_common_vm_ops;

	vma->vm_ops->open(vma);

	pr_debug("%s: mapped dma addr 0x%08lx at 0x%08lx, size %ld\n",
		__func__, (unsigned long)buf->paddr, vma->vm_start,
		buf->size);


//	ret = vb2_mmap_pfn_range(vma, buf->paddr, buf->size,
//				  &vb2_common_vm_ops, &buf->handler);
	printk("virtureaddr = 0x%08lx physicaladdr = 0x%08x\n", vma->vm_start,buf->paddr);
	return ret;
}

static int frame_channel_dmabuf_mmap(struct dma_buf *dma_buffer,
				     struct vm_area_struct *area)
{
	return frame_channel_vb2_mmap(dma_buffer->priv, area);
}

static void frame_channel_dmabuf_release(struct dma_buf *dma_buffer)
{
	frame_channel_vb2_put(dma_buffer->priv);
}

static struct dma_buf *frame_channel_vb2_get_dmabuf(void *buf_priv)
{
	struct vb2_dc_buf *buf = buf_priv;
	struct dma_buf *dma_buffer;

	dma_buffer = dma_buf_export(buf, &frame_channel_dmabuf_ops,
				    buf->size, O_RDWR);
	if (IS_ERR(dma_buffer))
		return NULL;

	/* The exported fd may outlive the V4L2 queue allocation. */
	atomic_inc(&buf->refcount);
	return dma_buffer;
}

static void *frame_channel_vb2_get_userptr(void *alloc_ctx, unsigned long vaddr,
					unsigned long size, int write)
{
	struct vb2_dc_buf *buf;

	buf = kzalloc(sizeof(*buf), GFP_KERNEL);
	if (!buf)
		return ERR_PTR(-ENOMEM);
#if 0
	struct vb2_dc_conf *conf = alloc_ctx;
	struct vm_area_struct *vma;
	dma_addr_t paddr = 0;
	int ret = 0;
	if(conf->tlb_enable){
		/* tlb mmap ops */
		buf->vma = NULL;
	}else{
		ret = vb2_get_contig_userptr(vaddr, size, &vma, &paddr);
		if (ret) {
			ISP_PRINT(ISP_ERROR_LEVEL,KERN_ERR "Failed acquiring VMA for vaddr 0x%08lx\n",
					vaddr);
			kfree(buf);
			return ERR_PTR(ret);
		}
		buf->vma = vma;
	}
	buf->size = size;
	buf->paddr = paddr;
	buf->vaddr = (void *)vaddr;
#endif
	buf->size = size;
	buf->paddr = (dma_addr_t)vaddr;
	buf->vaddr = (void *)vaddr;

	return buf;
}

static void frame_channel_vb2_put_userptr(void *mem_priv)
{
	struct vb2_dc_buf *buf = mem_priv;

	if (!buf)
		return;
#if 0
	struct vb2_dc_conf *conf = buf->conf;
	if(conf->tlb_enable != 0)
		vb2_put_vma(buf->vma);
#endif
	kfree(buf);
}

const struct vb2_mem_ops frame_channel_vb2_memops = {
	.alloc		= frame_channel_vb2_alloc,
	.put		= frame_channel_vb2_put,
	.get_dmabuf	= frame_channel_vb2_get_dmabuf,
	.cookie		= frame_channel_vb2_cookie,
	.vaddr		= frame_channel_vb2_vaddr,
	.mmap		= frame_channel_vb2_mmap,
	.get_userptr	= frame_channel_vb2_get_userptr,
	.put_userptr	= frame_channel_vb2_put_userptr,
	.num_users	= frame_channel_vb2_num_users,
};

static long frame_channel_resolve_dma(struct file *file,
				      unsigned int command,
				      unsigned long argument)
{
	struct tx_isp_t20_dma_info info;
	struct vb2_dc_buf *buf;
	struct dma_buf *dma_buffer;
	long ret = 0;

	if (command != TX_ISP_T20_GET_DMA_PHY || !argument)
		return -ENOIOCTLCMD;
	if (copy_from_user(&info, (void __user *)argument, sizeof(info)))
		return -EFAULT;
	dma_buffer = dma_buf_get((int)info.fd);
	if (IS_ERR(dma_buffer))
		return PTR_ERR(dma_buffer);
	if (dma_buffer->ops != &frame_channel_dmabuf_ops) {
		ret = -EINVAL;
		goto out;
	}
	buf = dma_buffer->priv;
	if (!buf || !info.size || info.size > buf->size) {
		ret = -EINVAL;
		goto out;
	}
	info.physical_address = (u32)buf->paddr;
	if (copy_to_user((void __user *)argument, &info, sizeof(info)))
		ret = -EFAULT;
out:
	dma_buf_put(dma_buffer);
	return ret;
}

static const struct file_operations frame_channel_resolver_ops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = frame_channel_resolve_dma,
};

static struct miscdevice frame_channel_resolver = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "tx-isp-t20-dmabuf",
	.fops = &frame_channel_resolver_ops,
};

int frame_channel_dmabuf_resolver_register(void)
{
	return misc_register(&frame_channel_resolver);
}

void frame_channel_dmabuf_resolver_unregister(void)
{
	misc_deregister(&frame_channel_resolver);
}

#ifdef	CONFIG_ISP_MODULE_USE_MMAP
static int frame_buffer_mmap_init(struct vb2_mmap_conf *mmap, struct device *dev)
{
	mmap->dev = dev;
	mmap->used = 0;
	mmap->size = TX_ISP_FRAME_CHANNEL_BUFFER_MAX;
	mmap->vaddr = dma_alloc_coherent(mmap->dev, mmap->size, &mmap->paddr,
					GFP_KERNEL);
	if (!mmap->vaddr) {
		dev_err(mmap->dev, "dma_alloc_coherent of size %ld failed\n",
			mmap->size);
		return -ENOMEM;
	}
	memset(mmap->vaddr, 0x2f, mmap->size);
	printk("~~~~~~~~ %s[%d] vaddr = 0x%08x paddr = 0x%08x ~~~~~~~~~~\n",__func__,__LINE__,
				(unsigned int)mmap->vaddr, (unsigned int)mmap->paddr);
	return ISP_SUCCESS;
}
#endif

static int frame_buffer_mmap_deinit(struct vb2_mmap_conf *mmap)
{
	if(mmap && mmap->vaddr && mmap->used == 0){
		dma_free_coherent(mmap->dev, mmap->size, mmap->vaddr,mmap->paddr);
		return ISP_SUCCESS;
	}
	return -EPERM;
}
void *frame_buffer_manager_create(struct device *dev)
{
	struct vb2_dc_conf *conf;
	int ret = ISP_SUCCESS;

	conf = (struct vb2_dc_conf *)kzalloc(sizeof(*conf), GFP_KERNEL);
	if (!conf)
		return ERR_PTR(-ENOMEM);
	conf->tlb_enable = 0;
	conf->mmap_enable = 0;

#ifdef	CONFIG_ISP_MODULE_WITH_TLB
	conf->tlb_enable = 1;
#endif
#ifdef	CONFIG_ISP_MODULE_USE_MMAP
	conf->mmap_enable = 1;
	ret = frame_buffer_mmap_init(&(conf->mmap), dev);
#endif
	if(ret != ISP_SUCCESS)
		goto exit;
	conf->dev = dev;
	mutex_init(&conf->mlock);
	return conf;
exit:
	kfree(conf);
	return NULL;
}

int frame_buffer_manager_cleanup(void *alloc_ctx)
{
	int ret = ISP_SUCCESS;
	struct vb2_dc_conf *conf =  (struct vb2_dc_conf *)alloc_ctx;
	if(conf->mmap_enable)
		ret = frame_buffer_mmap_deinit(&(conf->mmap));
	if(ret == ISP_SUCCESS)
		kfree(alloc_ctx);
	return ret;
}
