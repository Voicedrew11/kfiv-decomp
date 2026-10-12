#include "kf_entity.h"

extern u32 D_00446ED4;
extern float D_00413F80[];

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_001499F8(Entity *);

void func_00149610(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.6981317f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    side = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8((char *)entity, (void (*)(void))func_001499F8, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    angle = func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f) {
        if (D_00446ED4 % 10U == entity->unkD7 % 10U &&
            rand() % 100 < (entity->params->unk0A - entity->unk2C) * 100 / entity->params->unk0A) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
                return;
            }
        }
    }
    if (unkE0->unk78 == 0) {
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            entity->unkC8 = func_00190BE0(entity);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
