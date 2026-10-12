#include "kf_entity.h"

float func_00194F80(void *, void *);
float func_001957B8(float, float);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_0013BC60(Entity *);

void func_0013BA70(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *point;
    f32 angle;
    f32 range = 50.0f;

    ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit26 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    entity->unkD6 = 0x80;
    unkE0->unk30[0] = -18450.0f;
    unkE0->unk30[1] = 3190.0f;
    unkE0->unk30[2] = 4900.0f;
    if (entity->model->position[1] <= 3030.0f) {
        unkE0->unk7A = 7;
    } else {
        point = unkE0->unk30;
        if (func_00194F80(entity->model->position, point) < range) {
            angle = func_001957B8(entity->model->unkD0[1], -1.5707964f);
            if (ABS(angle) > 0.17453292f) {
                if (angle >= 0.0f) {
                    entity->unkC8 = 0x200;
                } else {
                    entity->unkC8 = 0x100;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 0;
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_0013BC60, unknown);
                return;
            }
        } else {
            entity->target = point;
            entity->unkC8 = func_00190F90(range, entity, 0);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
