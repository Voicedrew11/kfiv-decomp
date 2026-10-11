#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_001A32A8(Entity *);

void func_0016FE10(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 0x40;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 1);
        ENTITY_FLAG_BITS(entity)->bit22 = 0;
        ENTITY_FLAG_BITS(entity)->bit20 = 1;
        ENTITY_FLAG_BITS(entity)->bit27 = 1;
        ENTITY_FLAG_BITS(entity)->bit26 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 1;
        ENTITY_FLAG_BITS(entity)->bits2 = 2;
        ENTITY_FLAG_BITS(entity)->bit4 = 1;
        entity->unkA0 = 20;
        unkE0->unk68 = 0;
        entity->state++;
    case 1:
        unkE0->unk48[0] += (f32)entity->params->unk430 * ((f32)unkE0->unk68 / 30.0f);
        if ((f32)entity->params->unk432 < unkE0->unk48[0]) {
            unkE0->unk48[0] = (f32)entity->params->unk432;
        }
        if (entity->unkA0 != 0) {
            entity->model->unk15C = (f32)entity->unkD6 * (f32)entity->unkA0 / 20.0f;
        } else {
            if (entity->params->unkC6 != 0 && entity->params->unkC6 != 0xFFFF) {
                func_001A32A8(entity);
            }
            ENTITY_FLAG_BITS(entity)->bits2 = 3;
            ENTITY_FLAG_BITS(entity)->bit1 = 0;
        }
        break;
    }
    entity->unk88 += unkE0->unk48[0] + unkE0->unk48[0];
    unkE0->unk68++;
}
