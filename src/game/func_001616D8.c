#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
void func_0019A290(Entity *, s32);
int func_0019A310(char *, void (*)(void), int); // match: an int result keeps $2 busy after the call
void func_001958B8(char *, void (*)(void), int);

void func_001616D8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    entity->unkC6 = 4;
    switch (entity->unkCB) {
    case 0:
        func_00192C98(entity, 0xF, 0, 5, 1);
        entity->flags &= ~0x3800;
        entity->flags |= 0x4000;
        entity->flags &= ~0x80000;
        entity->flags &= ~0x800000000L;
        entity->flags &= ~0x200;
        entity->unkCB++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->unkCB++;
        }
        break;
    case 2:
        if ((s32)(entity->flags >> 34) & 1) {
            func_0019A290(entity, entity->params->unkC8);
            func_0019A310((char *)entity, NULL, 0);
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        break;
    }
}
