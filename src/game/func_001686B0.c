#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_00169DF8(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
int func_0016A450(char *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018CA50(int *);
void func_00196D48(Entity *);
float func_001957B8(float, float);
s16 func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(void);
void func_00181E48(void);
void func_00168DB8(Entity *);
void func_00169500(Entity *);
void func_00169738(Entity *);
void func_00169BB0(Entity *);
void func_00169F58(Entity *);
void func_0016A530(Entity *);

void func_001686B0(Entity *entity, s32 unknown) {
    EntityUnkE0Kind168160 *unkE0 = (EntityUnkE0Kind168160 *)&entity->unkE0;
    f32 direction[4];
    f32 angle;
    f32 turn_left;
    s32 side;
    s32 turn;

    if (entity->params->unk10 & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_00169DF8(entity, 200.0f, 1.0471976f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00169BB0, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, func_00181AE0, 1);
        return;
    }
    if (entity->spawn_info->unk40 == 1) {
        func_0016A450((char *)entity);
    } else {
        entity->params->unk410 = 0;
    }
    // match: angle is read here before anything has set it
    if (unkE0->unk7B == 0 && entity->distance_to_player < entity->unk38 + 50.0f + 100.0f && entity->unkA0 == 0 &&
        rand() % 100 < (s16)entity->params->unk434 && ABS(angle) < 1.0471976f) {
        turn_left = entity->unk68 - entity->model->unkD0[1];
        if (ABS(turn_left) < 1.0471976f) {
            func_001958B8((char *)entity, (void (*)(void))func_00169738, unknown);
            return;
        }
    }
    side = func_0018FD50(entity, 2);
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA0 == 0 || side == 0) {
        entity->unkCF = entity->unkCD;
        entity->unkCD = side;
        if (entity->unkCD != entity->unkCF) {
            entity->unk9E = D_00446ED4;
        }
        func_00190B88(entity, side);
        switch (entity->unkCD) {
        case 0:
            if (unkE0->unk7B == 1) {
                func_001958B8((char *)entity, (void (*)(void))func_00169500, unknown);
            } else if (func_0018F030((char *)entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 2) {
                    func_001958B8((char *)entity, (void (*)(void))func_00169F58, unknown);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_0016A530, unknown);
                }
            } else {
                func_001958B8((char *)entity, func_00181E48, unknown);
            }
            return;
        case 1:
            if (unkE0->unk7A == 1 && unkE0->unk7B == 0 && func_0018CA50((int *)entity) == 0) {
                entity->unk5C = (s32)&D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                func_001958B8((char *)entity, (void (*)(void))func_00168DB8, unknown);
                return;
            }
            entity->unk5C = (s32)&D_00413F80;
            entity->unkA0 = rand() % 60 + 0x78;
            unkE0->unk78 = 0;
            break;
        case 2:
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    if (entity->unkCD == 1) {
        turn = func_0018EAC0(entity, direction);
        func_00196D48(entity);
        if (ENTITY_FLAG_BITS(entity)->bit31) {
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (unkE0->unk79 == 0) {
                unkE0->unk79 = ABS(angle) > 0.5235988f;
                if (unkE0->unk79 == 0) {
                    goto done;
                }
            }
            entity->unkC8 = func_00190BE0(entity);
            if (entity->unkC8 == 0) {
                unkE0->unk79 = 0;
            }
        } else if (unkE0->unk7A == 1) {
            if (func_0018CA50((int *)entity) == 0 && unkE0->unk7B == 0) {
                entity->unk5C = (s32)&D_00413F80;
                entity->unkA0 = rand() % 60 + 0xF0;
                unkE0->unk78 = 0;
                func_001958B8((char *)entity, (void (*)(void))func_00168DB8, unknown);
                return;
            }
            if (turn > 0) {
                entity->unkC8 = 0x241;
            } else if (turn < 0) {
                entity->unkC8 = 0x141;
            } else {
                entity->unkC8 = func_00190F90(entity->unk38 + 100.0f + 30.0f, entity, 1);
            }
        } else {
            if (turn > 0) {
                entity->unkC8 = 0x201;
            } else if (turn < 0) {
                entity->unkC8 = 0x101;
            } else {
                entity->unkC8 = func_00190F90(entity->unk38 + 100.0f + 30.0f, entity, 0);
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
