#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(void);
void func_00181C60(void);
void func_00174DF0(Entity *);

void func_00174B68(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    f32 range;
    f32 angle;
    s32 next_action;
    s32 side;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    range = 300.0f;
    angle = 0.5235988f;
    if (func_0018ED98(entity, range, angle)) {
        func_001958B8((char *)entity, func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, range, angle)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    next_action = func_0018FD50(entity, 0);
    ENTITY_FLAG_BITS(entity)->bit9 = TRUE;
    if (entity->unkA0 == 0 || next_action == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = next_action;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        // match: called with one argument here
        ((void (*)(Entity *))func_00190B88)(entity);
        switch (entity->unkCD) {
        case 0:
            if (unkE0->unk68 == 0) {
                unkE0->unk68 = (u16)entity->params->unk430;
                func_001958B8((char *)entity, func_00174DF0, unknown);
                return;
            }
            unkE0->unk78 = -1;
            break;
        case 1:
            entity->target = D_00413F80;
            entity->unkA0 = rand() % 60 + 120;
            unkE0->unk78 = 0;
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
    if (entity->unkCD == 1) {
        side = func_0018EAC0(entity, direction);
        if (unkE0->unk78 == 0) {
            if (side > 0) {
                entity->unkC8 = 0x201;
            } else if (side < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
            }
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
