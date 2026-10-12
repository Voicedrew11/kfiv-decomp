#include "kf_entity.h"

int func_00197078(void);
int func_0018F030(char *);
void func_00182240(Entity *);
void func_00181E48(Entity *);
void func_001958B8(char *, void (*)(void), int);
float func_00194F80(void *, void *);
void func_00216778(void *, void *);
s16 func_00190BE0(Entity *);
s16 func_00190F90(float, void *, int);
void func_00178E10(Entity *);

void func_00179118(Entity *entity, s32 unknown) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;

    // match: the shared prototype takes no argument; retail passes the entity
    if (((int (*)(Entity *))func_00197078)(entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    reach = 100.0f;
    if (1200.0f < entity->model->position[0] || entity->model->position[0] < 800.0f) {
        unkE0->unk30[0] = 950.0f;
        unkE0->unk30[1] = entity->spawn_info->spawn_y;
        unkE0->unk30[2] = -1400.0f;
        unkE0->unk30[3] = 0.0f;
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    } else {
        if (unkE0->unk7A == 0) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1800.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1800.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk42;
        } else if (unkE0->unk7B == 0) {
            stand_point[0] = 900.0f;
            stand_point[1] = entity->spawn_info->spawn_y;
            stand_point[2] = -1000.0f;
            stand_point[3] = 0.0f;
            face_point[0] = 800.0f;
            face_point[1] = entity->spawn_info->spawn_y;
            face_point[2] = -1000.0f;
            face_point[3] = 0.0f;
            unkE0->unk7C = entity->spawn_info->unk43;
        }
        if (func_00194F80(entity->model->position, stand_point) < reach) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
            func_00216778(unkE0->unk30, face_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                func_001958B8((char *)entity, (void (*)(void))func_00178E10, unknown);
                return;
            }
        } else {
            func_00216778(unkE0->unk30, stand_point);
            entity->target = unkE0->unk30;
            entity->unkC8 = func_00190F90(reach * 0.5f, entity, 0);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
