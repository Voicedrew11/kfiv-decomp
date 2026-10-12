#include "kf_entity.h"

int func_00197078(void);
int func_0018F030(char *);
float func_00194F80(void *, void *);
float func_001957B8(float, float);
void func_00216778(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0013A210(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    home[0] = entity->spawn_info->spawn_x;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = -4400.0f;
    home[3] = 0.0f;
    range = 100.0f;
    if (func_00194F80(entity->model->position, home) < range) {
        angle = func_001957B8(entity->model->unkD0[1], 3.1415927f);
        if (ABS(angle) > 1.5707964f) {
            if (angle >= 0.0f) {
                entity->unkC8 = 0x200;
            } else {
                entity->unkC8 = 0x100;
            }
        }
    } else {
        func_00216778(unkE0->unk30, home);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(50.0f, entity, 1) | 0x400;
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}
