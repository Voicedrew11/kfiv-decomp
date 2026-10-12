#include "kf_entity.h"

extern u32 D_00446ED4;

int rand(void);
s32 func_0018ED98(Entity *, f32, f32);
s32 func_00197F00(Entity *);
s32 func_0018F648(Entity *);
s32 func_0018FD50(Entity *, s32);
void func_00190B88(Entity *, s32);
s32 func_00198D10(Entity *);
s32 func_0018F9C0(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00181C60(Entity *);
void func_00181E48(Entity *);
void func_00175AE8(Entity *);

void func_00175780(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    s32 side;

    ENTITY_FLAG_BITS(entity)->bits11 = 0;
    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, (void (*)(void))func_00181C60, 1);
        return;
    }
    if (func_00197F00(entity) && entity->unkA2 == 0) {
        func_0018F648(entity);
        func_001958B8((char *)entity, (void (*)(void))func_00181E48, unknown);
    } else {
        ENTITY_FLAG_BITS(entity)->bit32 = TRUE;
        side = func_0018FD50(entity, 0);
        if (entity->unkA0 == 0) {
            entity->unkCF = entity->unkCD;
            entity->unkCD = side;
            if (entity->unkCD != entity->unkCF) {
                entity->unk9E = D_00446ED4;
            }
            func_00190B88(entity, side);
            if (entity->unkCD == 0) {
                if (func_00198D10(entity)) {
                    func_0018F9C0(entity);
                    func_001958B8((char *)entity, (void (*)(void))func_00175AE8, unknown);
                    return;
                }
            } else {
                entity->unkA0 = rand() % 60 + 0x3C;
            }
        }
        func_001958B8((char *)entity, unkE0->unk18, unknown);
    }
}
