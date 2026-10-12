#include "kf_entity.h"
#include "kf_map.h"

#define SPAWN_FLAGS(spawn_info) ((EntitySpawnFlags *)&(spawn_info)->unk38)

extern EntitySpawnInfo *D_004467D0[2];
extern MapSlot D_0038CD20[];
extern PlayerData D_00413F80;

int func_00103548(int, int);
int rand(void);

/* Decides whether a dead entity comes back: it must be allowed to, have deaths left, and the player must
   have come within its respawn distance, at which point one roll against its respawn chance is made. */
s32 func_001963A8(Entity *entity) {
    f32 distance[4];
    EntitySpawnInfo *spawn_info;
    s32 death_count;

    if (!ENTITY_FLAG_BITS(entity)->bit7) {
        return FALSE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit4 && (entity->spawn_info->unk38 & (0x8000L << 46))) {
        return FALSE;
    }
    death_count = func_00103548(D_0038CD20[entity->group_id].tag, entity->spawn_info - D_004467D0[entity->group_id]);
    if ((s8)SPAWN_FLAGS(entity->spawn_info)->bytes[5] != 0 && death_count >= (s8)SPAWN_FLAGS(entity->spawn_info)->bytes[5]) {
        return FALSE;
    }
    spawn_info = entity->spawn_info;
    __asm__ volatile(
        "lqc2 $vf16, 0(%1)\n"
        "lqc2 $vf17, 0(%2)\n"
        "vsub.xyz $vf16, $vf16, $vf17\n"
        "vmul.xz $vf16, $vf16, $vf16\n"
        "vaddz.x $vf16, $vf16, $vf16z\n"
        "vsqrt Q, $vf16x\n"
        "vwaitq\n"
        "vaddq.x $vf16, $vf0, Q\n"
        "sqc2 $vf16, 0(%3)\n"
        : "=m"(distance) : "r"(&spawn_info->spawn_x), "r"(D_00413F80.position), "r"(distance));
    if (distance[0] <= (f32)spawn_info->unk50) {
        if ((entity->spawn_info->unk38 & (0xC000L << 45)) == (0x8000L << 44)) {
            return TRUE;
        }
        if (ENTITY_FLAG_BITS(entity)->pad8) {
            return FALSE;
        }
        ENTITY_FLAG_BITS(entity)->pad8 = TRUE;
        if (rand() % 100 < entity->spawn_info->unk52) {
            return TRUE;
        }
    } else {
        ENTITY_FLAG_BITS(entity)->pad8 = FALSE;
    }
    return FALSE;
}
