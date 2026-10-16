/* A 256x256 draw environment the game can switch to and back. */
#ifndef KF_DRAW_ENV_H
#define KF_DRAW_ENV_H

#include "common.h"

/* Puts a 256x256 16-bit draw environment at frame page 0x1C2, waits, and puts the normal one back */
void DrawEnv_PutOffscreenAndBack(void);
/* Puts the draw environment of DrawEnv_PutOffscreenAndBack and waits, without putting the normal one back */
void DrawEnv_PutOffscreen(void);

#endif
