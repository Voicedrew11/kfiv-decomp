#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_001957B8(float, float);
void func_001958B8(char *, void (*)(void), int);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_0012A778(Entity *);
void func_0012A8E0(Entity *);
void func_0012AA48(Entity *);

void func_0012A478(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;
    f32 angle;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
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
                func_001958B8((char *)entity, (void (*)(void))func_0012AA48, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            unkE0->unk7A = 0;
            return;
        case 1:
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.43633232f) {
                if (angle >= 0.0f) {
                    func_001958B8((char *)entity, (void (*)(void))func_0012A8E0, unknown);
                    return;
                }
                func_001958B8((char *)entity, (void (*)(void))func_0012A778, unknown);
                return;
            }
            unkE0->unk7A = 0;
            unkE0->unk78 = -1;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
