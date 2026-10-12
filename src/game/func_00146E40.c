#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_00196D48(Entity *);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_00146E40(Entity *entity, s32 unknown) {
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
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
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
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                entity->unkA0 = 300;
                unkE0->unk78 = 1;
            } else {
                entity->unkA0 = rand() % 60 + 120;
                unkE0->unk78 = 0;
            }
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
    angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
    if (entity->unk60 - entity->radius - 100.0f < 200.0f && ABS(angle) < 0.6981317f
        && D_00446ED4 % 10U == entity->unkD7 % 10U && rand() % 100 < ENTITY_CONTROLLER(entity)->unk434) {
        angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
        if (unkE0->unk7A == 0) {
            if (ABS(angle) > 0.6981317f && ABS(angle) < 1.5707964f) {
                func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
                unkE0->unk7A = 0;
                return;
            }
            if (!(ABS(angle) >= 1.5707964f)) {
                goto skip;
            }
        }
        entity->unkCF = entity->unkCD;
        entity->target = &D_00413F80;
        entity->unkA0 = 300;
        unkE0->unk78 = entity->unkCD = 1;
        unkE0->unk7A = 0;
    }
skip:
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
                angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
                if (entity->distance_to_player > 900.0f && ABS(angle) < 0.6981317f) {
                    entity->unkA0 = 300;
                    unkE0->unk78 = 1;
                }
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x241;
                } else if (turn < 0) {
                    entity->unkC8 = 0x141;
                } else {
                    entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
                }
                break;
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
