#include "kf_entity.h"

extern int D_00446770;
extern void *D_004FEB90[][0x100];

/* Looks up an entity of the current map by id. */
void *func_001267B0(int entity_id) {
    if ((u32)entity_id >= 0x100) {
        return NULL;
    }
    return D_004FEB90[D_00446770][entity_id];
}
