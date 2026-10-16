/* Item use effect (kinds 0x46–0x52): models, motion and textures shown when those items are used. */
#ifndef KF_ITEM_EFFECT_H
#define KF_ITEM_EFFECT_H

#include "common.h"

void ItemEffect_Load(void);
int ItemEffect_Reset(void);
void ItemEffect_Start(int kind, int marked);
void ItemEffect_Update(void);
void ItemEffect_Draw(void);

#endif
