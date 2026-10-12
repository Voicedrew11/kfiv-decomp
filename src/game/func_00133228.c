#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
float func_001957B8(float, float);
float func_00195478(float *, char *);
s32 func_001984B0(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_001325B8(Entity *);

void func_00133228(Entity *entity) {
    EntityUnkE0Kind131390 *unkE0 = (EntityUnkE0Kind131390 *)&entity->unkE0;
    PlayerData *player;

    entity->unkC6 |= 0x1000;
    unkE0->unk7E = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
    switch (entity->action_step) {
    case 0:
        func_001957B8(entity->model->unkD0[1], entity->unk68);
        player = &D_00413F80;
        if (func_001957B8(player->unk14, func_00195478(player->position, (char *)entity->model->position)) >= 0.0f) {
            func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 0);
        } else {
            func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 0);
        }
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (func_001984B0(entity, 0)) {
                func_001958B8((char *)entity, (void (*)(void))func_001325B8, 1);
            } else if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
