#include "kf_entity.h"
#include "kf_entity_world.h"

int rand(void);
void func_0018DC40(char *);

void func_00168000(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 remaining = 1;
    s32 i;
    s32 slots_left = entity->params->unk43C - unkE0->unk7A;

    for (i = 0; i < entity->params->unk43C; i++) {
        if (!((unkE0->unk6A >> i) & 1)) {
            if (rand() % 100 < remaining * 100 / slots_left) {
                s32 other_id = i + 10;
                Entity *other = Entity_GetBySpawnId(other_id);
                if (other != NULL && ((s32)other->flags & 1)) {
                    func_0018DC40((char *)other);
                    unkE0->unk7B = other_id;
                    remaining--;
                }
            }
            if (remaining <= 0) {
                break;
            }
            slots_left--;
            if (slots_left <= 0) {
                break;
            }
        }
    }
    unkE0->unk68 = entity->params->unk43E;
}
