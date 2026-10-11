#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_00195AC0(Entity *, void *, s32, s32, s32);
void func_001958B8(char *, void (*)(void), int);
float func_00194FB8(float);
float sinf(float);
float cosf(float);

void func_00169500(Entity *entity) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    char *unk350 = entity->params->unk350;

    entity->unkC6 |= 0x5000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x1D, 0, 1, 1);
        entity->flags &= ~0x3800;
        entity->flags &= ~0x200;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->flags |= 0x20000;
            entity->action_step++;
        }
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        break;
    case 2:
        if (unkE0->unk6E == 0) {
            s32 flag = (s32)(entity->flags >> 49) & 1;

            if (flag == 1) {
                func_00195AC0(entity, unk350, 0, 0x1E, 0x1F);
                unkE0->unk6E = flag;
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk7B = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        entity->unk84 = -sinf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        entity->unk8C = -cosf(func_00194FB8(entity->model->unkD0[1])) * unkE0->unk44;
        unkE0->unk44 -= 2.0f;
        if (unkE0->unk44 < 0.0f) {
            unkE0->unk44 = 0.0f;
        }
        break;
    }
}
