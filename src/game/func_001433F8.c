#include "kf_entity.h"

void *func_001267B0(int);
void func_0018DC40(char *);
int func_00192C98(void *, int, int, u32, int);
void func_001958B8(char *, void (*)(void), int);
void func_001435B0(Entity *, s32);

void func_001433F8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    Entity *leader;
    Entity *other;

    if (entity->spawn_info->unk40 == 2) {
        leader = func_001267B0(entity->spawn_info->variant);
        if (leader != NULL) {
            if (ENTITY_FLAG_BITS(leader)->bits2 >= 2) {
                // match: called as returning int; only then are the flags kept in $3 afterwards
                ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, unkE0->unk14, 0);
                ENTITY_FLAG_BITS(entity)->bit7 = FALSE;
                return;
            }
            entity->state = leader->state;
        }
    }
    if (entity->spawn_info->unk40 == 1) {
        for (other = D_004467E0; other != NULL; other = other->next) {
            if (other->params == entity->params && other != entity && ((s32)other->flags & 1)) {
                if (!ENTITY_FLAG_BITS(other)->bit1 || ENTITY_FLAG_BITS(other)->bits2 >= 3) {
                    func_0018DC40((char *)other);
                }
            }
        }
    }
    if (entity->unkC6 == 0) {
        switch (entity->state) {
        case 1:
        case 2:
            func_001435B0(entity, unknown);
            break;
        case 0:
        default:
            func_001958B8((char *)entity, unkE0->unk18, unknown);
            break;
        }
    }
    func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    } else {
        entity->state = 0;
    }
}
