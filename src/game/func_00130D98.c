#include "kf_entity.h"

s32 func_001984B0(Entity *, s32);
void func_001958F8(Entity *, void (*)(void), int, int, int);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
int func_0018EAC0(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);

void func_00130D98(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];

    entity->unkC6 |= 0x1000;
    if (func_001984B0(entity, 0)) {
        func_001958F8(entity, (void (*)(void))func_00181E48, 1, 0, 0);
        return;
    }
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 4, 0, entity->params->unkC4 >> 1, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkA0 = entity->params->unk436;
        entity->action_step++;
        break;
    case 1:
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        entity->unk94 += entity->unk94;
        if (entity->unkA0 == 0 || func_0018EAC0(entity, direction) != 0) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
