#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194FB8(float);
float func_00195478(float *, char *);
float func_001957B8(float, float);
s32 func_0018EC78(Entity *, f32);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_0016CE08(Entity *);
void func_0016D210(Entity *);
s32 func_0016D6C0(Entity *);

void func_0016C3A0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
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
            if (func_0018F030((char *)entity)) {
                func_001958B8((char *)entity, (void (*)(void))func_0016D210, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_0016CE08, unknown);
            }
            return;
        case 1:
            entity->target = &D_00413F80;
            if ((f32)entity->params->unk438 < entity->distance_to_player) {
                unkE0->unk78 = 0;
            } else if (entity->distance_to_player < (f32)entity->params->unk436) {
                unkE0->unk78 = 1;
            } else if (func_001957B8(D_00413F80.unk14,
                                     func_00195478(D_00413F80.position, (char *)entity->model->position)) > 0.0f) {
                unkE0->unk78 = 2;
            } else {
                unkE0->unk78 = 3;
            }
            entity->unkA0 = entity->params->unk43A;
            break;
        case 2:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 4;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (func_0016D6C0(entity)) {
        return;
    }
    switch (unkE0->unk78) {
    case 0:
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 |= 0x201;
        } else if (turn < 0) {
            entity->unkC8 |= 0x101;
        } else {
            entity->unkC8 |= func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
        }
        break;
    case 1:
        if (func_0018EC78(entity, func_00194FB8(entity->model->unkD0[1] + 3.1415927f))) {
            unkE0->unk78 = rand() % 3 + 2;
        } else {
            entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
            entity->unkC8 |= 2;
        }
        break;
    case 2:
        entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
        entity->unkC8 |= 4;
        if ((f32)entity->params->unk438 < entity->distance_to_player) {
            entity->unkC8 |= 1;
        } else if (entity->distance_to_player < (f32)entity->params->unk436) {
            entity->unkC8 |= 2;
        }
        break;
    case 3:
        entity->unkC8 |= ((u16 (*)(Entity *))func_00190BE0)(entity); // match: read as unsigned here
        entity->unkC8 |= 8;
        if ((f32)entity->params->unk438 < entity->distance_to_player) {
            entity->unkC8 |= 1;
        } else if (entity->distance_to_player < (f32)entity->params->unk436) {
            entity->unkC8 |= 2;
        }
        break;
    case 4:
        entity->unkC8 |= func_00190BE0(entity);
        break;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
