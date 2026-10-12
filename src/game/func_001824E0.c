#include "kf_entity.h"

void func_00216778(void *, void *);
void func_0019A348(Entity *);
void func_0019A388(Entity *);
void func_001958B8(char *, void (*)(void), int);
float func_00194FB8(float);
void func_00199BB0(Entity *);

/* per-frame update of an enemy: run its handlers, count its timers down, apply this frame's move */
void func_001824E0(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    func_00216778(entity->unk70, entity->model->position);
    if (ENTITY_FLAG_BITS(entity)->bit41) {
        func_0019A348(entity);
    }
    if (ENTITY_FLAG_BITS(entity)->bit42) {
        func_0019A388(entity);
    }
    if (!(entity->unkC6 & 0x60)) {
        if (entity->unk2C == 0) {
            unkE0->unk0C = NULL;
            func_001958B8((char *)entity, unkE0->unk14, 0);
        } else if (ENTITY_FLAG_BITS(entity)->bits36 != 0 && !ENTITY_FLAG_BITS(entity)->bit35) {
            if (unkE0->unk08 != NULL) {
                ((void (*)(Entity *))unkE0->unk08)(entity);
            }
        } else if (ENTITY_FLAG_BITS(entity)->bit5) {
            if (unkE0->unk00 != NULL) {
                unkE0->unk00(entity, 0);
            }
        }
    }
    ENTITY_FLAG_BITS(entity)->bit41 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit42 = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bit6) {
        if (unkE0->unk0C != NULL) {
            ((void (*)(Entity *))unkE0->unk0C)(entity);
        } else if (entity->handler != NULL) {
            entity->handler(entity);
        }
    }
    if (entity->unkA0 != 0) {
        entity->unkA0--;
    }
    if (entity->unkA2 != 0) {
        entity->unkA2--;
    }
    if (unkE0->unk04 != NULL) {
        ((void (*)(Entity *))unkE0->unk04)(entity);
    }
    if (entity->unk28 != 0 && !(entity->unkC6 & 0x46)) {
        entity->model->position[0] += entity->unk84 * 0.75f;
        entity->model->position[1] += entity->unk88 * 0.75f;
        entity->model->position[2] += entity->unk8C * 0.75f;
        entity->model->unkD0[0] = func_00194FB8(entity->model->unkD0[0] + entity->unk90 * 0.75f);
        entity->model->unkD0[1] = func_00194FB8(entity->model->unkD0[1] + entity->unk94 * 0.75f);
        entity->model->unkD0[2] = func_00194FB8(entity->model->unkD0[2] + entity->unk98 * 0.75f);
    } else {
        entity->model->position[0] += entity->unk84;
        entity->model->position[1] += entity->unk88;
        entity->model->position[2] += entity->unk8C;
        entity->model->unkD0[0] = func_00194FB8(entity->model->unkD0[0] + entity->unk90);
        entity->model->unkD0[1] = func_00194FB8(entity->model->unkD0[1] + entity->unk94);
        entity->model->unkD0[2] = func_00194FB8(entity->model->unkD0[2] + entity->unk98);
    }
    ENTITY_FLAG_BITS(entity)->bits36 = 0;
    entity->unkC8 = 0;
    ENTITY_FLAG_BITS(entity)->bit43 = FALSE;
    func_00199BB0(entity);
}
