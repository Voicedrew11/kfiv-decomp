#include "kf_entity.h"

extern PlayerData D_00413F80;

int func_0019A448(void *, void *, int, int, int, int, int);
int func_0018EAC0(void *, void *);
int func_00172BB8(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00173198(Entity *);
void func_00172738(Entity *);

void func_00172F00(Entity *entity) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 turn;

    entity->unkC6 |= 0x3000;
    switch (entity->action_step) {
    case 0:
        func_0019A448(entity, &unkE0->unk80, 0, 3, 0, entity->params->unkC4, 0);
        func_0019A448(entity, &unkE0->unk88, 1, 3, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bit10 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        entity->target = &D_00413F80;
        unkE0->unk68 = 0;
        entity->action_step++;
    case 1:
        if (entity->distance_to_player < entity->radius + 250.0f + 100.0f) {
            f32 above = D_00413F80.position[1] - entity->model->position[1];

            if (100.0f < above && above < 400.0f) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                func_001958B8((char *)entity, (void (*)(void))func_00173198, 1);
                return;
            }
        }
        if (unkE0->unk68++ >= 0xD3) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
            return;
        }
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
        height = D_00413F80.position[1] - entity->model->position[1];
        if (height < 200.0f || ENTITY_FLAG_BITS(entity)->bit44 == 1 || ENTITY_FLAG_BITS(entity)->bit45 == 1) {
            unkE0->unk79 = 2;
        } else if (400.0f < height || ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            unkE0->unk79 = -2;
        } else {
            unkE0->unk79 = 0;
        }
        func_00172738(entity);
        break;
    }
}
