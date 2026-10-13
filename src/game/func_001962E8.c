#include "common.h"
#include "kf_entity.h"
#include "kf_effect.h"

/* Clears the 0x2 flag once the entity's position value passes 10000. */
s32 func_001962E8(Entity *entity) {
    u16 ctrl_flags;

    if (*(f32 *)((char *)entity->model + 0xC4) > 10000.0f) {
        ctrl_flags = ENTITY_CONTROLLER(entity)->unkC6;
        if (ctrl_flags != 0 && ctrl_flags != 0xFFFF) {
            KF_Effect_FreeForOwner(entity);
        }
        entity->flags &= ~2;
        return 1;
    }
    return 0;
}
