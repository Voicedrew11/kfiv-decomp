#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);

extern float D_00413F80[];

void func_00177040(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;

    if (unkE0->unk7A != 0) {
        unkE0->unk7A--;
    } else {
        unkE0->unk48 = D_00413F80[0];
        unkE0->unk4C = D_00413F80[1] - 220.0f;
        unkE0->unk50 = D_00413F80[2];
        unkE0->unk7A = (u8)entity->params->unk436;
    }
    ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit21 = TRUE;
    ENTITY_FLAG_BITS(entity)->bit16 = TRUE;
    if (entity->spawn_info->unk40 != 0) {
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    } else {
        ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit29) {
        func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    } else {
        func_00192C98(entity, 1, 0, entity->params->unkC4, 0);
    }
}
