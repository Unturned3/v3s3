
#include <stdio.h>
#include <unistd.h>

#include <vencoder.h>
#include <veInterface.h>
#include <memoryAdapter.h>

#include "cam.h"
#include "util.h"
#include "conf.h"

static VideoEncoder *gVideoEnc;
static VencBaseConfig baseConfig;
static int g_width = G_WIDTH;
static int g_height = G_HEIGHT;
static int g_pix_fmt = G_CEDARC_PIX_FMT;

FILE *fpH264 = NULL;

int h264_init() {

	fpH264 = fopen("/mnt/out.h264", "wb");

	VencH264Param h264Param;
	CLEAR(h264Param);
	h264Param.bEntropyCodingCABAC = 1;
	h264Param.nBitrate = 8 * 1024 * 1024;
	h264Param.nFramerate = 30;
	h264Param.nCodingMode = VENC_FRAME_CODING;
	h264Param.nMaxKeyInterval = 30;
	h264Param.sProfileLevel.nProfile = VENC_H264ProfileHigh;
	h264Param.sProfileLevel.nLevel = VENC_H264Level42;
	h264Param.sQPRange.nMinqp = 10;
	h264Param.sQPRange.nMaxqp = 30;

	CLEAR(baseConfig);
	baseConfig.memops = MemAdapterGetOpsS();
	if (baseConfig.memops == NULL) {
		dlog("Error: MemAdapterGetOpsS() failed\n");
		return -1;
	}
	CdcMemOpen(baseConfig.memops);

	baseConfig.nInputWidth = g_width;
	baseConfig.nInputHeight = g_height;
	baseConfig.nStride = g_width;
	baseConfig.nDstWidth = g_width;
	baseConfig.nDstHeight = g_height;
	baseConfig.eInputFormat = g_pix_fmt;

	gVideoEnc = VideoEncCreate(VENC_CODEC_H264);
	if (gVideoEnc == NULL) {
		dlog("Error: VideoEncCreate() failed\n");
		return -1;
	}

	{
		VideoEncSetParameter(gVideoEnc, VENC_IndexParamH264Param, &h264Param);
		int value = 0;
		VideoEncSetParameter(gVideoEnc, VENC_IndexParamIfilter, &value);
		value = 0;
		VideoEncSetParameter(gVideoEnc, VENC_IndexParamRotation, &value);
		value = 0;
		VideoEncSetParameter(gVideoEnc, VENC_IndexParamSetPSkip, &value);
	}
	VideoEncInit(gVideoEnc, &baseConfig);

	VencHeaderData sps_pps_data;
	VideoEncGetParameter(gVideoEnc, VENC_IndexParamH264SPSPPS, &sps_pps_data);
	fwrite(sps_pps_data.pBuffer, 1, sps_pps_data.nLength, fpH264);

	// Allocate one input buffer
	// Optimize: share this buffer with v4l2 to achieve zero-copy?
	/*
	VencAllocateBufferParam bufferParam;
	CLEAR(bufferParam);
	bufferParam.nSizeY = baseConfig.nInputWidth * baseConfig.nInputHeight;
	bufferParam.nSizeC = baseConfig.nInputWidth * baseConfig.nInputHeight / 2;
	bufferParam.nBufferNum = 1;
	//bufferParam.nBufferNum = G_BUF_COUNT;
	AllocInputBuffer(gVideoEnc, &bufferParam);
	*/

	// test code for virt-to-phys
	/*
	VencInputBuffer inputBuffer;
	CLEAR(inputBuffer);
	GetOneAllocInputBuffer(gVideoEnc, &inputBuffer);
	dlog("VirY: %p, PhyY: %p\n", inputBuffer.pAddrVirY, inputBuffer.pAddrPhyY);
	dlog("VirC: %p, PhyC: %p\n", inputBuffer.pAddrVirC, inputBuffer.pAddrPhyC);
	*inputBuffer.pAddrVirY = 3;
	while (1) { sleep(1); }
	ReturnOneAllocInputBuffer(gVideoEnc, &inputBuffer);
	*/

	dlog("Info: h264 encocder init OK\n");
	return 0;
}

int h264_encode(unsigned char *addrPhyY, unsigned char *addrPhyC) {
	VencInputBuffer inputBuffer;
	VencOutputBuffer outputBuffer;
	CLEAR(inputBuffer);
	CLEAR(outputBuffer);

	// Original code performed 1 new AllocInputBuffer per frame
	// Which quickly screwed up memory allocation

	/*
	GetOneAllocInputBuffer(gVideoEnc, &inputBuffer);

	memcpy(inputBuffer.pAddrVirY, addrPhyY,
		   baseConfig.nInputWidth * baseConfig.nInputHeight);
	memcpy(inputBuffer.pAddrVirC, addrPhyC,
		   baseConfig.nInputWidth * baseConfig.nInputHeight / 2);
	*/

	//FlushCacheAllocInputBuffer(gVideoEnc, &inputBuffer);
	inputBuffer.pAddrPhyY = addrPhyY;
	inputBuffer.pAddrPhyC = addrPhyC;

	AddOneInputBuffer(gVideoEnc, &inputBuffer);

	if (VideoEncodeOneFrame(gVideoEnc) != VENC_RESULT_OK) {
		dlog("Error: VideoEncodeOneFrame() failed\n");
		return -1;
	}

	AlreadyUsedInputBuffer(gVideoEnc, &inputBuffer);
	//ReturnOneAllocInputBuffer(gVideoEnc, &inputBuffer);

	GetOneBitstreamFrame(gVideoEnc, &outputBuffer);

	if (outputBuffer.nSize0 > 0) {
		fwrite(outputBuffer.pData0, 1, outputBuffer.nSize0, fpH264);
	}
	if (outputBuffer.nSize1 > 0) {
		fwrite(outputBuffer.pData1, 1, outputBuffer.nSize1, fpH264);
	}

	FreeOneBitStreamFrame(gVideoEnc, &outputBuffer);
	return 0;
}

void h264_deinit() {
	if (baseConfig.memops != NULL) {
		CdcMemClose(baseConfig.memops);
		baseConfig.memops = NULL;
	}
	ReleaseAllocInputBuffer(gVideoEnc);
	VideoEncDestroy(gVideoEnc);
	gVideoEnc = NULL;
	fclose(fpH264);
}
