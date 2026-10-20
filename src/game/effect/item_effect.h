/* Item use effect (kinds 0x46–0x52): models, motion and textures shown when those items are used. */
#ifndef KF_ITEM_EFFECT_H
#define KF_ITEM_EFFECT_H

#include "common.h"

/* the effect shown when item `kind` is used (files item_ms/...), item_effect_defs_by_kind[kind] (kinds 0x46 to 0x52 are used) */
typedef struct {
    int next_step_frame;            /* 0x00 the motion frame at which the effect goes to its next step */
    int fade_per_frame;             /* 0x04 taken off the effect's brightness each frame while it fades */
    int spawn_frame;                /* 0x08 the motion frame at which the spawner effect starts */
    int effect_number;              /* 0x0C an effect number for Effect_StartSpawner */
} ItemUseEffectDef;

void ItemEffect_Load(void);
int ItemEffect_Reset(void);
void ItemEffect_Start(int kind, int marked);
void ItemEffect_Update(void);
void ItemEffect_Draw(void);

/* data still in asm */
extern char item_effect_small_model_path[];
extern char item_effect_small_motion_path[];
extern char item_effect_large_model_path[];
extern char item_effect_large_motion_path[];
extern char item_effect_texture_path_format[];

#endif
