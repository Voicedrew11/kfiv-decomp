#include "kf_entity.h"

int rand(void);
void *func_001267B0(int);
void func_0018DC40(char *);

void func_00150478(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 remaining = 2;
    s32 chance = 200;
    s32 i;
    s32 slots_left = 6 - unkE0->unk7A;

    for (i = 0; i < 6; i++) {
        if (!((unkE0->unk6A >> i) & 1)) {
            if (rand() % 100 < chance / slots_left) {
                Entity *other = func_001267B0(i + 10);
                if (other != NULL && ((s32)other->flags & 1)) {
                    chance -= 100;
                    func_0018DC40((char *)other);
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
    unkE0->unk68 = entity->params->unk434;
}
