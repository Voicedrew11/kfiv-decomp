#include "kf_entity.h"
#include "kf_entity_world.h"

extern u8 D_0034B6D0[];
extern u8 D_00349690[];

/* Map tag 0x64: clears two flag bits on spawn id 1 while a cutscene gate runs. */
void func_0011A210(int map_slot) {
    Entity *entity;

    entity = Entity_GetBySpawnIdForMap(map_slot, 1);
    if (D_0034B6D0[0x340] == 0) {
        if (D_0034B6D0[0x345] != 0 && D_0034B6D0[0x346] == 0) {
            D_0034B6D0[0x346] = 1;
        }
        if (D_0034B6D0[0x346] == 1) {
            if (D_00349690[0x1905] == 0) {
                D_00349690[0x1905] = 1;
                D_0034B6D0[0x345] = 0x11;
            }
            return;
        }
    }
    if (entity != NULL) {
        ENTITY_FLAG_BITS(entity)->bit1 = 0;
        ENTITY_FLAG_BITS(entity)->bit7 = 0;
    }
}
