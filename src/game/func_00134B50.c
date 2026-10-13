#include "kf_entity.h"

int func_0018EAC0(void *, void *);
s16 func_00190D50(float, void *, int);
void func_00216D20(void *, void *);
void func_00216CD8(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
void func_001958B8(char *, void (*)(void), int);

/* walk back to the spawn point, turning away from whatever is pushing the entity */
void func_00134B50(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 to_spawn[4];
    f32 push[4];
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    entity->unkA0 = 0;
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    turn = func_0018EAC0(entity, direction);
    if (turn == 0) {
        entity->target = entity->spawn_info;
        entity->unkC8 = func_00190D50(50.0f, entity, 0);
    } else {
        push[0] = push[1] = push[2] = push[3] = 0.0f;
        if (entity->flags & (0xC000L << 31)) {
            push[0] += entity->unkA4[0];
            push[1] += entity->unkA4[1];
            push[2] += entity->unkA4[2];
        }
        if ((s32)(entity->flags >> 48) & 1) {
            push[0] += entity->unkB0[0];
            push[1] += entity->unkB0[1];
            push[2] += entity->unkB0[2];
        }
        func_00216D20(push, push);
        Vector_Subtract(to_spawn, entity->spawn_info, entity->model->position);
        func_00216D20(to_spawn, to_spawn);
        func_00216CD8(to_spawn, push, to_spawn);
        if (to_spawn[1] <= 0.0f) {
            if (turn == -1) {
                turn = 1;
            }
        } else if (turn == 1) {
            turn = -1;
        }
        entity->target = entity->spawn_info;
        if (turn > 0) {
            entity->unkC8 = 0x201;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x221;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x211;
            }
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
            if (push[1] > 0.0f) {
                entity->unkC8 = 0x121;
            } else if (push[1] < 0.0f) {
                entity->unkC8 = 0x111;
            }
        } else {
            entity->unkC8 = func_00190D50(50.0f, entity, 0);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
