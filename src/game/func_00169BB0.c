#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
void func_001958B8(char *, void (*)(void), int);

void func_00169BB0(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;

    entity->unkC6 |= 0x10;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0xD, 0, 0xA, 1);
        if (entity->params->unk10 & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkA0 = entity->params->unkCE;
            entity->flags |= 0x8000L << 20;
            entity->flags &= ~0x4000;
            entity->state++;
        }
        break;
    case 2:
        if (ENTITY_FLAG_BITS(entity)->bit43 && ENTITY_FLAG_BITS(entity)->bit39 && !ENTITY_FLAG_BITS(entity)->bit38) {
            entity->flags &= ~(0x8000L << 28);
            entity->unkA0 = entity->params->unkCE;
        }
        func_0018D600((char **)entity, 0x32F, 0, (int)entity->model->unkD0);
        if (entity->unkA0 == 0) {
            ENTITY_FLAG_BITS(entity)->bit35 = 0;
            ENTITY_FLAG_BITS(entity)->bit14 = 1;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
