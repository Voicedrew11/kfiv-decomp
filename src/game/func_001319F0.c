#include "kf_entity.h"

void func_00131B38(Entity *, int);
void func_00131BB8(Entity *, s32);
void func_00131C78(char *, int);
void func_00131D70(Entity *, s32);
void func_00132090(Entity *, s32);

void func_001319F0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    f32 distance;

    if (unkE0->unk7D <= 0) {
        distance = entity->distance_to_player - entity->radius - 100.0f;
        if (distance > 1200.0f) {
            unkE0->unk7C = 0;
            unkE0->unk7D = 10;
        } else if (distance <= 1200.0f && distance > 800.0f) {
            unkE0->unk7C = 1;
            unkE0->unk7D = 10;
        } else if (distance <= 800.0f && distance > 500.0f) {
            unkE0->unk7C = 2;
            unkE0->unk7D = 10;
        } else if (distance <= 500.0f && distance > 100.0f) {
            unkE0->unk7C = 3;
            unkE0->unk7D = 10;
        } else {
            unkE0->unk7C = 4;
            unkE0->unk7D = 10;
        }
    }
    switch (unkE0->unk7C) {
    case 0:
        func_00131B38(entity, unknown);
        break;
    case 1:
        func_00131BB8(entity, unknown);
        break;
    case 2:
        func_00131C78((char *)entity, unknown);
        break;
    case 3:
        func_00131D70(entity, unknown);
        break;
    case 4:
        func_00132090(entity, unknown);
        break;
    default:
        func_00131B38(entity, unknown);
        break;
    }
}
