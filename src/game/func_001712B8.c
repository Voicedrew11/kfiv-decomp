#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
s16 func_00190BE0(Entity *);
char *func_0010CAB8(char *, u32);
int func_00195E48(void *, int, void *, void *);
void func_0019A290(Entity *, s32);
void func_001958B8(char *, void (*)(void), int);
void func_00170AB8(Entity *);

void func_001712B8(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityParamsUnk3D8Array *unk3D8 = entity->params->unk3D8;
    f32 target[4]; // match: never used, but retail keeps 16 bytes of stack for it
    char *point;

    entity->unkC6 |= 1;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x17, 0, entity->params->unkC4 >> 1, 1);
        if (entity->params->flags & 0x100) {
            ENTITY_FLAG_BITS(entity)->bits11 = 0;
        } else {
            ENTITY_FLAG_BITS(entity)->bits11 = ((((EntityParamsWord *)entity->params)->unk00 & 0x06000000) == 0x02000000) ? 2 : 1;
        }
        ENTITY_FLAG_BITS(entity)->bit9 = 0;
        entity->action_step++;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        if (unk3D8->record.unk01 != 0) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        break;
    case 2:
        if (unk3D8->record.unk01 >= entity->model->unkFC->unk82) {
            entity->unkC8 |= func_00190BE0(entity);
        }
        if (((s32)(entity->flags >> 58) & 3) == 0 && entity->model->unkFC->unk82 == 0x1E &&
            (entity->unk2A == 0 || unk3D8->unk0C != 0)) {
            point = func_0010CAB8((char *)entity->model, unk3D8->unk11);
            if (point != NULL) {
                func_00195E48(entity, unk3D8->unk10, point, entity->model->unkD0);
            }
        }
        if (entity->model->unkFC->unk82 > 0 && unk3D8->record.unk03 >= entity->model->unkFC->unk82) {
            entity->flags |= 0x80000;
        } else {
            entity->flags &= ~0x80000;
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            func_0019A290(entity, unk3D8->record.unk02);
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
            return;
        }
        break;
    }
    func_00170AB8(entity);
}
