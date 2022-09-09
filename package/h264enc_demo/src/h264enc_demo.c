
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include <linux/media.h>

#include "h264.h"
#include "cam.h"
#include "util.h"
#include "conf.h"

/*
	Getting subdevice name from major & minor numbers:
	https://git.linuxtv.org/v4l-utils.git/tree/utils/media-ctl/libmediactl.c
	in function 'media_get_devname_sysfs()'

	More detail on the media-ctl API:
	https://www.kernel.org/doc/html/latest/userspace-api/media/mediactl/media-controller.html
*/

int main() {
	h264_init();
	cam_open();
	cam_init(G_WIDTH, G_HEIGHT, V4L2_PIX_FMT_NV12);
	cam_start_capture();

	struct timespec t_start;
	clock_gettime(CLOCK_REALTIME, &t_start);

	for (int i=0; i<G_FRAMES; i++) {

		int j = cam_dqbuf();
		buffer_t *buf = cam_get_buf(j);

		//h264_encode(buf->addrVirY, buf->addrVirC);
		h264_encode(buf->addrPhyY, buf->addrPhyC);

		if (ENABLE_SAVE) {
			char output_file[32] = "frame";
			char suffix_str[32];
			sprintf(suffix_str, "%02d", i);
			strcat(output_file, suffix_str);
			FILE *fp = fopen(output_file, "wb");
			fwrite(buf->start, 1, buf->length, fp);
			fclose(fp);
		}

		cam_qbuf();	// Queue the recently dequeued buffer back to the device
	}

	struct timespec t_stop;
	clock_gettime(CLOCK_REALTIME, &t_stop);

	long seconds = t_stop.tv_sec - t_start.tv_sec;
	long nanoseconds = t_stop.tv_nsec - t_start.tv_nsec;
	double elapsed = seconds + nanoseconds / 1e9;

	cam_stop_capture();
	cam_deinit();
	cam_close();
	h264_deinit();

	dlog("\nInfo: captured %d frames in %.2fs; FPS = %.1f\n",
		G_FRAMES, elapsed, G_FRAMES / elapsed);
	return 0;
}
