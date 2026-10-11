#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
s32 func_0017EA98(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_0017EEB0(Entity *);

void func_0017DF10(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 next_action = 0; // match: the unused initial value changes the instruction count that sizes a compiler hash table, and with it two registers
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->unk5C = (s32)D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->unk5C = (s32)D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    next_action = func_0018FD50(entity, 0);
    if (D_00413F80[1] - entity->model->position[1] < entity->params->unk436) {
        entity->unk5C = (s32)D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = 2;
    }
    if (D_00413F80[1] - entity->model->position[1] > (s16)entity->params->unk434) { // match: read signed here
        entity->unk5C = (s32)D_00413F80;
        entity->unkA0 = rand() % 30 + 30;
        unkE0->unk78 = 0;
        next_action = 1;
        entity->unkCD = next_action;
        unkE0->unk79 = -2;
    }
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))func_00190B88)(entity);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                func_001958B8((char *)entity, (void (*)(void))func_0017EEB0, unknown);
            }
            return;
        case 1:
            entity->unk5C = (s32)D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
            break;
        default:
            entity->unk5C = (s32)D_00413F80;
            entity->unkA0 = rand() % 30 + 60;
            unkE0->unk78 = 0;
            break;
        }
    }
    if (entity->unkCD == 1) {
        side = func_0018EAC0(entity, direction);
        if (unkE0->unk78 == 0) {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                // match: called like func_00190F90, with a speed and a mode the callee ignores
                entity->unkC8 = ((s32 (*)(f32, Entity *, s32))func_0017EA98)(0.0f, entity, 0);
            }
        }
    }
    func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
}
