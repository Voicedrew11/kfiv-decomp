#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);

void func_0013B568(Entity *entity, s32 unknown) {
    EntityModel *model = entity->model;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit20 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0;
    model->position[0] = -16200.0f;
    model->position[1] = 3030.0f;
    model->position[2] = 4900.0f;
    model->unkD0[1] = 1.5707964f;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8((char *)entity, entity->unkE0.unk18, unknown);
}
