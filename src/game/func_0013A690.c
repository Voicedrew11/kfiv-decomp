#include "kf_entity.h"

int func_00197078(void);
int func_0018F030(char *);
float func_00194F80(void *, void *);
float func_001957B8(float, float);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_001387C0(Entity *);

void func_0013A690(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    if (unkE0->unk6A > 0) {
        unkE0->unk18();
        return;
    }
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
    if (entity->model->position[0] < 6650.0f || entity->model->position[0] > 7350.0f) {
        unkE0->unk30[0] = entity->spawn_info->spawn_x;
        entity->target = unkE0->unk30;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[3] = 0.0f;
        unkE0->unk30[2] = -4200.0f;
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    } else if (func_00194F80(entity->model->position, entity->spawn_info) < 100.0f) {
        angle = func_001957B8(entity->model->unkD0[1], entity->spawn_info->unk14);
        if (ABS(angle) > 0.17453292f) {
            func_001958B8((char *)entity, (void (*)(void))func_001387C0, 1);
            return;
        }
        entity->prev_state = entity->state;
        entity->state = 0;
        return;
    } else {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    ((void (*)(Entity *))unkE0->unk18)(entity);
}
