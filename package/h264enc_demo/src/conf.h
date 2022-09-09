
#ifndef _conf_h_
#define _conf_h_

#include <stdbool.h>
#include <linux/videodev2.h>
#include <vencoder.h>

#define ENABLE_DLOG
#define ENABLE_SAVE 0

#define G_BUF_COUNT 5
#define G_WIDTH 1920
#define G_HEIGHT 1080
#define G_V4L2_PIX_FMT V4L2_PIX_FMT_NV12
#define G_FPS 30
#define G_SUBDEV_ENTITY_NAME "ov5640 1-003c"

#define G_FRAMES 450	// 15 seconds at 30fps

#define G_CEDARC_PIX_FMT VENC_PIXEL_YUV420SP

#endif
