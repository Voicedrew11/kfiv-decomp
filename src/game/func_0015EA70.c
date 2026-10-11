#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
s32 func_0015F298(Entity *, f32, f32);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
float func_001957B8(float, float);
float func_00194FB8(float);
void func_001958B8(char *, void (*)(void), int);
void func_0015EF60(Entity *);
void func_00181AE0(void);
void func_00181C60(void);
void func_00181E48(void);
void func_00182240(void);

void func_0015EA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    if (entity->params->flags & 0x100) {
        entity->flags &= ~0x3800;
    } else {
        entity->flags = (entity->flags & ~0x3800) | 0x800;
    }
    if ((entity->distance_to_player < entity->radius + 400.0f
         && ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 1.5707964f && D_00446ED4 % 15U == 0
         && rand() % 50 < 80)
        || func_0015F298(entity, 200.0f, 1.0471976f)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    mode = func_0018FD50(entity, 0);
    entity->flags |= 0x200;
    if (entity->unkA0 == 0 || mode == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = mode;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with the entity alone here
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
    if (unkE0->unk78 == 0) {
        func_00196D48(entity);
        if ((s32)(entity->flags >> 31) & 1) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else {
            side = func_0018EAC0(entity, direction);
            if (side > 0) {
                entity->unkC8 = 0x241;
            } else if (side < 0) {
                entity->unkC8 = 0x141;
            } else {
                if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                    && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                    unkE0->unk7A = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    func_001958B8((char *)entity, (void (*)(void))func_0015EF60, unknown);
                    return;
                }
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
