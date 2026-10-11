#include "kf_entity.h"

void func_0018D600(char **, int, u32, int);
float func_00195388(float *, float, float);
float func_00194F80(void *, void *);
void func_001958B8(char *, void (*)(void), int);

void func_00176238(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    entity->unkC8 |= 1;
    if (unkE0->unk68 < entity->params->unk43A) {
        func_0018D600((char **)entity, entity->params->unk438, 0xFF, (int)entity->model->unkD0);
        unkE0->unk68++;
    }
    // match: the whole step is written out in both arms (only the call is shared in retail)
    if (-2000.0f < entity->model->position[1]) {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, -0.6981317f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    } else {
        turn = entity->model->unkD0[0];
        func_00195388(&turn, 0.0f, 20.0f);
        entity->unk90 += turn - entity->model->unkD0[0];
    }
    if (func_00194F80(entity->model->position, entity->spawn_info) > 10000.0f) {
        ENTITY_FLAG_BITS(entity)->bit1 = FALSE;
    }
    func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
}
