#include "kf_entity.h"

int func_00197078(void);
int func_0018CA50(int *);
int func_0018F030(char *);
float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);
void func_00181E48(Entity *);
void func_00182240(Entity *);

void func_0013EFB8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 angle;

    // match: func_00197078 takes the entity
    if (((int (*)(Entity *))func_00197078)(entity) && !func_0018CA50((int *)entity)) {
        if (func_0018F030((char *)entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00182240, 1);
        } else {
            func_001958B8((char *)entity, (void (*)(void))func_00181E48, 1);
        }
        return;
    }
    angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
    if (ABS(angle) > 0.5235988f) {
        unkE0->unk48 = entity->unk68;
        unkE0->unk79 = 1;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
