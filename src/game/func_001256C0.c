#include "kf_entity.h"
#include "kf_map.h"
#include "kf_collision.h"

#define SCRATCH_MATRIX ((f32 *)0x70000000)
#define SCRATCH_VECTOR ((f32 *)0x70000040)
#define RECORD(entity) ((MapRecord *)(entity)->unk14)
#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->unk38)

extern int D_0038CC80[3];
extern Entity *D_004467E0;
extern PlayerData D_00413F80;
extern MapDrawState D_0038A1B0;
extern f32 D_00431AF0[16];

void func_0010C9E8(ModelInstance *);
s32 func_001963A8(Entity *);
void func_0018DC40(char *);
s32 func_001962E8(Entity *);
f32 func_00194F48(f32 *, f32 *);
float func_00194F80(void *, void *);
float func_001954A8(float *, float *);
int func_00126240(Entity *);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
float func_00194FB8(float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void func_00126120(Entity *);
MapRecordBounds *func_001263D8(Entity *);

/* The per-frame update of every live entity: respawn, the countdowns, where the player is, the push it is
   under, its own update function, and the map blocks its model is drawn with. */
void func_001256C0(void) {
    Entity *entity;
    EntitySpawnInfo *spawn_info;

    if (D_0038CC80[0] & 4) {
        return;
    }
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        func_0010C9E8(ENTITY_MODEL(entity));
        if (ENTITY_FLAG_BITS(entity)->bits2 >= 3 || !ENTITY_FLAG_BITS(entity)->bit1) {
            if (!func_001963A8(entity)) {
                continue;
            }
            func_0018DC40((char *)entity);
        }
        if (func_001962E8(entity)) {
            continue;
        }
        if (entity->unk82 != 0) {
            entity->unk82--;
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->bit16) {
            entity->unk20 = 0;
            entity->unk24 = 0;
            ENTITY_FLAG_BITS(entity)->pad40 = FALSE;
            entity->unk28 = 0;
            entity->unk2A = 0;
        }
        entity->unk60 = ((f32 (*)(f32 *, f32 *))func_00194F48)(entity->model->position, D_00413F80.position); // match: returns the distance (other files declare it void)
        entity->distance_to_player = func_00194F80(entity->model->position, D_00413F80.position);
        entity->unk68 = func_001954A8(entity->model->position, D_00413F80.position);
        entity->unk84 = entity->unk88 = entity->unk8C = entity->unk90 = entity->unk94 = entity->unk98 = 0.0f;
        if (!func_00126240(entity)) {
            if (!ENTITY_FLAG_BITS(entity)->bit30) {
                func_0020C300(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->unkE0);
            }
            ENTITY_FLAG_BITS(entity)->bit30 = TRUE;
            continue;
        }
        ENTITY_FLAG_BITS(entity)->bit30 = FALSE;
        if (entity->unk80 != 0) {
            entity->unk80--;
        }
        if (entity->unkA3 != 0) {
            entity->unkA3--;
        }
        if (entity->unk24 != 0) {
            entity->unk24--;
        }
        if (entity->unk28 != 0) {
            entity->unk28--;
        }
        if (entity->unk2A != 0) {
            entity->unk2A--;
        }
        if (entity->unk20 != 0) {
            if (entity->unk20 % 30 == 0) {
                entity->unk2C = (entity->unk2C < 3) ? 0 : entity->unk2C - 3;
                entity->flags &= ~(3L << 38);
            }
            entity->unk20--;
        }
        if (ENTITY_FLAG_BITS(entity)->bits36 != 0) {
            if (entity->params->unk0A < entity->unk2E) {
                entity->unk2E = entity->params->unk0A;
            }
            if (ENTITY_FLAG_BITS(entity)->pad40) {
                entity->unk4C = (f32)entity->unk2E * 75.0f / (f32)entity->params->unk0A;
                entity->unkA3 = 6;
            } else {
                entity->unk4C = (f32)entity->unk2E * 25.0f / (f32)entity->params->unk0A;
                entity->unkA3 = 3;
            }
        }
        if (entity->params->flags & 0x1000) {
            entity->unk4C = 0.0f;
        }
        if (entity->unk4C > 0.0f) {
            f32 forward = 1.0f; // match: declared ahead of the pointer, or the pointer is hoisted out of the loop
            f32 *push = SCRATCH_VECTOR;

            push[0] = 0.0f;
            push[1] = 0.0f;
            push[2] = forward;
            push[3] = 0.0f;
            sceVu0RotMatrixY(SCRATCH_MATRIX, D_00431AF0, func_00194FB8(entity->unk6C));
            sceVu0ApplyMatrix(SCRATCH_VECTOR, SCRATCH_MATRIX, SCRATCH_VECTOR);
            KF_Vector_Scale(SCRATCH_VECTOR, SCRATCH_VECTOR, entity->unk4C);
            func_00216C48(entity->model->position, entity->model->position, SCRATCH_VECTOR);
        }
        if (entity->unk50 == 0) {
            continue;
        }
        ((void (*)(Entity *))entity->unk50)(entity);
        if (entity->unk4C > 0.0f) {
            entity->unk4C -= entity->unk40 * 4.0f / 10.0f;
        }
        func_00126120(entity);
        if (entity->unk14 == NULL) {
            entity->unk14 = func_001263D8(entity);
        }
        spawn_info = entity->spawn_info;
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[2] != -1 || entity->unk14 == NULL) {
            ENTITY_MODEL(entity)->unk14C = &D_0038A1B0.unk290[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[2]];
        } else {
            ENTITY_MODEL(entity)->unk14C = &D_0038A1B0.unk290[(u8)(RECORD(entity)->unk38 >> 27) & 1][RECORD(entity)->unk36];
        }
        if ((s8)SPAWN_FLAGS(spawn_info)->bytes[3] != -1 || entity->unk14 == NULL) {
            ENTITY_MODEL(entity)->unk148 = (s32)&D_0038A1B0.unkA90[entity->group_id][(s8)SPAWN_FLAGS(spawn_info)->bytes[3]];
        } else {
            ENTITY_MODEL(entity)->unk148 = (s32)&D_0038A1B0.unkA90[(u8)(RECORD(entity)->unk38 >> 27) & 1][RECORD(entity)->unk37];
        }
        func_0010C9E8(ENTITY_MODEL(entity));
    }
}
