#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
s32 func_00196D70(Entity *, void *, f32, f32, f32);
s32 func_0018F9C0(Entity *);
void func_0018B6F8(f32 *, f32, f32, void *, void *);
int func_0018EAC0(void *, void *);
s16 func_00191340(void *, int);
s16 func_00190D50(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00134750(Entity *);

void func_00134DC8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 *point;
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (func_00196D70(entity, D_00413F80, entity->params->unk43A * 0.017453292f,
                      entity->params->unk43C * 0.017453292f, entity->params->unk43E) &&
        unkE0->unk6A <= 0) {
        func_0018F9C0(entity);
        // match: an int result keeps $2 busy after the call
        ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00134750, unknown);
        unkE0->unk6A = entity->params->unk438;
        return;
    }
    if (unkE0->unk6A > 0) {
        unkE0->unk6A--;
    }
    if (entity->unkA0 == 0) {
        point = unkE0->unk30;
        func_0018B6F8(point, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info,
                      entity->model->position);
        entity->target = point;
        entity->unkA0 = rand() % 120 + 240;
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        entity->unkC8 = func_00190D50(50.0f, entity, 0);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
