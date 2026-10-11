#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
s32 func_00197F00(Entity *);
s32 func_00198D10(Entity *);
int func_0018CA50(int *);

s32 func_00164A60(Entity *entity, s32 mode) {
    s32 chances[4];
    s32 weights[4];
    s32 choice = -1;
    s32 i;
    s32 total;
    s32 fade;
    s32 elapsed;
    s32 roll;
    s32 sum;

    for (i = 0; i < 4; i++) {
        weights[i] = entity->params->unk14[i];
    }
    if (!func_00197F00(entity) && (entity->unkA2 != 0 || !func_00198D10(entity))) {
        weights[0] = 0;
    }
    if (mode == 0) {
        if ((u32)(entity->flags >> 63) || func_0018CA50((int *)entity)) {
            weights[0] = 0;
        }
    } else if (mode == 1) {
        if (func_0018CA50((int *)entity)) {
            weights[0] = 0;
        }
    } else if (mode == 2) {
        if ((u32)(entity->flags >> 63)) {
            weights[0] = 0;
        }
    }
    total = 0;
    if (weights[0] + weights[1] + weights[2] + weights[3] == 0) {
        return -1;
    }
    for (i = 0; i < 4; i++) {
        if (i == entity->unkCD) {
            elapsed = (u16)D_00446ED4 - entity->unk9E;
            if (elapsed < 240) {
                fade = 0x80 - (elapsed * (elapsed << 7)) / 57600;
                fade += 0x80;
            } else {
                fade = 0x80;
            }
        } else {
            fade = 0x80;
        }
        chances[i] = (fade * ((entity->unkBC[i] * weights[i]) >> 6)) >> 7;
        total += chances[i];
    }
    if (total == 0) {
        return -1;
    }
    roll = rand() % total;
    sum = 0;
    for (i = 0; i < 4; i++) {
        sum += chances[i];
        if (roll < sum) {
            choice = i;
            break;
        }
    }
    return choice;
}
