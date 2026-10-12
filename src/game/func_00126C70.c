#include "kf_entity.h"

int rand(void);
void func_0018B778(float, void *, void *, void *);
float func_00194F80(void *, void *);
float func_00195818(void);
float func_001957B8(float, float);
int func_001958B8(Entity *, void (*)(Entity *), int);

/* wander: every so often walk to a point near the spawn point, turn to a random heading, or stand still */
void func_00126C70(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 roll;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        roll = rand() % 100;
        if (roll > 60) {
            func_0018B778(entity->spawn_info->unk4C, entity->unkE0.unk30, entity->spawn_info, entity->model->position);
            entity->target = entity->unkE0.unk30;
            entity->unkA0 = rand() % 120 + 240;
            unkE0->unk78 = 0;
        } else if (roll > 40) {
            unkE0->unk48 = func_00195818();
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 1;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 2;
        }
    }
    if (unkE0->unk78 == 0) {
        if (entity->target != NULL) {
            if (func_00194F80(entity->model->position, entity->target) > 50.0f) {
                entity->unkC8 = 1;
            }
        }
    } else if (unkE0->unk78 == 1) {
        turn = func_001957B8(entity->model->unkD0[1], unkE0->unk48);
        if (ABS(turn) > entity->params->turn_limit) {
            entity->unkC8 = (turn >= 0.0f) ? 0x200 : 0x100;
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
