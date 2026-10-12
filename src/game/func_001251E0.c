#include "kf_entity.h"

extern Vector4Arg D_0043E7C0;
extern s32 D_003E6E40[][2];

int rand(void);
void func_00216778(void *, void *);
void KF_Vector_Scale(float *, float *, float);
void KF_Vector_MultiplyComponents(float *, float *, float *);
int func_00192C98(void *, int, int, u32, int);
void func_00126120(Entity *);

/* Puts an entity back to how it spawns: flags, counters, position, rotation and its sizes scaled. */
void func_001251E0(Entity *entity) {
    Vector4Arg scale = D_0043E7C0;
    f32 widest;

    // match: read first; after the stores below, the scheduler would hold this load back behind them
    entity->unk2C = entity->params->unk0A;
    ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits2 = 0;
    ENTITY_FLAG_BITS(entity)->bit14 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit15 = FALSE;
    ENTITY_FLAG_BITS(entity)->pad8 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit19 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit22 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit24 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit25 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit30 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit17 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit31 = FALSE;
    entity->unk50 = 0;
    entity->handler = NULL;
    entity->prev_handler = NULL;
    entity->unk82 = 0;
    entity->unk80 = 0;
    entity->unkA3 = 0;
    entity->unk9E = 0;
    entity->unkA2 = 0;
    entity->unk20 = 0;
    entity->unk24 = 0;
    entity->unk28 = 0;
    entity->unk2A = 0;
    entity->action_step = 0;
    entity->unkCB = 0;
    entity->state = 0;
    entity->unkCD = 0;
    entity->prev_state = 0;
    entity->unkCF = 0;
    ((EntityFlags *)&entity->flags)->halves[1] = 0;
    entity->unkC6 = 0;
    entity->unkC8 = 0;
    entity->unkA0 = 0;
    entity->unk48 = 10.0f;
    func_00216778(entity->model->position, entity->spawn_info);
    func_00216778(entity->model->unkD0, &entity->spawn_info->unk10);
    KF_Vector_Scale(entity->model->unkE0, &scale.x, (f32)entity->params->unkD2 / 100.0f);
    func_00216778(&scale, entity->spawn_info->unk20);
    if (!(entity->params->flags & 0x240)) {
        KF_Vector_Scale(&scale.x, &scale.x, (f32)(rand() % 11 + 95) / 100.0f);
    }
    KF_Vector_MultiplyComponents(entity->model->unkE0, entity->model->unkE0, &scale.x);
    widest = (scale.z < scale.x) ? scale.x : scale.z;
    entity->unk30 = (f32)entity->params->unk06 * widest;
    entity->unk34 = (f32)entity->params->unk08 * scale.y;
    entity->radius = (f32)entity->params->unkCC * widest;
    entity->height = (f32)entity->params->unkEE * scale.y;
    entity->unk44 = (f32)entity->params->unk04 * widest;
    entity->unk40 = (f32)entity->params->unk0C * scale.x * scale.y * scale.z;
    entity->unk40 = ABS(entity->unk40);
    entity->model->unk13C = (entity->unk30 < entity->unk34) ? entity->unk34 : entity->unk30;
    func_00192C98(entity, 0, 0, 0, 1);
    ((void (*)(Entity *))D_003E6E40[entity->params->unk02][0])(entity);
    func_00126120(entity);
}
