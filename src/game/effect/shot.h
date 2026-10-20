#ifndef KF_SHOT_H
#define KF_SHOT_H

#include "common.h"

void Shot_ClearAll(void);
void *Shot_Alloc(void);
void *Shot_AllocSecondRing(void);

/* data still in asm */
extern s32 dark_one_hits_left;  /* g_dmgLassBossCnt: weapon hits the last boss still takes */

#endif
