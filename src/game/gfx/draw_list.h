/* The deferred draw list (gfx/draw_list.c). */
#ifndef KF_DRAW_LIST_H
#define KF_DRAW_LIST_H

#include "common.h"
#include "model/model.h"

/* gfx/draw_list.c */
void DrawList_Clear(void);
void DrawList_Add(ModelInstance *model, s32 layer, s32 list);
void DrawList_Draw(u32 layer, u32 list);

#endif
