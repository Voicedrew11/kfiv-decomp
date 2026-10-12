#include "kf_entity.h"

extern int D_004467D8[2];
extern Entity D_003BEDF0[][0x80];

/* Clears flag bit 4 of every entity in one of the two entity arrays. */
void func_00125188(int array_index) {
    Entity *entity;
    int i;

    entity = D_003BEDF0[array_index];
    for (i = 0; i < D_004467D8[array_index]; i++, entity++) {
        entity->flags &= ~0x10;
    }
}
