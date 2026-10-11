#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;
extern char D_0043E928[];

int printf(const char *, ...);
s32 func_0015C518(Entity *, f32, f32);
s32 func_0015C688(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
int func_0015C958(char *, float);
s16 func_00190F90(float, void *, int);
float func_001957B8(float, float);
float func_00195478(float *, char *);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(void);
void func_00181C60(void);
void func_00181E48(void);
void func_00182240(void);

void func_0015CC20(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (func_0015C518(entity, 200.0f, 1.0471976f)) {
            func_001958B8((char *)entity, func_00181AE0, 1);
            break;
        }
        if (func_0015C688(entity, 300.0f, 0.5235988f)) {
            func_001958B8((char *)entity, func_00181C60, 1);
            break;
        }
        if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
            func_001958B8((char *)entity, func_00181AE0, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        side = func_0018FD50(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = 0;
            if (entity->unkCF != 0) {
                entity->unk9E = D_00446ED4;
            }
            func_00190B88(entity, 0);
            if (entity->unkCD == 0) {
                unkE0->unk68 = 2;
                if (func_0018F030((char *)entity)) {
                    func_001958B8((char *)entity, func_00182240, 1);
                } else {
                    func_001958B8((char *)entity, func_00181E48, 1);
                }
            } else {
                printf(D_0043E928);
            }
            break;
        }
        entity->target = D_00413F80;
        angle = func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
        side = func_0018EAC0(entity, direction);
        if (ABS(angle) > 0.7853982f) {
            entity->unkC8 = 2;
        } else {
            entity->unkC8 = 0x41;
        }
        if (side > 0) {
            entity->unkC8 |= 0x200;
        } else if (side < 0) {
            entity->unkC8 |= 0x100;
        } else if (ABS(angle) > 0.7853982f) {
            // match: the caller passes a second integer argument the callee does not take
            entity->unkC8 = ((int (*)(Entity *, int, float))func_0015C958)(entity, 1, entity->radius + 100.0f + 30.0f);
        } else {
            entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
        }
        ((void (*)(Entity *))unkE0->unk18)(entity);
        break;
    }
}
