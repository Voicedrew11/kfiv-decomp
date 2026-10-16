#include "common.h"
#include "kf_model.h"

void *memset(void *, int, u32);
void Vector_Subtract(void *, void *, void *);
float sqrtf(float);

extern f32 player_state[];

/* the draw lists: 5 layers, 2 lists each, 1000 depth buckets of models */
static ModelInstance *draw_buckets[5][2][1000];

/* Forgets every model in the draw lists. */
void DrawList_Clear(void) {
    memset(draw_buckets, 0, sizeof(draw_buckets));
}

/* Puts a model in the draw list bucket for its distance from the player (1000 buckets over 5000 units). */
void DrawList_Add(ModelInstance *model, s32 layer, s32 list) {
    f32 delta[4];
    ModelInstance **bucket;
    ModelInstance *next;
    s32 depth;

    if (model->part_count != 0 && (u32)layer < 5 && (u32)list < 2) {
        Vector_Subtract(delta, &model->position, player_state);
        depth = (sqrtf(delta[0] * delta[0] + delta[1] * delta[1] + delta[2] * delta[2]) * 1000.0f) / 5000.0f;
        if (depth < 0) {
            depth = 0;
        } else if (depth >= 1000) {
            depth = 999;
        }
        bucket = (ModelInstance **)((char *)draw_buckets + (depth * 4 + list * 0xFA0 + layer * 0x1F40));
        next = *bucket;
        *bucket = model;
        model->draw_next[list] = next;
    }
}

/* Draws the models of one draw list, from the farthest bucket to the nearest. */
void DrawList_Draw(u32 layer, u32 list) {
    int depth;
    ModelInstance *model;
    ModelInstance **bucket;

    if (layer < 5U && list < 2U) {
        for (depth = 999; depth >= 0; depth--) {
            bucket = (ModelInstance **)((char *)draw_buckets + (depth * 4 + list * 0xFA0 + layer * 0x1F40));
            model = *bucket;
            if (model != 0) {
                do {
                    if (model->draw != 0) {
                        model->draw(model);
                    }
                    model->draw_parts(model);
                    if (model->after_draw != 0) {
                        model->after_draw(model);
                    }
                    model = model->draw_next[list];
                } while (model != 0);
            }
        }
    }
}
