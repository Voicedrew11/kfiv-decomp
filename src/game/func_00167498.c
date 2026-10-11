#include "kf_entity.h"

s32 func_0018ED98(Entity *, f32, f32);
s32 func_00167F08(Entity *);
s32 func_00167BA0(Entity *);
s32 func_00167C70(Entity *);
void func_00167608(Entity *);
void func_00167808(Entity *);
void func_001958B8(char *, void (*)(void), int);
void func_00181C60(void);
void func_00181E48(void);
void func_00182240(void);

void func_00167498(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    if (func_0018ED98(entity, 300.0f, 0.5235988f)) {
        func_001958B8((char *)entity, func_00181C60, 1);
    } else {
        entity->flags |= 0x400;
        entity->flags |= 0x200;
        if (func_00167F08(entity)) {
            func_001958B8((char *)entity, (void (*)(void))func_00167808, unknown);
        } else if (entity->unkA2 == 0 && func_00167BA0(entity)) {
            if (func_00167C70(entity)) {
                if (((s32)(entity->flags >> 58) & 3) == 0) {
                    func_001958B8((char *)entity, (void (*)(void))func_00167608, unknown);
                } else {
                    func_001958B8((char *)entity, func_00182240, unknown);
                }
            } else {
                func_001958B8((char *)entity, func_00181E48, unknown);
            }
        } else {
            func_001958B8((char *)entity, (void (*)(void))unkE0->unk18, unknown);
        }
    }
}
