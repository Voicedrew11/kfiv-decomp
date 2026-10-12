#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00141C38(Entity *);

void func_00141AF0(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
    ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
    if (entity->unkA0 == 0) {
        if (rand() % 100 < ENTITY_CONTROLLER(entity)->unk434) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            entity->target = NULL;
            func_001958B8((char *)entity, (void (*)(void))func_00141C38, unknown);
            unkE0->unk7A = 0;
            return;
        }
        entity->target = D_00413F80;
    }
    turn = func_0018EAC0(entity, direction);
    if (turn > 0) {
        entity->unkC8 = 0x201;
    } else if (turn < 0) {
        entity->unkC8 = 0x101;
    } else {
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
