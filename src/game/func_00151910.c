#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
void func_00195898(char *, u8);
void func_001958B8(char *, void (*)(void), int);

void func_00151910(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 duration;

    entity->unkC6 |= 0x1000;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 0, 0, 0, 1);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        entity->unkA0 = (u16)entity->params->unk436;
        unkE0->unk48[0] = entity->model->unkE0[0];
        unkE0->unk48[1] = entity->model->unkE0[1];
        unkE0->unk48[2] = entity->model->unkE0[2];
        unkE0->unk54 = entity->unk38;
        entity->model->unkE0[0] = 0.0f;
        entity->model->unkE0[2] = 0.0f;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit21 = 1;
        ENTITY_FLAG_BITS(entity)->bit18 = 1;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->state++;
        break;
    case 1:
        func_0018D600((char **)entity, 0x22F, 0xFF, (int)entity->model->unkD0);
        entity->state++;
        break;
    case 2:
        duration = entity->params->unk436;
        entity->model->unkE0[0] = (f32)(duration - entity->unkA0) / (f32)duration;
        entity->model->unkE0[2] = (f32)(duration - entity->unkA0) / (f32)duration;
        entity->unk38 = unkE0->unk54 * ((f32)(duration - entity->unkA0) / (f32)duration);
        if (entity->unkA0 == 0) {
            entity->model->unkE0[0] = unkE0->unk48[0];
            entity->model->unkE0[1] = unkE0->unk48[1];
            entity->model->unkE0[2] = unkE0->unk48[2];
            entity->unk38 = unkE0->unk54;
            func_00195898((char *)entity, 2);
            ENTITY_FLAG_BITS(entity)->bit21 = 0;
            ENTITY_FLAG_BITS(entity)->bit18 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                entity->unkE0.unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
