#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00197078(void);
int func_0018F030(char *);
float func_00194F80(void *, void *);
float func_001954A8(float *, float *);
float func_001957B8(float, float);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0013A398(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction;
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
    range = 100.0f;
    if (D_00413F80.position[0] < 6150.0f) {
        unkE0->unk30[0] = entity->unk30 + 6150.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(50.0f, entity, 0);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else if (D_00413F80.position[0] > 8000.0f) {
        unkE0->unk30[0] = 8000.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4400.0f;
        if (func_00194F80(entity->model->position, unkE0->unk30) > range) {
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(50.0f, entity, 0);
        } else {
            entity->target = &D_00413F80;
            direction = func_001954A8(entity->model->position, D_00413F80.position);
            angle = func_001957B8(entity->model->unkD0[1], direction);
            if (ABS(angle) > 1.5707964f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
            }
        }
    } else {
        entity->target = &D_00413F80;
        entity->unkC8 = func_00190F90(entity->radius + range + 30.0f, entity, 0);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}
