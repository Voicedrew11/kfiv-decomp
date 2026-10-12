#include "kf_entity.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern u32 D_00446ED4;

int rand(void);

/* rand() scaled to 0-99 (the factor is 2 to the power -31) */
#define ROLL_PERCENT() ((s32)(rand() * 100.0f * 0.0000000004656613f))
/* a chance is never under 5 percent */
#define AT_LEAST_5(chance) ((chance) < 5 ? 5 : (chance))

/* A hit may start the player's six timed ailments: each has a chance in percent in the
   attack, less the player's resistance to it but never under 5. One that is already
   running is left alone. */
void func_001B2C48(void *attack_chances) {
    EntityParamsUnk240PartUnk12 *chances = attack_chances;

    if ((u16)chances->unk00[0] != 0 && D_00413F80.flag_timers[0] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[0] - D_00413F80.unkDC[0])) {
            D_00413F80.timed_flags[0] = 1800;
            D_00413F80.unk2C4 = D_00446ED4;
        }
    }
    if ((u16)chances->unk00[1] != 0 && D_00413F80.flag_timers[1] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[1] - D_00413F80.unkDC[1])) {
            D_00413F80.timed_flags[1] = 150;
            D_00413F80.unk2C8 = D_00446ED4;
        }
    }
    if ((u16)chances->unk00[2] != 0 && D_00413F80.flag_timers[2] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[2] - D_00413F80.unkDC[2])) {
            D_00413F80.timed_flags[2] = 3600;
            D_00413F80.unk2CC = D_00446ED4;
        }
    }
    if ((u16)chances->unk00[3] != 0 && D_00413F80.flag_timers[3] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[3] - D_00413F80.unkDC[3])) {
            D_00413F80.timed_flags[3] = 5400;
            D_00413F80.unk2D0 = D_00446ED4;
        }
    }
    if ((u16)chances->unk00[4] != 0 && D_00413F80.flag_timers[4] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[4] - D_00413F80.unkDC[4])) {
            D_00413F80.timed_flags[4] = 1800;
            D_00413F80.unk2D4 = D_00446ED4;
        }
    }
    if ((u16)chances->unk00[5] != 0 && D_00413F80.flag_timers[5] == 0) {
        if (ROLL_PERCENT() < AT_LEAST_5((u16)chances->unk00[5] - D_00413F80.unkDC[5])) {
            D_00413F80.timed_flags[5] = 18000;
            D_00413F80.unk2D8 = D_00446ED4;
        }
    }
}
