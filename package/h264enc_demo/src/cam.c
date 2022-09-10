
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include <linux/media.h>

#include "cam.h"
#include "util.h"
#include "conf.h"

static int fd = -1;
static int g_width = 640, g_height = 480;
static int g_buf_count = G_BUF_COUNT;
static int buf_idx = 0;
static buffer_t *buffers = NULL;

#define NUM_CTRLS 2
struct v4l2_control ctrls[NUM_CTRLS] = {
	{V4L2_CID_HFLIP, 1},
	{V4L2_CID_VFLIP, 1}
};

static int check_cnt = 0;

int sanity_check(int buf_idx, int offset) {
	/*
	int addr = offset;
	if (check_cnt % 30 == 29) {
		dlog("Info: sanity check: offset = %d\n", offset);
	}
	check_ret(ioctl(fd, CAM_V2P_IOCTL, &addr), "CAM_V2P_IOCTL");
	assert(buffers[buf_idx].addrPhyY == addr);
	check_cnt += 1;
	*/
	return 0;
}

int cam_open() {
	fd = open("/dev/video0", O_RDWR, 0);
	check_ret(fd, "open /dev/video0");
	return 0;
}

static int cam_media_init() {
	int ret = 0;
	struct media_v2_entity *mve = NULL;
	struct media_v2_pad *mvp = NULL;
	int mfd = open("/dev/media0", O_RDWR, 0);
	check_cleanup(mfd, "open /dev/media0");

	struct media_device_info mdi;
	CLEAR(mdi);
	check_cleanup(ioctl(mfd, MEDIA_IOC_DEVICE_INFO, &mdi), "MEDIA_IOC_DEVICE_INFO");

	dlog("Info: media device driver: %s\n", mdi.driver);

	struct media_v2_topology mvt;
	CLEAR(mvt);
	check_cleanup(ioctl(mfd, MEDIA_IOC_G_TOPOLOGY, &mvt), "MEDIA_IOC_G_TOPOLOGY");

	dlog("Info: %d media entities detected\n", mvt.num_entities);

	mve = calloc(mvt.num_entities, sizeof(*mve));
	mvp = calloc(mvt.num_pads, sizeof(*mvp));
	if (!mve || !mvp) {
		dlog("Error: mve/mvp calloc() failed\n");
		goto cleanup;
	}
	mvt.ptr_entities = (unsigned long) mve;
	mvt.ptr_pads = (unsigned long) mvp;
	check_cleanup(ioctl(mfd, MEDIA_IOC_G_TOPOLOGY, &mvt), "MEDIA_IOC_G_TOPOLOGY");

	int entity_id = -1, subdev_pad = -1;

	for (int i=0; i<mvt.num_entities; i++) {
		if (strcmp(G_SUBDEV_ENTITY_NAME, mve[i].name) == 0) {
			entity_id = mve[i].id;
			dlog("Info: %s: subdev entity id = %d\n", G_SUBDEV_ENTITY_NAME,
				 entity_id);
		}
	}

	if (entity_id == -1) {
		dlog("Error: media entity %s not found\n", G_SUBDEV_ENTITY_NAME);
		goto cleanup;
	}

	for (int i=0; i<mvt.num_pads; i++) {
		if (mvp[i].entity_id == entity_id) {
			subdev_pad = mvp[i].index;
			dlog("Info: %s: subdev pad = %d\n", G_SUBDEV_ENTITY_NAME, subdev_pad);
		}
	}

	if (subdev_pad == -1) {
		dlog("Error: no subdev pad found for %s\n", G_SUBDEV_ENTITY_NAME);
		goto cleanup;
	}

	struct media_entity_desc dsc;
	CLEAR(dsc);
	dsc.id = entity_id;
	check_cleanup(ioctl(mfd, MEDIA_IOC_ENUM_ENTITIES, &dsc), "MEDIA_IOC_ENUM_ENTITIES");

	dlog("Info: %s: subdev major = %d, minor = %d\n",
		dsc.name, dsc.dev.major, dsc.dev.minor);

	struct v4l2_fract fract;
	fract.numerator = 1;
	fract.denominator = G_FPS;

	struct v4l2_subdev_frame_interval ival;
	CLEAR(ival);
	ival.interval = fract;
	ival.pad = subdev_pad;

	int sfd = open("/dev/v4l-subdev0", O_RDWR);
	check_cleanup(sfd, "open /dev/v4l2-subdev0");
	check_cleanup(ioctl(sfd, VIDIOC_SUBDEV_S_FRAME_INTERVAL, &ival),
					"VIDIOC_SUBDEV_S_FRAME_INTERVAL");
	
	dlog("Info: %s: frame rate set to %d\n", G_SUBDEV_ENTITY_NAME, G_FPS);

cleanup:
	if (sfd >= 0) {
		close(sfd);
		sfd = -1;
	}
	if (mve)
		free(mve);
	if (mvp)
		free(mvp);
	if (mfd >= 0) {
		close(mfd);
		mfd = -1;
	}
	return ret;
}

static int cam_init_userptr(unsigned int buffer_size) {
	struct v4l2_requestbuffers req;
	CLEAR(req);

	req.count = g_buf_count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;

	check_ret(ioctl(fd, VIDIOC_REQBUFS, &req), "VIDIOC_REQBUFS");
	g_buf_count = req.count;

	buffers = calloc(g_buf_count, sizeof(*buffers));

	if (buffers == NULL) {
		dlog("Error: buffer calloc() failed\n");
		return -1;
	}

	/*
	for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = malloc(buffer_size);

		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
	*/
}

int cam_init(unsigned int width, unsigned int height, unsigned int pixfmt) {

	if (cam_media_init() < 0) {
		dlog("Error: cam_media_init() failed\n");
		return -1;
	}

	struct v4l2_capability cap;
	CLEAR(cap);
	check_ret(ioctl(fd, VIDIOC_QUERYCAP, &cap), "VIDIOC_QUERYCAP");

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ||
		!(cap.capabilities & V4L2_CAP_STREAMING)) {
		dlog("Error: V4L2_CAP_VIDEO_CAPTURE or V4L2_CAP_STREAMING not supported\n");
		return -1;
	}
	
	struct v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.pixelformat = pixfmt;
	g_width = width;
	g_height = height;
	check_ret(ioctl(fd, VIDIOC_S_FMT, &fmt), "VIDIOC_S_FMT");
	
	for (int i=0; i<NUM_CTRLS; i++)
		check_ret(ioctl(fd, VIDIOC_S_CTRL, &ctrls[i]), "VIDIOC_S_CTRL");

	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count = g_buf_count;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	check_ret(ioctl(fd, VIDIOC_REQBUFS, &req), "VIDIOC_REQBUFS");

	g_buf_count = req.count;	// update the actual number of buffers expected by device

	buffers = calloc(g_buf_count, sizeof(*buffers));

	if (buffers == NULL) {
		dlog("Error: buffer calloc() failed\n");
		return -1;
	}

	for (int i=0; i<g_buf_count; i++) {
		struct v4l2_buffer buf;
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		check_ret(ioctl(fd, VIDIOC_QUERYBUF, &buf), "VIDIOC_QUERYBUF");

		buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
								MAP_SHARED, fd, buf.m.offset);
		buffers[i].length = buf.length;
		buffers[i].addrVirY = buffers[i].start;
		// This ALIGN_16B thing is wrong?
		// At 1920x1080, this will screw up the data alignment and create a green band in video

		//buffers[i].addrVirC = buffers[i].start + ALIGN_16B(g_width) * ALIGN_16B(g_height);
		buffers[i].addrVirC = buffers[i].start + g_width * g_height;
		
		int addr = buf.m.offset;
		check_ret(ioctl(fd, CAM_V2P_IOCTL, &addr), "CAM_V2P_IOCTL");
		buffers[i].addrPhyY = (void *) addr;
		//buffers[i].addrPhyC = addr + ALIGN_16B(g_width) * ALIGN_16B(g_height);
		buffers[i].addrPhyC = (void *) (addr + g_width * g_height);
	}

	return 0;
}

int cam_start_capture() {
	for (int i=0; i<g_buf_count; i++) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		check_ret(ioctl(fd, VIDIOC_QBUF, &buf), "VIDIOC_QBUF");
		sanity_check(i, buf.m.offset);
	}
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	check_ret(ioctl(fd, VIDIOC_STREAMON, &type), "VIDIOC_STREAMON");
	return 0;
}

// Returns non-negative dequeued buffer index upon success
int cam_dqbuf() {
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = buf_idx;
	check_ret(ioctl(fd, VIDIOC_DQBUF, &buf), "VIDIOC_DQBUF");
	sanity_check(buf_idx, buf.m.offset);
	return buf_idx;
}

buffer_t *cam_get_buf(int idx) {
	if (0 <= idx && idx < g_buf_count)
		return buffers + idx;
	return NULL;
}

// Enqueue the last dequeued buffer back to the camera device
int cam_qbuf() {
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = buf_idx;
	check_ret(ioctl(fd, VIDIOC_QBUF, &buf), "VIDIOC_QBUF");
	sanity_check(buf_idx, buf.m.offset);
	buf_idx = (buf_idx + 1) % g_buf_count;
	return 0;
}

int cam_stop_capture() {
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	check_ret(ioctl(fd, VIDIOC_STREAMOFF, &type), "VIDIOC_STREAMOFF");
	return 0;
}

void cam_deinit() {
	for (int i=0; i<g_buf_count; i++)
		munmap(buffers[i].start, buffers[i].length);
	if (buffers) {
		free(buffers);
		buffers = NULL;
	}
}

void cam_close() {
	if (fd != -1) {
		close(fd);
		fd = -1;
	}
}
