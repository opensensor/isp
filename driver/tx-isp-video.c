#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>

#include "tx-isp-main.h"
#include "tx-isp.h"
#include "tx-isp-hw.h"

//
//int tx_isp_video_s_stream(struct IMPISPDev *dev, int enable)
//{
//    struct isp_framesource_state *fs;
//    int ret = 0;
//
//    pr_info("Setting video stream state: %d\n", enable);
//
//    if (!dev) {
//        pr_err("Invalid device in stream control\n");
//        return -EINVAL;
//    }
//
//    // Get frame source channel 0 as seen in enable_isp_streaming
//    fs = &dev->frame_sources[0];
//    if (!fs) {
//        pr_err("Frame source not initialized\n");
//        return -EINVAL;
//    }
//
//    if (enable) {
//        // Perform stream enable sequence
//        ret = enable_isp_streaming(dev, NULL, 0, true);
//        if (ret)
//            return ret;
//
//        // Update state seen in decompiled enable paths
//        fs->state = 2;
//    } else {
//        // Stop streaming
//        ret = enable_isp_streaming(dev, NULL, 0, false);
//        if (ret)
//            return ret;
//
//        // Reset state
//        fs->state = 1;
//    }
//
//    pr_info("Stream state changed to: %d\n", enable);
//    return 0;
//}
