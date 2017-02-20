#ifndef _MOUSECAMCONFIG_H_
#define _MOUSECAMCONFIG_H_

#include <math.h>

#define OUT_REPORT_SIZE			55
#define FRAME_X					18
#define FRAME_Y					18
#define PIXEL_SIZE				20
#define FRAME_SIZE				FRAME_X * FRAME_Y
#define WINDOW_WIDTH			2*(FRAME_X * PIXEL_SIZE)
#define WINDOW_HEIGTH			FRAME_Y * PIXEL_SIZE + 18
#define NUMBER_OF_FRAMEPARTS	ceil((float)FRAME_SIZE / (float)OUT_REPORT_SIZE) + 1
#define START_SCAN_POSITION		(WINDOW_WIDTH) / 2

#endif
