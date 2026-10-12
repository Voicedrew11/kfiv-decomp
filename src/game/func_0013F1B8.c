#include "kf_entity.h"

extern PlayerData D_00413F80;
extern char D_0043E8C0[];

int printf(const char *, ...);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
float func_00194F80(void *, void *);
s32 func_00198D10(Entity *);
s32 func_00197F00(Entity *);
int func_0018CA50(int *);
s32 func_0018F9C0(Entity *);
s32 func_0018F648(Entity *);
void func_00216778(void *, void *);
void *func_001267B0(int);
void func_00196D48(Entity *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0013F1B8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 direction[4];
    f32 angle;
    f32 chase_angle;
    f32 margin = 0.0f;
    Entity *other;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    if (func_00194F80(entity->spawn_info, &D_00413F80) > entity->spawn_info->unk4C) {
        if (ENTITY_FLAG_BITS(entity)->bit32) {
            if (func_00198D10(entity) && !func_0018CA50((int *)entity)) {
                func_0018F9C0(entity);
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                return;
            }
        }
    } else if (ENTITY_FLAG_BITS(entity)->bit32) {
        if (func_00197F00(entity) && !func_0018CA50((int *)entity)) {
            func_0018F648(entity);
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            return;
        }
    }
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (D_00413F80.position[0] < -7900.0f) {
        func_00216778(post, &D_00413F80);
        post[0] = -8000.0f;
        if (post[2] > 3350.0f) {
            post[2] = 3550.0f;
        } else if (post[2] < 2750.0f) {
            post[2] = 2550.0f;
        } else {
            post[2] = 3050.0f;
            other = func_001267B0(0x28);
            if (ENTITY_FLAG_BITS(other)->bits2 == 0) {
                other = func_001267B0(0x28);
                if (ENTITY_FLAG_BITS(other)->bit1) {
                    margin = entity->radius + entity->radius;
                }
            }
        }
        if (unkE0->unk78 == 0) {
            if (func_00194F80(entity->model->position, post) < margin + 60.0f) {
                unkE0->unk78 = 1;
                printf(D_0043E8C0);
            } else {
                entity->target = post;
                turn = func_0018EAC0(entity, direction);
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(50.0f, entity, 0);
                }
                func_001958B8((char *)entity, unkE0->unk18, unknown);
                return;
            }
        } else {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (func_00194F80(entity->model->position, post) > margin + 150.0f) {
                entity->target = post;
                unkE0->unk78 = 0;
                return;
            }
            if (ENTITY_FLAG_BITS(entity)->bit32 && func_00198D10(entity) && !func_0018CA50((int *)entity)) {
                func_0018F9C0(entity);
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                return;
            }
            if (ABS(angle) > 0.5235988f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x201;
                } else {
                    entity->unkC8 = 0x101;
                }
                func_001958B8((char *)entity, unkE0->unk18, unknown);
                return;
            }
        }
    } else {
        entity->target = &D_00413F80;
    }
    func_00196D48(entity);
    if (ENTITY_FLAG_BITS(entity)->bit31) {
        chase_angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk79 == 0) {
            unkE0->unk79 = ABS(chase_angle) > 0.5235988f;
            if (unkE0->unk79 == 0) {
                goto done;
            }
        }
        entity->unkC8 = func_00190BE0(entity);
        if (entity->unkC8 == 0) {
            unkE0->unk79 = 0;
        }
    } else {
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
