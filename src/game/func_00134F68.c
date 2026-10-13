#include "kf_entity.h"

extern float D_00413F80[];

int func_0018EAC0(void *, void *);
s16 func_00191340(void *, int);
s16 func_00190D50(float, void *, int);
void func_00216778(void *, void *);
s32 func_00196D70(Entity *, void *, f32, f32, f32);
int func_0018F030(char *);
float func_00194FB8(float);
void func_00195588(f32 *, f32);
void Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00134398(Entity *);
void func_00134750(Entity *);

void func_00134F68(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        switch (unkE0->unk78) {
        case 0: {
            f32 *point = unkE0->unk30;

            func_00216778(point, D_00413F80);
            unkE0->unk30[1] -= 220.0f;
            entity->target = point;
            entity->unkC8 = func_00190D50(entity->radius + 100.0f + 30.0f, entity, 0);
            if (entity->distance_to_player < (f32)ENTITY_CONTROLLER(entity)->unk434) {
                unkE0->unk78++;
            }
            break;
        }
        case 1: {
            f32 *point = unkE0->unk30;

            func_00216778(point, D_00413F80);
            unkE0->unk30[1] -= 190.0f;
            entity->target = point;
            if (func_00196D70(entity, point, entity->params->unk43A * 0.017453292f,
                              entity->params->unk43C * 0.017453292f, entity->params->unk43E)) {
                if (func_0018F030((char *)entity)) {
                    func_001958B8((char *)entity, (void (*)(void))func_00134750, unknown);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_00134398, unknown);
                }
                unkE0->unk68 = entity->params->unk436;
                unkE0->unk78++;
                return;
            }
            entity->unkC8 = func_00190D50(entity->radius + 100.0f + 30.0f, entity, 0);
            break;
        }
        case 2: {
            f32 *point = unkE0->unk30;

            direction[0] = func_00194FB8(entity->unk68 + 3.1415927f);
            func_00195588(point, direction[0]);
            Vector_Scale(point, point, 800.0f);
            func_00216C48(point, point, D_00413F80);
            entity->target = point;
            unkE0->unk30[1] = entity->spawn_info->spawn_y;
            entity->unkC8 = func_00190D50(50.0f, entity, 0);
            if (entity->distance_to_player >= (f32)ENTITY_CONTROLLER(entity)->unk434 || unkE0->unk68 <= 0) {
                unkE0->unk78 = 0;
            }
            break;
        }
        }
    }
    if (unkE0->unk68 > 0) {
        unkE0->unk68--;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
