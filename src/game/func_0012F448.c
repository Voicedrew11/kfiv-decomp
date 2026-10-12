#include "kf_entity.h"

int rand(void);
int func_0018EAC0(void *, void *);
float func_00194FB8(float);
s32 func_00195AC0(Entity *, void *, s32, s32, s32);
float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);

void func_0012F448(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 turn;

    if (unkE0->unk7A == 0) {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->unkC8 = 0x41;
        turn = func_0018EAC0(entity, direction);
        if (turn != 0 && unkE0->unk7B <= 0) {
            if (turn > 0) {
                direction[0] += 1.5707964f;
            } else if (turn < 0) {
                direction[0] -= 1.5707964f;
            }
            direction[0] += (f32)(rand() % 140 - 70) * 0.017453292f;
            unkE0->unk48 = func_00194FB8(direction[0]);
            unkE0->unk7A = 1;
        }
        if (unkE0->unk7B > 0) {
            unkE0->unk7B--;
        }
        if (unkE0->unk7C == 0) {
            unkE0->unk7C = func_00195AC0(entity, &entity->params->unk240[1], 0, 1, 1);
        }
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        angle = func_001957B8(entity->model->unkD0[1], unkE0->unk48);
        if (ABS(angle) > entity->params->turn_limit) {
            if (angle >= 0.0f) {
                entity->unkC8 |= 0x600;
            } else {
                entity->unkC8 |= 0x500;
            }
        } else {
            unkE0->unk7A = 0;
            unkE0->unk7C = 0;
            unkE0->unk7B = entity->params->unk434;
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
