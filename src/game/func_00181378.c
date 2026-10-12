#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0019A310(char *, void (*)(void), int);

void func_00181378(Entity *entity) {
    EntityParams *params;
    s32 frame;
    s32 kind;

    entity->unkC6 |= 2;
    switch (entity->unkCB) {
    case 0:
        frame = entity->model->unkFC->unk82 - 3;
        if (frame < 0) {
            frame = 0;
        }
        func_00192C98(entity, entity->unkD1, frame, 5, 1);
        params = entity->params;
        if (params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            kind = 1;
            if ((((EntityParamsWord *)params)->unk00 & 0x6000000) == 0x2000000) {
                kind = 2;
            }
            ENTITY_FLAG_BITS(entity)->bits11 = kind;
        }
        ENTITY_FLAG_BITS(entity)->bit10 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit9 = FALSE;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkC6 &= ~2;
            func_0019A310((char *)entity, NULL, 0);
        }
        break;
    }
}
