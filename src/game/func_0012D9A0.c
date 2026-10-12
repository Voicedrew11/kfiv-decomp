#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
s16 func_00191340(void *, int);
float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);

int func_0012D9A0(void *entity_arg, int unknown) {
    Entity *entity = entity_arg;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
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
            unkE0->unk4C = entity->params->unk432;
            unkE0->unk4C += rand() % (ENTITY_CONTROLLER(entity)->unk434 * 2) - ENTITY_CONTROLLER(entity)->unk434;
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
        turn = func_0018EAC0(entity, direction);
        if (turn != 0) {
            entity->unkC8 = func_00191340(entity, turn);
        } else {
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - unkE0->unk4C - entity->model->position[1];
            if (ABS(delta) > entity->params->unk54) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x20;
                } else {
                    entity->unkC8 |= 0x10;
                }
            }
            entity->unkC8 |= 1;
            if (entity->distance_to_player < entity->radius + 100.0f + 30.0f) {
                entity->unkA0 = 0;
            }
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
