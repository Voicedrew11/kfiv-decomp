#include "kf_entity.h"

extern u32 D_00446ED4;
extern float D_00413F80[];

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194F80(void *, void *);
void func_0018BA10(f32 *, f32 *, f32, f32);
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
void func_001587A8(Entity *);
void func_00158948(Entity *);
void func_00159180(Entity *);

void func_00157E70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 retreat_point[4];
    f32 destination[4];
    f32 direction[4];
    EntityModel *model;
    f32 height_difference;
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
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
        return;
    }
    if (unkE0->unk7B == 1 && unkE0->unk6A == 0) {
        func_001958B8((char *)entity, (void (*)(void))func_001587A8, 1);
        return;
    }
    if (entity->unkA0 == 0 && rand() % 100 < entity->params->unk43A) {
        func_001958B8((char *)entity, (void (*)(void))func_001587A8, 1);
        return;
    }
    if (entity->unkA0 == 0 && rand() % 100 < entity->params->unk438) {
        func_001958B8((char *)entity, (void (*)(void))func_00158948, 1);
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
            model = entity->model;
            height_difference = D_00413F80[1] - model->position[1];
            if (ABS(height_difference) > 200.0f) {
                func_0018BA10(retreat_point, model->position, func_00194F80(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = retreat_point;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
            } else if (unkE0->unk7B == 0) {
                if (func_0018F030((char *)entity)) {
                    if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                        func_001958B8((char *)entity, (void (*)(void))func_00159180, unknown);
                    } else {
                        func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                    }
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
                }
                return;
            } else {
                func_0018BA10(destination, model->position, func_00194F80(model->position, entity->spawn_info), entity->radius * 4.0f);
                entity->target = destination;
                entity->unkA0 = rand() % 30 + 60;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
            }
            break;
        case 1:
            entity->target = D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
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
