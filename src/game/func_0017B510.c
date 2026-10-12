#include "kf_entity.h"

int func_00197078(void);
int func_0018F030(char *);
float func_00194F80(void *, void *);
void func_00216778(void *, void *);
s16 func_00190BE0(Entity *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_0017AE10(Entity *);

void func_0017B510(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 near_point[4];
    f32 far_point[4];
    f32 range;

    // match: takes the entity; the shared prototype has no parameters
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    range = 100.0f;
    if (entity->model->position[0] > -800.0f || entity->model->position[0] < -1200.0f) {
        unkE0->unk30[0] = -950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = entity->unkE0.unk30;
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    } else {
        if (unkE0->unk79 == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1000.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1000.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk42;
        } else if (unkE0->unk7A == 0) {
            near_point[0] = -900.0f;
            near_point[1] = entity->spawn_info->spawn_y;
            near_point[2] = -1800.0f;
            near_point[3] = 0.0f;
            far_point[0] = -800.0f;
            far_point[1] = entity->spawn_info->spawn_y;
            far_point[2] = -1800.0f;
            far_point[3] = 0.0f;
            unkE0->unk7B = entity->spawn_info->unk43;
        }
        if (func_00194F80(entity->model->position, near_point) < range) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            func_00216778(unkE0->unk30, far_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                func_001958B8((char *)entity, (void (*)(void))func_0017AE10, unknown);
                return;
            }
        } else {
            func_00216778(unkE0->unk30, near_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(range * 0.5f, entity, 0);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
