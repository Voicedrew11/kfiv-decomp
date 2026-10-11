#include "kf_entity.h"

void func_001958B8(char *, void (*)(void), int);
void func_00179DD8(Entity *);
void func_00179C10(Entity *);

extern u8 D_00413FBC[];

void func_00179BB0(Entity *entity) {
    if (D_00413FBC[0] == 0x2E) {
        ENTITY_FLAG_BITS(entity)->bit29 = TRUE;
        func_001958B8((char *)entity, func_00179DD8, 1);
    } else {
        ENTITY_FLAG_BITS(entity)->bit29 = FALSE;
        func_001958B8((char *)entity, func_00179C10, 1);
    }
}
