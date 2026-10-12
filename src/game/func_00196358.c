#include "kf_entity.h"

int func_00196358(Entity *entity) {
    if (entity->unk14 != NULL && (((EntitySpawnInfo *)entity->unk14)->unk38 & 0x04000000) == 0) {
        return 0;
    }
    if (entity->distance_to_player <= 8000.0f) {
        return 1;
    }
    return 0;
}
