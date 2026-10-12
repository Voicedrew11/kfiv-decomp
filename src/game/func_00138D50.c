#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00197078(void);
int func_0018F030(char *);
float func_00194F80(void *, void *);
float func_001957B8(float, float);
void func_00216778(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_00138D50(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    f32 facing;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (!(s32)(entity->flags >> 63) && ((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    if (D_00413F80.position[0] < -6000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = 1.5707964f;
    } else if (D_00413F80.position[0] > -4350.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4400.0f;
        post[3] = 0.0f;
        facing = -1.5707964f;
    } else if (D_00413F80.position[2] < -4000.0f) {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = entity->unk30 + -4800.0f;
        post[3] = 0.0f;
        facing = 0.0f;
    } else {
        post[0] = -5200.0f;
        post[1] = entity->spawn_info->spawn_y;
        post[2] = -4000.0f - entity->unk30;
        post[3] = 0.0f;
        facing = 3.1415927f;
    }
    if (entity->model->position[2] < -4800.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(50.0f, entity, 1);
    } else if (func_00194F80(entity->model->position, post) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], facing);
        if (ABS(angle) > 0.5235988f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, post);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(50.0f, entity, 1) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}
