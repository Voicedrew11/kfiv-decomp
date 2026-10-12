#include "kf_entity.h"

float func_00194F80(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);

void func_0013BE18(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point = unkE0->unk30;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -16200.0f;
    unkE0->unk30[1] = 3030.0f;
    unkE0->unk30[2] = 4900.0f;
    if (func_00194F80(entity->model->position, point) < range) {
        unkE0->unk7A = 1;
    } else {
        entity->target = point;
        entity->unkC8 = func_00190F90(range, entity, 0);
    }
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
