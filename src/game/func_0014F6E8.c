#include "kf_entity.h"

extern u32 D_00446ED4;
extern float D_00413F80[];

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
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
void func_0014FF68(Entity *);
void func_0014FCC0(Entity *);
s32 func_001503A8(Entity *);
void func_00150200(Entity *);

void func_0014F6E8(Entity *entity, s32 unknown) {
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
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    if (func_001503A8(entity)) {
        func_001958B8((char *)entity, (void (*)(void))func_00150200, unknown);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
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
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_0014FCC0, unknown);
                } else if (ENTITY_FLAG_BITS(entity)->bits58 == 1) {
                    func_001958B8((char *)entity, (void (*)(void))func_0014FF68, unknown);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                }
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
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
            turn = func_0018EAC0(entity, direction);
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
