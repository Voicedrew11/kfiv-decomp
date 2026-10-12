#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
float func_00194FB8(float);
float func_00195478(float *, char *);
float func_001957B8(float, float);
int func_00190BE0(Entity *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int); // match: this file only matches with this type; func_00190F90.c defines it differently
int func_001958B8(Entity *, void (*)(Entity *), int);
void func_00181AE0(Entity *);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00182240(Entity *);
void func_00196D48(Entity *);

/* chase: face the player, then close in, or circle left or right around the player for a while */
int func_00127E08(void *entity_ptr, int unknown) {
    Entity *entity = entity_ptr;
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 heading;
    f32 angle;
    s32 side;
    s32 turn;
    s32 mode;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181AE0, 1);
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
                func_001958B8((char *)entity, (void (*)(void))func_00182240, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
            }
            return;
        case 1:
            angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
            if (ABS(angle) > 0.17453292f) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 60 + 60;
                unkE0->unk78 = 0;
            } else {
                heading = func_00195478(D_00413F80.position, (char *)entity->model->position) - D_00413F80.unk14;
                heading = func_00194FB8(heading);
                if (ABS(heading) > 0.7853982f || rand() % 100 < entity->params->unk430) {
                    entity->target = &D_00413F80;
                    entity->unkA0 = rand() % 60 + 120;
                    unkE0->unk78 = 1;
                } else {
                    if (heading >= 0.0f) {
                        unkE0->unk7A = 0;
                    } else {
                        unkE0->unk7A = 1;
                    }
                    entity->unkA0 = rand() % 30 + 60;
                    unkE0->unk78 = 2;
                }
            }
            break;
        case 2:
            entity->unkA0 = 30;
            unkE0->unk78 = -1;
            break;
        default:
            unkE0->unk78 = -1;
            break;
        }
    }
    mode = entity->unkCD;
    if (mode == 1) {
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
        } else {
            turn = func_0018EAC0(entity, &heading);
            switch (unkE0->unk78) {
            case 0:
                angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                if (ABS(angle) <= 0.17453292f) {
                    entity->unkA0 = 0;
                }
                ENTITY_FLAG_BITS(entity)->bits11 = 3;
                break;
            case 1:
                if (turn > 0) {
                    entity->unkC8 = 0x201;
                } else if (turn < 0) {
                    entity->unkC8 = 0x101;
                } else {
                    entity->unkC8 = func_00190F90(entity->radius + 100.0f + 30.0f, entity, 0);
                }
                break;
            case 2:
                angle = func_001957B8(entity->model->unkD0[1], entity->unk68);
                if (ABS(angle) > entity->params->turn_limit) {
                    if (angle >= 0.0f) {
                        entity->unkC8 |= 0x200;
                    } else {
                        entity->unkC8 |= 0x100;
                    }
                }
                if (entity->flags & (0xD000L << 33)) {
                    unkE0->unk78 = 1;
                } else if (unkE0->unk7A != 0) {
                    entity->unkC8 |= 8;
                } else {
                    entity->unkC8 |= 4;
                }
                break;
            }
        }
    }
done:
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
