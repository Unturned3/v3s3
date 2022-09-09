
#ifndef _h264_h_
#define _h264_h_

#include <stdio.h>

#include <veInterface.h>
#include <vencoder.h>

#include "conf.h"
#include "util.h"

int h264_init();

int h264_encode(unsigned char *AddrVirY, unsigned char *AddrVirC);

void h264_deinit();

#endif
