#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

int rand(void);
s32 func_0018FD50(Entity *, s32);
f32 func_0018A9F0(void *, void *);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
int func_00172BB8(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00172F00(Entity *);
void func_00173198(Entity *);
void func_001735C8(Entity *);

void func_00171E30(Entity *entity, s32 unknown) {
    EntityUnkE0Kind1740A8 *unkE0 = (EntityUnkE0Kind1740A8 *)&entity->unkE0;
    f32 direction[4];
    f32 height;
    s32 side;
    s32 turn;

    if (entity->params->flags & 0x100) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
    } else {
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
    }
    if (entity->unkA0 == 0) {
        if (rand() % 100 < entity->params->unk438) {
            if (!(rand() & 1)) {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = 1;
            } else {
                entity->target = &D_00413F80;
                entity->unkA0 = rand() % 30 + 0x1E;
                unkE0->unk78 = 0;
                entity->unkCD = 1;
                unkE0->unk79 = -1;
            }
        } else if (entity->unkA0 == 0) {
            unkE0->unk79 = 0;
        }
    }
    side = func_0018FD50(entity, 0);
    if (entity->spawn_info->unk40 == 0) {
        if (func_0018A9F0(entity->unk14, entity->model->position) < (f32)entity->params->unk436 ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (func_0018A9F0(entity->unk14, entity->model->position) > (f32)(s16)entity->params->unk434 ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    } else if (entity->spawn_info->unk40 == 1 || entity->spawn_info->unk40 == 2) {
        if (entity->spawn_info->spawn_y - entity->model->position[1] <
                (f32)entity->spawn_info->unk42 * (entity->spawn_info->unk43 != 0 ? 100.0f : -100.0f) ||
            ENTITY_FLAG_BITS(entity)->bit44 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = 2;
        } else if (entity->spawn_info->spawn_y - entity->model->position[1] > (f32)entity->spawn_info->variant * 100.0f ||
                   ENTITY_FLAG_BITS(entity)->bit46 == 1) {
            side = 1;
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x1E;
            unkE0->unk78 = 0;
            entity->unkCD = 1;
            unkE0->unk79 = -2;
        }
    }
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
            if (func_0018F030((char *)entity)) {
                if (D_00413F80.position[1] - entity->model->position[1] < 200.0f) {
                    entity->target = &D_00413F80;
                    entity->unkA0 = rand() % 30 + 0x1E;
                    unkE0->unk78 = 0;
                    entity->unkCD = 1;
                    unkE0->unk79 = 2;
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_001735C8, unknown);
                    return;
                }
            } else {
                height = D_00413F80.position[1] - entity->model->position[1];
                if (100.0f < height && height < 400.0f) {
                    func_001958B8((char *)entity, (void (*)(void))func_00173198, unknown);
                } else {
                    func_001958B8((char *)entity, (void (*)(void))func_00172F00, unknown);
                }
                return;
            }
            break;
        case 1:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        default:
            entity->target = &D_00413F80;
            entity->unkA0 = rand() % 30 + 0x3C;
            unkE0->unk78 = 0;
            break;
        }
    }
    if (unkE0->unk78 == 0) {
        turn = func_0018EAC0(entity, direction);
        if (turn > 0) {
            entity->unkC8 = 0x201;
        } else if (turn < 0) {
            entity->unkC8 = 0x101;
        } else {
            // match: retail passes two more arguments here and takes the result as 16 bits
            entity->unkC8 = ((s16 (*)(Entity *, s32, f32))func_00172BB8)(entity, 0, 0.0f);
        }
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
