#include "kf_entity.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;
extern u8 D_0034A018[];
// match: the size is unknown; a complete small type would be read through $gp
extern f32 D_0044681C[];

int rand(void);
void func_00196870(void *, int);
void func_00162B48(Entity *);
void func_00196CC0(void *);
void func_0018E9C0(void *);
s16 func_00190F90(float, void *, int);
int func_00182DE0(void *, int);
int func_00182FD8(void *, int);
void func_00183150(Entity *, s32);
void func_001601B0(Entity *, s32);
void func_00160630(Entity *, s32);
void func_00160B30(Entity *, s32);
f32 func_0018A9F0(void *, void *);
void func_0018DA38(Entity *, s32, s32, s32);
void func_001958B8(char *, void (*)(void), int);

void func_0015FD38(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    EntityUnkE0Kind15F8C0 *timers = (EntityUnkE0Kind15F8C0 *)unkE0;
    f32 height;

    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 1);
    }
    if (D_0034A018[0] == 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        ENTITY_FLAGS(entity) |= 0x40000;
        return;
    }
    ENTITY_FLAGS(entity) &= ~0x40000;
    if (timers->unk6C > 0) {
        timers->unk6C--;
    }
    if (D_00413F80.unk259 != 0) {
        timers->unk6C = entity->params->unk43A;
    }
    func_00162B48(entity);
    if (entity->unkC6 == 0) {
        func_00196CC0(entity);
        func_0018E9C0(entity);
        if (D_00413F80.position[0] >= 6600.0f) {
            ENTITY_FLAGS(entity) |= 0x40000;
            ENTITY_FLAGS(entity) &= ~0x3800;
            entity->unk5C = (s32)entity->spawn_info;
            entity->unkC8 = func_00190F90(500.0f, entity, 0) | 0x440;
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
        } else if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            func_00182DE0(entity, unknown);
        } else {
            switch (entity->unkCC) {
            case 0:
                func_00182FD8(entity, unknown);
                break;
            case 1:
                func_00183150(entity, unknown);
                break;
            case 2:
                if (timers->unk7C == 0) {
                    func_001601B0(entity, unknown);
                } else if (timers->unk7C == 1) {
                    func_00160630(entity, unknown);
                } else {
                    func_00160B30(entity, unknown);
                }
                break;
            default:
                func_00182FD8(entity, unknown);
                break;
            }
        }
    }
    if (((s32)(ENTITY_FLAGS(entity) >> 28) & 1) && !(entity->unkC6 & 0x1000)) {
        height = func_0018A9F0(entity->unk14, entity->model->position);
        if (height != D_0044681C[0]) {
            if (height > 45.0f) {
                timers->unk7A = 1;
            } else if (height < 35.0f) {
                timers->unk7A = 0;
                timers->unk7B = 1;
            }
            if (timers->unk7A != 0) {
                entity->unkC8 |= 0x20;
            } else {
                entity->unkC8 |= 0x10;
            }
        }
    }
    if (timers->unk7C == 0) {
        if (timers->unk68 == 0 && timers->unk6A == 0 && timers->unk7D == 0 && rand() % 100 < 60) {
            timers->unk7D = 1;
            timers->unk7E = 20;
        }
        if (timers->unk7D != 0) {
            if (timers->unk7E != 0) {
                entity->unkD6 = (timers->unk7E << 7) / 20;
                timers->unk7E--;
            } else {
                timers->unk68 = entity->params->unk434;
                timers->unk7D = 0;
                func_0018DA38(entity, 7, 0, 0);
            }
        }
    }
    if (timers->unk68 != 0) {
        entity->model->unk102 |= 0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1E;
        entity->unkD6 = 0x80;
        ENTITY_FLAGS(entity) |= 0x200000;
        if (--timers->unk68 <= 0) {
            timers->unk6A = entity->params->unk438;
        }
    } else {
        entity->model->unk102 &= ~0x800;
        ((EntityFlagWord *)&entity->spawn_info->unk38)->bytes[3] = 0x1F;
        ENTITY_FLAGS(entity) &= ~0x200000;
        if (timers->unk6A != 0) {
            timers->unk6A--;
        }
    }
}
