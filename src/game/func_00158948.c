#include "kf_entity.h"

extern float D_00413F80[];
extern f32 D_00446818[];

int rand(void);
int func_00192C98(void *, int, int, u32, int);
void func_0018D600(char **, int, u32, int);
float func_00194F80(void *, void *);
void func_0018BA10(f32 *, f32 *, f32, f32);
f32 func_0018A9F0(void *, void *);
float func_00195478(float *, char *);
float func_00194FB8(float);
void func_00216778(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_00158C50(Entity *);

void func_00158948(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 *destination;
    f32 height;
    f32 yaw;

    entity->unkC6 |= 0x2000;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 1, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        ENTITY_FLAG_BITS(entity)->bits36 = 0;
        unkE0->unk68 = 1;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAG_BITS(entity)->bit10 = 0;
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            ENTITY_FLAG_BITS(entity)->bit20 = 1;
            ENTITY_FLAG_BITS(entity)->bit18 = 1;
            ENTITY_FLAG_BITS(entity)->bit21 = 1;
            ENTITY_FLAG_BITS(entity)->bit27 = 1;
            unkE0->unk7C = 0;
            unkE0->unk6E = 0;
            func_0018D600((char **)entity, 0x234, 0, (int)entity->model->unkD0);
            entity->state++;
        }
        break;
    case 2:
        if (unkE0->unk7C == 0) {
            destination = unkE0->unk30;
            func_0018BA10(destination, (f32 *)entity->spawn_info, func_00194F80(entity->model->position, entity->spawn_info), entity->spawn_info->unk4C);
            height = func_0018A9F0(entity->unk14, destination);
            if (height != D_00446818[0]) {
                yaw = func_00195478(D_00413F80, (char *)destination);
                if (ABS(func_00194FB8(yaw - D_00413F80[5])) > 0.7853982f) {
                    unkE0->unk30[1] += height - (f32)(rand() % 200);
                    unkE0->unk7C = 1;
                }
            }
        }
        entity->unkD6 -= (unkE0->unk7B != 0) ? 2 : 4;
        if (entity->unkD6 > 0x80) {
            entity->unkD6 = 0;
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (unkE0->unk7C == 1) {
                func_00216778(entity->model->position, unkE0->unk30);
            }
            func_001958B8((char *)entity, (void (*)(void))func_00158C50, 1);
        }
        break;
    }
}
