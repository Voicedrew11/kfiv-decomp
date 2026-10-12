#include "kf_entity.h"

int rand(void);
void func_0018B6F8(f32 *, f32, f32, void *, void *);
int func_0018EAC0(void *, void *);
s16 func_00190D50(float, void *, int);
int func_00191340(Entity *, int);
void func_001958B8(char *, void (*)(void), int); // match: this file only matches with this type; func_001958B8.c defines it differently

void func_0012C088(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (entity->unkA0 == 0) {
        if (rand() % 100 > 20) {
            func_0018B6F8(unkE0->unk30, entity->spawn_info->unk4C, entity->spawn_info->unk56, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 120;
        } else {
            entity->target = NULL;
            entity->unkA0 = rand() % 30 + 30;
        }
    }
    turn = func_0018EAC0(entity, direction);
    if (turn != 0) {
        entity->unkC8 = func_00191340(entity, turn);
    } else {
        entity->unkC8 = func_00190D50(50.0f, entity, 1);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
