#include "kf_entity.h"
#include "kf_entity_world.h"

/* Counts nearby allies in certain states and scales a ranged-attack weight from it. */
int func_0016A450(Entity *entity) {
    EntityUnkE0Kind17FC60 *work;
    Entity *ally;
    s32 ally_index;
    s32 ally_count;
    u64 ally_flags;
    s32 flag_bits;

    work = (EntityUnkE0Kind17FC60 *)&entity->unkE0;
    ally_count = 0;
    for (ally_index = 0; ally_index < 8; ally_index++) {
        ally = Entity_GetBySpawnId(ally_index + 0xA);
        if (ally != NULL) {
            ally_flags = ally->flags;
            if ((ally_flags & 3) == 1) {
                flag_bits = (s32)((ally_flags << 0x1E) >> 0x20); // match: bits 30-31 of the flag word
                if (((flag_bits & 3) == 0) || ((flag_bits & 3) == 3)) {
                    ally_count++;
                }
            }
        }
    }
    entity->params->unk3D8[2].record.unk00 = (f32)work->unk68[2] * (f32)ally_count * 0.125f;
    return 0;
}
