#include "kf_entity.h"

extern float D_00413F80[];
extern u32 D_00446ED4;

s32 func_0015E000(Entity *, f32, f32);
s32 func_0018ED98(Entity *, f32, f32);
int func_00192C98(void *, int, int, u32, int);
void func_0015DB88(Entity *);
s32 func_0018EEE0(Entity *, f32, f32);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
int func_0018F030(char *);
int func_0018EAC0(void *, void *);
s16 func_00190F90(float, void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00181AE0(void);
void func_00181C60(void);
void func_00181E48(void);
void func_00182240(void);

void func_0015E350(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 direction[4];
    s32 side;

    entity->unkC6 |= 0x3000;
    switch (entity->state) {
    case 0:
        func_00192C98(entity, 3, 0, entity->params->unkC4, 0);
        ENTITY_FLAGS(entity) = (ENTITY_FLAGS(entity) & ~0x3800) | 0x800;
        unkE0->unk1C = 0.0f;
        ENTITY_FLAGS(entity) &= ~0x200;
        entity->state++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->state++;
        }
        break;
    case 2:
        if (func_0015E000(entity, 200.0f, 1.0471976f)) {
            func_001958B8((char *)entity, (void (*)(void))func_0015DB88, 1);
            break;
        }
        if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
            func_001958B8((char *)entity, func_00181C60, 1);
            break;
        }
        if (func_0018EEE0(entity, 300.0f, 0.5235988f)) {
            func_001958B8((char *)entity, func_00181AE0, 1);
            break;
        }
        if ((s32)(ENTITY_FLAGS(entity) >> 53) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            unkE0->unk68 = 2;
            if ((s32)(ENTITY_FLAGS(entity) >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, 1);
            }
        }
        side = func_0018FD50(entity, 0);
        ENTITY_FLAGS(entity) |= 0x200;
        if (side == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = 0;
            if (entity->unkCF != 0) {
                entity->unk9E = D_00446ED4;
            }
            func_00190B88(entity, 0);
            if (entity->unkCD == 0) {
                unkE0->unk68 = 2;
                if (func_0018F030((char *)entity)) {
                    func_001958B8((char *)entity, func_00182240, 1);
                } else {
                    func_001958B8((char *)entity, func_00181E48, 1);
                }
            }
            break;
        }
        entity->unk5C = (s32)D_00413F80;
        side = func_0018EAC0(entity, direction);
        if (side > 0) {
            entity->unkC8 = 0x241;
        } else if (side < 0) {
            entity->unkC8 = 0x141;
        } else {
            entity->unkC8 = func_00190F90(entity->unk38 + 100.0f + 30.0f, entity, 1);
        }
        ((void (*)(Entity *))unkE0->unk18)(entity);
        break;
    }
}
