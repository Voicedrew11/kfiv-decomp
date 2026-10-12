#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
void func_00196D48(Entity *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00180590(Entity *);
void func_001807D0(Entity *);
s32 func_00180A90(Entity *);
s32 func_00180CA0(Entity *, s32, f32, f32);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);

void func_00180170(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            unkE0->unk7B = func_00180A90(entity);
            if (unkE0->unk7B == 0 && ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                return;
            }
            if (unkE0->unk7B != 0) {
                if (ENTITY_FLAG_BITS(entity)->bits58 == unkE0->unk7A) {
                    func_001958B8((char *)entity, (void (*)(void))func_00180590, unknown);
                } else {
                    unkE0->unk7A = ENTITY_FLAG_BITS(entity)->bits58;
                    func_001958B8((char *)entity, (void (*)(void))func_001807D0, unknown);
                }
            } else {
                if (ENTITY_FLAG_BITS(entity)->bits58 == unkE0->unk7A) {
                    func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
                } else {
                    unkE0->unk7A = ENTITY_FLAG_BITS(entity)->bits58;
                    func_001958B8((char *)entity, (void (*)(void))func_001807D0, unknown);
                }
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            if (unkE0->unk7A == 0) {
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 1;
            } else {
                entity->unkA0 = rand() % 60 + 0x3C;
                unkE0->unk78 = 0;
            }
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
    if (entity->unkCD == 1) {
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
            switch (unkE0->unk78) {
            case 0:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
                }
                break;
            case 1:
                if (turn == 0) {
                    entity->unkC8 = func_00180CA0(entity, 0, 200.0f, 800.0f);
                } else {
                    entity->unkC8 = func_00180CA0(entity, turn, 200.0f, 800.0f);
                }
                break;
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
