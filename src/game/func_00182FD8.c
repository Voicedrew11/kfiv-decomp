#include "kf_entity.h"

int rand(void);
int func_0018EAC0(void *, void *);
void func_0018B778(float, void *, void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);

/* wander: every so often pick a point near the spawn point to walk to, or stand still */
int func_00182FD8(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            func_0018B778(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
