#include "kf_entity.h"

extern float D_00413F80[];

int rand(void);
int func_00192C98(void *, int, int, u32, int);
int func_00199EB8(void *, void *, float);
float func_001957B8(float, float);
float func_00195478(float *, char *);
int func_0018F030(char *);
void func_001958B8(char *, void (*)(void), int);
void func_00165A70(Entity *);

void func_00165E68(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 side;

    entity->unkC6 |= 0x1000;
    switch (entity->state) {
    case 0:
        side = func_001957B8(D_00413F80[5], func_00195478(D_00413F80, (char *)entity->model->position));
        if (unkE0->unk7B == 1) {
            if (side > 0.0f) {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else if (side < 0.0f) {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            }
        } else {
            if ((f32)(rand() % 120) > side * 57.29578f + 60.0f) {
                func_00192C98(entity, 7, 0, entity->params->unkC4 >> 1, 1);
            } else {
                func_00192C98(entity, 6, 0, entity->params->unkC4 >> 1, 1);
            }
        }
        entity->flags = (entity->flags & ~0x3800) | 0x800;
        entity->flags &= ~0x200;
        entity->state++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        func_00199EB8(entity, &unkE0->unk1C, entity->unk68);
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (unkE0->unk7B == 0) {
                if ((s32)(entity->flags >> 5) & 1) {
                    entity->unkE0.unk00(entity, 1);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
                }
            } else {
                func_0018F030((char *)entity);
                func_001958B8((char *)entity, (void (*)(void))func_00165A70, 1);
            }
        }
        break;
    }
}
