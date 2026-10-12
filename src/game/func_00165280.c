#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_0018B778(float, void *, void *, void *);
void func_00196D48(Entity *);
float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_00165E68(Entity *);
void func_00166158(Entity *);
void func_00166800(Entity *);
s32 func_00166BF8(Entity *);

void func_00165280(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 point[4];
    f32 direction[4];
    f32 angle;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00166800, 1);
        return;
    }
    if (entity->unkA0 == 0) {
        angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
        if (rand() % 100 < entity->params->unk436 && ABS(angle) < 1.0471976f
            && ABS(entity->unk68 - entity->model->unkD0[1]) < 1.0471976f) {
            unkE0->unk7B = 0;
            func_001958B8((char *)entity, (void (*)(void))func_00165E68, unknown);
            return;
        }
        if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
            && func_00166BF8(entity)) {
            unkE0->unk7B = 0;
            func_001958B8((char *)entity, (void (*)(void))func_00166158, unknown);
            return;
        }
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
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                return;
            }
            angle = func_001957B8(D_00413F80.unk14, func_00195478(D_00413F80.position, (char *)entity->model->position));
            if (rand() % 100 < entity->params->unk436 && ABS(angle) < 0.6981317f) {
                unkE0->unk7B = 1;
                func_001958B8((char *)entity, (void (*)(void))func_00165E68, unknown);
                return;
            }
            if (rand() % 100 < entity->params->unk438 && ABS(angle) < 1.5707964f && entity->spawn_info->unk40 == 0
                && func_00166BF8(entity)) {
                unkE0->unk7B = 1;
                func_001958B8((char *)entity, (void (*)(void))func_00166158, unknown);
                return;
            }
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            return;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 60 + 60;
            unkE0->unk78 = 0;
            break;
        case 2:
            func_0018B778(entity->spawn_info->unk4C, point, entity->spawn_info, entity->model->position);
            entity->target = point;
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
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
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
            entity->unkC8 |= 0x400;
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
