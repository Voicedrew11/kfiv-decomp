#include "kf_entity.h"

void func_001760B8(void *, int);
void func_00176238(void *, int);

void func_00176050(Entity *entity, int frame) {
    if (entity->unkC6 != 0) {
        return;
    }
    if (entity->distance_to_player < (float)ENTITY_CONTROLLER(entity)->unk430) {
        // match: retail reads the u8 state signed (lb)
        if ((s8)entity->state != 3) {
            entity->state = 3;
        }
    }
    if ((s8)entity->state == 3) {
        func_00176238(entity, frame);
        return;
    }
    func_001760B8(entity, frame);
}
