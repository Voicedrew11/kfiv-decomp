#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
s32 func_00164950(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_00164940(char *);
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
void func_00163848(Entity *);
void func_001639A8(Entity *);
void func_001640B8(Entity *);
void func_00164480(Entity *);
void func_001646B8(Entity *);
void func_00181AE0(void);
void func_00181E48(void);
void func_00182240(void);

void func_00163310(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind162BC8 *work = (EntityUnkE0Kind162BC8 *)unkE0;
    f32 direction[4];
    f32 angle;
    s32 mode;
    s32 side;

    entity->flags = (entity->flags & ~0x3800) | 0x800;
    if (func_00164950(entity, 200.0f, 2.443461f)) {
        func_001958B8((char *)entity, (void (*)(void))func_001639A8, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    if (func_00164940((char *)entity)) {
        func_001958B8((char *)entity, (void (*)(void))func_00164480, unknown);
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
        func_00190B88(entity, mode);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 1) {
                    func_001958B8((char *)entity, (void (*)(void))func_001646B8, unknown);
                } else {
                    func_001958B8((char *)entity, func_00182240, unknown);
                }
            } else {
                if (((s32)(entity->flags >> 58) & 3) == 3) {
                    func_001958B8((char *)entity, (void (*)(void))func_001640B8, unknown);
                } else {
                    func_001958B8((char *)entity, func_00181E48, unknown);
                }
            }
            return;
        case 1:
            entity->target = D_00413F80;
            unkE0->unk78 = 0;
            if (entity->distance_to_player > 500.0f) {
                entity->unkA0 = rand() % 30 + 60;
                work->unk6E = 1;
            } else {
                entity->unkA0 = rand() % 30 + 60;
                work->unk6E = 0;
            }
            break;
        case 2:
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        side = func_0018EAC0(entity, direction);
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
        } else if (work->unk6E != 0) {
            if (side > 0) {
                entity->unkC8 = 0x241;
            } else if (side < 0) {
                entity->unkC8 = 0x141;
            } else {
                if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                    && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                    unkE0->unk7B = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    func_001958B8((char *)entity, (void (*)(void))func_00163848, unknown);
                    return;
                }
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
            entity->unkC8 |= 0x400;
        } else {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f
                    && D_00446ED4 % 15U == 0 && !(rand() & 1)) {
                    unkE0->unk7B = (func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f) ? 1 : -1;
                    func_001958B8((char *)entity, (void (*)(void))func_00163848, unknown);
                    return;
                }
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
