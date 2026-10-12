#include "kf_entity.h"

float func_00194F80(void *, void *);
int func_00197078(void);
int func_0018CA50(int *);
int func_0018F030(char *);
float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0013EE40(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 home[4];
    f32 angle;
    f32 range = 100.0f;

    home[0] = -7900.0f;
    home[1] = entity->spawn_info->spawn_y;
    home[2] = 3050.0f;
    home[3] = 0.0f;
    if (func_00194F80(entity->model->position, home) > range) {
        unkE0->unk78 = 0;
    }
    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity) && !func_0018CA50((int *)entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    angle = func_001957B8(entity->model->unkD0[1], unkE0->unk48);
    if (ABS(angle) > entity->params->turn_limit) {
        if (angle >= 0.0f) {
            entity->unkC8 |= 0x200;
        } else {
            entity->unkC8 |= 0x100;
        }
    } else {
        unkE0->unk79 = 0;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
