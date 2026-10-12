#include "kf_entity.h"

extern ModelInstanceBlock D_003E7140[2];

void func_0010B018(ModelInstance *, s32, s32);

void func_00133708(Entity *entity, s32 unknown) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;

    if (unkE0->unk7E != 0 && ENTITY_FLAG_BITS(entity)->bits2 == 0) {
        D_003E7140[0].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[0].model.unk148 = ENTITY_MODEL(entity)->unk148;
        D_003E7140[1].model.unk14C = ENTITY_MODEL(entity)->unk14C;
        D_003E7140[1].model.unk148 = ENTITY_MODEL(entity)->unk148;
        func_0010B018(&D_003E7140[0].model, 0, unknown);
        func_0010B018(&D_003E7140[1].model, 0, unknown);
    }
}
