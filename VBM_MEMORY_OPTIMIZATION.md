# VBMPool0 Memory Optimization Strategy

## Current State
- **VBMPool0**: 12.5MB from rmem (41% of total)
- **Buffer Count**: 4 buffers × 3.1MB each
- **Format**: 1920×1080 YUV420 (1.5 bytes per pixel)

## Optimization Options

### 1. Reduce Buffer Count (Immediate)
```c
// Current: 4 buffers (capture, process, encode, spare)
// Optimized: 3 buffers (remove spare)
// Savings: 3.1MB (25% reduction)
```

### 2. Dynamic Buffer Allocation
```c
// Instead of pre-allocating 4 buffers:
// - Start with 2 buffers
// - Allocate additional buffers on-demand
// - Free unused buffers after 30 seconds
```

### 3. Buffer Format Optimization
```c
// Current: YUV420 (1.5 bytes/pixel)
// Option: YUV422 packed (2 bytes/pixel) - better for some sensors
// Option: RAW10 (1.25 bytes/pixel) - if ISP can handle
```

### 4. Memory Pool Sharing
```c
// Share buffers between channels when not simultaneously active
// Channel 0 (main): Use full resolution
// Channel 1 (sub): Use cropped region of Channel 0 buffer
```

### 5. Alternative Memory Sources
```c
// Move VBMPool0 to regular kernel memory (vmalloc)
// Keep only active buffers in rmem
// Use DMA coherent memory for hardware access
```

## Implementation Priority

1. **High Priority**: Reduce buffer count from 4 to 3
2. **Medium Priority**: Dynamic allocation/deallocation
3. **Low Priority**: Format optimization (requires extensive testing)

## Expected Results
- **Buffer reduction**: Save 3.1MB rmem
- **Dynamic allocation**: Save 6.2MB when idle
- **Total potential savings**: Up to 9.3MB (74% reduction)

This would reduce VBMPool0 from 12.5MB to as low as 3.1MB, freeing up precious rmem for other video channels.
