#include "kf_entity.h"

extern PlayerData D_00413F80;

int rand(void);
int func_00197078(void);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
s16 func_00191340(void *, int);
float func_001957B8(float, float);
void func_00216778(void *, void *);
void func_00194F48(f32 *, f32 *);
void func_001958B8(char *, void (*)(void), int);
void func_0012CCA0(Entity *);
void func_0012D040(Entity *);

void func_0012C1D0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 target[4];
    f32 direction[4];
    f32 delta; /* angle to the wanted heading, then height to the wanted level */
    s32 turn;
    s32 roll;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (unkE0->unk78 != 4 && entity->unkA2 == 0 && ((int (*)(Entity *))func_00197078)(entity)) { // match: the entity is passed
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_0012D040, unknown);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_0012CCA0, unknown);
        }
        unkE0->unk48 = 0.0f;
        unkE0->unk78 = 3;
        return;
    }
    if (unkE0->unk78 == 0) {
        if (rand() % 100 < 75) {
            roll = rand() % 60;
            unkE0->unk78 = 1;
            unkE0->unk7B = roll;
        } else {
            unkE0->unk4C = D_00413F80.position[1] - 220.0f - (f32)(rand() % 700);
            if (entity->radius + 100.0f + 200.0f < entity->distance_to_player) {
                unkE0->unk7A = 1;
            } else {
                unkE0->unk7A = 0;
            }
            entity->unkA0 = 120;
            unkE0->unk78 = 2;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 1:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - 190.0f + (f32)unkE0->unk7B - entity->model->position[1];
            if (delta > 10.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -10.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x40;
            }
            break;
        case 2:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = unkE0->unk4C - entity->model->position[1];
            if (delta > 30.0f) {
                entity->unkC8 |= 0x20;
            } else if (delta < -30.0f) {
                entity->unkC8 |= 0x10;
            }
            entity->unkC8 |= 1;
            if (unkE0->unk7A != 0) {
                entity->unkC8 |= 0x40;
            }
            func_00216778(target, &D_00413F80);
            target[1] = unkE0->unk4C;
            // match: the shared prototype has no result; it returns the distance
            if (((f32 (*)(f32 *, f32 *))func_00194F48)(target, entity->model->position) <= 200.0f) {
                unkE0->unk78 = 0;
            }
            break;
        case 3:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            if (entity->radius + 100.0f + 100.0f < entity->distance_to_player) {
                entity->unkC8 |= 0x41;
            }
            unkE0->unk68++;
            if (unkE0->unk68 > 90) {
                unkE0->unk68 = 0;
                unkE0->unk78 = 4;
            }
            break;
        case 4:
            delta = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(delta) > entity->params->turn_limit) {
                if (delta >= 0.0f) {
                    entity->unkC8 |= 0x200;
                } else {
                    entity->unkC8 |= 0x100;
                }
            }
            delta = D_00413F80.position[1] - entity->model->position[1];
            entity->unkC8 |= 0x51;
            if (delta > 700.0f) {
                unkE0->unk78 = 0;
            }
            break;
        default:
            unkE0->unk78 = 0;
            break;
        }
    }
    if (entity->unk54 != (void (*)(Entity *))unkE0->unk18) {
        func_001958B8((char *)entity, unkE0->unk18, unknown);
    }
}
