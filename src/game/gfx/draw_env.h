/* A 256x256 draw environment the game can switch to and back. */
#ifndef KF_DRAW_ENV_H
#define KF_DRAW_ENV_H

#include "common.h"
#include "gfx/packet.h"

/* Puts a 256x256 16-bit draw environment at frame page 0x1C2, waits, and puts the normal one back */
void DrawEnv_PutOffscreenAndBack(void);
/* Puts the draw environment of DrawEnv_PutOffscreenAndBack and waits, without putting the normal one back */
void DrawEnv_PutOffscreen(void);

/* data still in asm */
extern GsDrawEnv game_dbuff_draw0;  /* game_dbuff.draw0 */
extern GsDrawEnv game_dbuff_draw1;  /* game_dbuff.draw1 */
extern GsDataBlock offscreen_square_corners;  /* two corners of a 256x256 square around (2048, 2048), 12.4 fixed point */

#endif
