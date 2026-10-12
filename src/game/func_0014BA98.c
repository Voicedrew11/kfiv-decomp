#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
void func_0018B778(float, void *, void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_0014BF80(Entity *);

void func_0014BA98(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    side = func_0018FD50(entity, 0);
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (func_0018F030((char *)entity)) {
                if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_0014BF80, unknown);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
                }
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            return;
        case 1:
            func_0018B778(entity->spawn_info->unk4C, unkE0->unk30, entity->spawn_info, entity->model->position);
            entity->target = unkE0->unk30;
            entity->unkA0 = rand() % 60 + 210;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 60 + 90;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        entity->unkC8 = func_00190F90(50.0f, entity, 0);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
