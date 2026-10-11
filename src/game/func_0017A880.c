#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(void);
void func_00181C60(void);
void func_00181E48(void);
void func_00182240(void);

void func_0017A880(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 range;
    f32 angle;
    f32 turn;
    s32 next_action;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    range = 300.0f;
    angle = 0.5235988f;
    if (func_0018ED98(entity, range, angle)) {
        func_001958B8((char *)entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, range, angle)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    next_action = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))func_00190B88)(entity);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8((char *)entity, func_00182240, unknown);
            } else {
                func_001958B8((char *)entity, func_00181E48, unknown);
            }
            return;
        case 1:
            if ((s16)entity->params->unk434 < entity->distance_to_player) { // match: read signed here
                turn = func_001957B8(entity->unk68, entity->model->unkD0[1]);
                turn = ABS(turn);
                if (turn > 0.7853982f) {
                    entity->target = D_00413F80;
                    entity->unkA0 = rand() % 30 + 60;
                    unkE0->unk78 = 0;
                } else {
                    unkE0->unk78 = -1;
                }
            } else {
                entity->target = D_00413F80;
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 1;
            }
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (entity->unkCD == 1) {
        side = func_0018EAC0(entity, direction);
        switch (unkE0->unk78) {
        case 0:
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190BE0(entity);
            }
            break;
        case 1:
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
            }
            break;
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
