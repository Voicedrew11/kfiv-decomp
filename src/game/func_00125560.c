#include "kf_entity.h"

extern int D_004467D8[2];
extern Entity D_003BEDF0[][0x80];
void *memset(void *, int, u32);

/* Empties one of the two entity arrays and makes the other array's entities forget the map
   records (Entity.unk14) that belonged to its map. */
void func_00125560(int array_index) {
    Entity *entity;
    int i;

    D_004467D8[array_index] = 0;
    memset(D_003BEDF0[array_index], 0, sizeof(D_003BEDF0[array_index]));
    for (i = 0; i < 0x80; i++) {
        entity = &D_003BEDF0[array_index ^ 1][i];
        if (entity->unk14 != NULL && ((u8)(((MapRecord *)entity->unk14)->unk38 >> 27) & 1) == array_index) {
            entity->unk14 = NULL;
        }
    }
}
