#include "kf_entity.h"

int func_0019A448(void *, void *, int, int, int, int, int);
void func_001958B8(char *, void (*)(void), int);
// match: the definition takes an Actor, a struct local to func_00182A08.c
void func_00182A08(void);
void func_00182BD0(char *);
void func_00183570(Entity *);
void func_0014F5A8(Entity *, s32);
void func_0014FA28(Entity *);

void func_0014F3F8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 |= 0x2000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, entity->unkE0.unk80, 0, 2, 0, entity->params->unkC4, 0);
        func_0019A448(entity, entity->unkE0.unk88, 1, 2, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = TRUE;
        entity->action_step++;
        break;
    case 1:
        if (((EntityUnkE0Slot *)unkE0->unk80)->unk01 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            unkE0->unk04 = func_00182A08;
            unkE0->unk08 = (void (*)(void))func_00182BD0;
            unkE0->unk18 = (void (*)(void))func_00183570;
            unkE0->unk14 = (void (*)(void))func_0014FA28;
            entity->unkE0.unk00 = func_0014F5A8;
            ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
