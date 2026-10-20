/* Direct GS drawing: clears, copies, sprites and image transfers (gfx/gs_draw.c). */
#ifndef KF_GS_DRAW_H
#define KF_GS_DRAW_H

#include "common.h"

/* gfx/gs_draw.c */
void GsDraw_SetDrawTarget(s64 frame, s64 scissor, s64 xy_offset);
void GsDraw_QueueFlush(void);
void GsDraw_QueueTexturedSprite(void *sprite_arg);
void GsDraw_QueueTexFlush(void);
void GsDraw_EnableDepthTest(void);
void GsDraw_ResetDrawTarget(void);
void GsDraw_CopyFrame(int dest_base);
void GsDraw_FillScreen(int red, int green, int blue, int alpha, s64 blend);
void GsDraw_ClearFrame(void);
void GsDraw_ClearDepth(void);
void GsDraw_SetScreenAlpha(int alpha);
void GsDraw_QueueVu1Matrix(void *matrix);
void GsDraw_DrawScrollingTexture(void *texture_arg, void *offset_arg);
void GsDraw_CopyFrameToTexture(int fbp, int psm, int width, int height);

#endif
