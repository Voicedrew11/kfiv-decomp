#include "kf_entity.h"

// match: the call leaves the sixth argument (rate, $f12) unset
int Bone_TurnPart(void *, f32 *, int, f32 *, f32 *);

int func_00196078(Entity *actor, f32 *out_a, int mode, f32 *out_b, f32 *limit) {
    void *target = actor->model->unk138;

    if (target == NULL) {
        return 0;
    }
    if (!(ENTITY_CONTROLLER(actor)->flags & 0x2000)) {
        return 0;
    }
    return Bone_TurnPart(target, out_a, mode, out_b, limit);
}
