#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
s32 func_0015A790(Entity *, f32, f32);
s32 func_0015A8F8(Entity *, f32, f32);
float func_00194FB8(float);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(void);
void func_0015A3D8(Entity *);
void func_0015A538(Entity *);
void func_0015B620(Entity *);

void func_00159D28(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s8 turn;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk6A = entity->params->unk438 / 2 + rand() % (entity->params->unk438 / 2);
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 1;
        if (func_0015A790(entity, 200.0f, 1.0471976f) || func_0015A8F8(entity, 300.0f, 0.5235988f)) {
            unkE0->unk7A = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_001958B8((char *)entity, (void (*)(void))func_0015A538, 1);
            break;
        }
        if (entity->distance_to_player < (f32)entity->params->unk436 + 100.0f + entity->radius &&
            ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
            unkE0->unk7A = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            ENTITY_FLAG_BITS(entity)->bits58 = 1;
            func_001958B8((char *)entity, func_00181E48, 1);
            break;
        }
        if (entity->unkA0 == 0) {
            if (unkE0->unk6A == 0 && ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) < 0.34906584f) {
                unkE0->unk7A = 1;
                unkE0->unk6A = entity->params->unk438 / 2 + rand() % (entity->params->unk438 / 2);
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                func_001958B8((char *)entity, (void (*)(void))func_0015B620, 1);
                break;
            }
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 1.5707964f) {
                entity->unkC6 = 0;
                entity->unkC8 = 0;
                entity->unkA0 = 0;
                unkE0->unk7B = func_00194FB8(entity->unk68 - entity->model->unkD0[1]) > 0.0f ? 1 : -1;
                func_001958B8((char *)entity, (void (*)(void))func_0015A3D8, 1);
                break;
            }
            if (ABS(func_00194FB8(entity->unk68 - entity->model->unkD0[1])) > 0.34906584f) {
                entity->target = D_00413F80;
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = 0;
            } else {
                entity->unkA0 = rand() % 30 + 30;
                unkE0->unk78 = -1;
            }
        }
        turn = func_0018EAC0(entity, direction);
        switch (unkE0->unk78) {
        case 0:
            func_00192C98(entity, 4, 0, entity->params->unkC4, 0);
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 1);
            }
            if (entity->unkC8 & 0x100) {
                unkE0->unk1C -= entity->params->unk58;
                if (unkE0->unk1C < -entity->params->turn_limit) {
                    unkE0->unk1C = -entity->params->turn_limit;
                }
            } else if (entity->unkC8 & 0x200) {
                unkE0->unk1C += entity->params->unk58;
                if (entity->params->turn_limit < unkE0->unk1C) {
                    unkE0->unk1C = entity->params->turn_limit;
                }
            } else {
                unkE0->unk1C = 0.0f;
            }
            direction[0] = unkE0->unk1C;
            if (entity->unkC8 & 0x400) {
                direction[0] += direction[0];
            }
            entity->unk94 += direction[0];
            unkE0->unk44 = 0.0f;
            break;
        case -1:
            func_00192C98(entity, 0, 0, entity->params->unkC4, 0);
            unkE0->unk1C = 0.0f;
            unkE0->unk44 = 0.0f;
            break;
        }
        break;
    }
}
