#include "kf_entity.h"

s32 func_0018ED98(Entity *, f32, f32);
void func_001958B8(char *, void (*)(void), int);
int func_001488C0(char *);
s32 func_00148990(Entity *);
void func_00181C60(Entity *);
void func_00147D40(Entity *);
void func_00148100(Entity *);
void func_001483F8(Entity *);
void func_00148688(Entity *);

void func_00147B98(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;

    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    if (func_0018ED98(entity, 300.0f, 3.1415927f)) {
        // match: called as returning int; only then is the 2 below loaded into $3
        ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00181C60, 1);
        unkE0->unk7A = 2;
        return;
    }
    unkE0->unk7A = 1;
    ENTITY_FLAG_BITS(entity)->bit9 = 1;
    if (entity->unkA2 == 0 && !(s32)(entity->flags >> 63) && func_001488C0((char *)entity)) {
        if (func_00148990(entity)) {
            if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                func_001958B8((char *)entity, (void (*)(void))func_001483F8, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00148688, unknown);
            }
        } else {
            if (ENTITY_FLAG_BITS(entity)->bits58 == 0) {
                func_001958B8((char *)entity, (void (*)(void))func_00147D40, unknown);
            } else {
                func_001958B8((char *)entity, (void (*)(void))func_00148100, unknown);
            }
        }
        return;
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
