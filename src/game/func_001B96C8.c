#include "kf_player.h"
#include "kf_game.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern GameDefinition D_00416D78[70];
extern PlayerItemDef D_0041B9B8[];
extern FxTableEntry D_00402DA8[];
extern LevelRecord D_003679A0[70];
extern u32 D_00446ED4;

extern int rand(void);
void func_001BB2E8(int);
int func_001BF6E8(void);

/* TRUE once every `period` frames */
static inline s32 is_frame_multiple(u32 period) {
    return D_00446ED4 % period == 0;
}

#define GAUGE_FULL 5000

/* a timer that is running, stepped; TRUE on the frame it runs out */
#define TIMER_SIGNED_ENDS(timer) ((timer) != 0 && --(timer) <= 0)
#define TIMER_ENDS(timer) ((timer) != 0 && --(timer) == 0)

#define RUN_DOWN_PAIR(index, timers, second, first) \
    if (TIMER_SIGNED_ENDS(D_00413F80.timers[index])) { \
        D_00413F80.first[index] = 0; \
        D_00413F80.second[index] = 0; \
    }

#define RUN_DOWN_SIGNED(index, timers, values) \
    if (TIMER_SIGNED_ENDS(D_00413F80.timers[index])) { \
        D_00413F80.values[index] = 0; \
    }

#define RUN_DOWN(index, timers, values) \
    if (TIMER_ENDS(D_00413F80.timers[index])) { \
        D_00413F80.values[index] = 0; \
    }

#define WEAR_OFF(index) \
    if (wear < (u16)D_00413F80.timed_flags[index]) { \
        D_00413F80.timed_flags[index] -= wear; \
    } else { \
        D_00413F80.timed_flags[index] = 0; \
    }

/* The per-frame bookkeeping of the player: timers, the two gauges, and the drawn bars. */
void func_001B96C8(void) {
    GameDefinition *definition = &D_00416D78[D_00413F80.unk3C];
    PlayerItemDef *item_def = &D_0041B9B8[D_00413F80.unk3C];
    FxTableEntry *fx_entry = &D_00402DA8[D_00413F80.unk264];
    s32 wear;
    s32 draining;
    s32 bar_target;
    s32 bar_step;

    if (D_00413F80.unk1A8 == 0 && D_00413F80.unk2B4 != 0 && D_00446ED4 % D_00413F80.unk2B4 == 0) {
        if (D_00413F80.unk2B0 > 0) {
            if (D_00413F80.unkB4 > 0) {
                D_00413F80.unkB4++;
            }
            D_00413F80.unk2B0--;
        } else {
            D_00413F80.unk2B0++;
            D_00413F80.unkB4--;
        }
        if (D_00413F80.unk2B0 == 0) {
            D_00413F80.unk2B4 = 0;
        }
    }

    RUN_DOWN_PAIR(0, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(1, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(2, unk158, unk188, unk11C);
    RUN_DOWN_PAIR(0, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(1, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(2, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(3, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(4, unk15E, unk18E, unk122);
    RUN_DOWN_PAIR(5, unk15E, unk18E, unk122);
    RUN_DOWN(0, unk14C, unk110);
    RUN_DOWN(1, unk14C, unk110);
    RUN_DOWN(2, unk14C, unk110);
    RUN_DOWN(3, unk14C, unk110);
    RUN_DOWN(4, unk14C, unk110);
    RUN_DOWN(5, unk14C, unk110);
    RUN_DOWN_SIGNED(0, unk176, unk13A);
    RUN_DOWN_SIGNED(1, unk176, unk13A);
    RUN_DOWN_SIGNED(2, unk176, unk13A);
    RUN_DOWN_SIGNED(0, unk17C, unk140);
    RUN_DOWN_SIGNED(1, unk17C, unk140);
    RUN_DOWN_SIGNED(2, unk17C, unk140);
    RUN_DOWN_SIGNED(3, unk17C, unk140);
    RUN_DOWN_SIGNED(4, unk17C, unk140);
    RUN_DOWN_SIGNED(5, unk17C, unk140);
    RUN_DOWN(0, unk16A, unk12E);
    RUN_DOWN(1, unk16A, unk12E);
    RUN_DOWN(2, unk16A, unk12E);
    RUN_DOWN(3, unk16A, unk12E);
    RUN_DOWN(4, unk16A, unk12E);
    RUN_DOWN(5, unk16A, unk12E);

    draining = D_00413F80.unkEC != 0;
    if (D_00413F80.unkF0 != 0 && D_00446ED4 % D_00413F80.unkF0 == 0 && D_00413F80.unk1A8 == 0 && D_00413F80.unkB4 > 0) {
        D_00413F80.unkB4++;
    }
    if (D_00413F80.unkF4 != 0 && D_00446ED4 % D_00413F80.unkF4 == 0 && D_00413F80.unk1A8 == 0) {
        D_00413F80.unkB4--;
    }
    if (D_00413F80.unkF8 != 0 && D_00446ED4 % D_00413F80.unkF8 == 0 && D_00413F80.unk1A8 == 0) {
        D_00413F80.unkB8++;
    }
    if (D_00413F80.unkFC != 0 && D_00446ED4 % D_00413F80.unkFC == 0 && D_00413F80.unk1A8 == 0) {
        D_00413F80.unkB8--;
    }
    if (D_00413F80.unk19C > 0) {
        D_00413F80.unk19C--;
    }
    if (D_00413F80.unk1A0 > 0) {
        D_00413F80.unk1A0--;
    }

    wear = 1;
    if (is_frame_multiple(3)) {
        if (D_00413F80.equipment[2] == 0x4B) {
            wear = 0;
        }
        if (D_00413F80.equipment[7] == 0x9B) {
            wear++;
        }
    }
    if ((u16)D_00413F80.timed_flags[0] != 0) {
        if (D_00413F80.unk2DC <= 0 ? is_frame_multiple(30) : is_frame_multiple(90)) {
            func_001BB2E8(3);
        }
    }
    if ((u16)D_00413F80.timed_flags[3] != 0) {
        draining = TRUE;
    }
    if (wear != 0) {
        WEAR_OFF(0);
        WEAR_OFF(3);
        WEAR_OFF(1);
        WEAR_OFF(2);
        WEAR_OFF(4);
        WEAR_OFF(5);
    }
    if (draining) {
        if (is_frame_multiple(30)) {
            func_001BB2E8(1);
            D_00413F80.unkB8--;
        }
    }

    if (D_00413F80.unkB4 > D_00413F80.unk58) {
        D_00413F80.unkB4 = D_00413F80.unk58;
    } else if (D_00413F80.unkB4 < 0) {
        D_00413F80.unkB4 = 0;
    }
    if (D_00413F80.unkB8 > D_00413F80.unk5C) {
        D_00413F80.unkB8 = D_00413F80.unk5C;
    } else if (D_00413F80.unkB8 < 0) {
        D_00413F80.unkB8 = 0;
    }

    if (D_00413F80.unk1A8 == 0 && D_00413F80.unk258 == 0) {
        if ((s32)(D_00413F80.unk1C8 >> 35) & 1) {
            D_00413F80.unkBC -= 83;
            if (D_00413F80.unkBC < 0) {
                D_00413F80.unkBC = 0;
            }
        } else if (item_def->unk63 < 2) {
            if (D_00413F80.unkC4 <= 0) {
                if (D_00413F80.unkBC < GAUGE_FULL) {
                    s32 rate = D_00413F80.unkAC;

                    if (rate < 20) {
                        rate = 20;
                    }
                    D_00413F80.unkBC += rate * 42 / (definition->unk33 + 1);
                    if (D_00413F80.unkBC > GAUGE_FULL) {
                        D_00413F80.unkBC = GAUGE_FULL;
                    }
                }
            }
            if (D_00413F80.unkC4 > 0) {
                D_00413F80.unkC4--;
            }
        } else {
            D_00413F80.unkBC = 0;
        }
    }
    if (D_00413F80.unk1A8 == 0 && D_00413F80.unk264 != 0) {
        if ((s32)(D_00413F80.unk1C8 >> 35) & 1) {
            D_00413F80.unkC0 -= 83;
            if (D_00413F80.unkC0 < 0) {
                D_00413F80.unkC0 = 0;
            }
        } else {
            if (D_00413F80.unkC8 <= 0) {
                if (D_00413F80.unkC0 < GAUGE_FULL) {
                    s32 rate = D_00413F80.unkB0;

                    if (rate < 20) {
                        rate = 20;
                    }
                    D_00413F80.unkC0 += rate * 42 / (fx_entry->unkFE + 1);
                    if (D_00413F80.unkC0 > GAUGE_FULL) {
                        D_00413F80.unkC0 = GAUGE_FULL;
                    }
                }
            }
            if (D_00413F80.unkC8 > 0) {
                D_00413F80.unkC8--;
            }
        }
    }
    if (D_00413F80.unk3D == 0 && item_def->unk42 == 0xFFFF
        && (item_def->unk52 == 0xFFFF || (D_003679A0 + D_00413F80.unk3C)->unk14 + 1 < 3) && func_001BF6E8() == 0) {
        D_00413F80.unkC0 = 0;
        D_00413F80.unk264 = 0;
    }
    if ((u16)D_00413F80.timed_flags[5] != 0) {
        D_00413F80.unkC0 = 0;
    }
    if (!((s32)(D_00413F80.unk1C8 >> 37) & 1) && D_00413F80.unk200 > 0) {
        D_00413F80.unk200--;
    }

    if (D_00413F80.unk1A8 == 0 && D_00413F80.unkB4 > 0) {
        bar_target = D_00413F80.unkB4 * 49 / D_00413F80.unk58;
        if (bar_target < D_00413F80.unk1A4) {
            D_00413F80.unk1A4 -= 2;
            if (D_00413F80.unk1A4 < bar_target) {
                D_00413F80.unk1A4 = bar_target;
            }
        } else if (D_00413F80.unk1A4 < bar_target) {
            D_00413F80.unk1A4++;
            if (bar_target < D_00413F80.unk1A4) {
                D_00413F80.unk1A4 = bar_target;
            }
        }
    } else {
        D_00413F80.unk1A4 = 0;
    }

    if (D_00413F80.unk5C < D_00413F80.unk1A6) {
        D_00413F80.unk1A6 = D_00413F80.unk5C;
    }
    bar_step = D_00413F80.unk5C / 64;
    if (bar_step <= 0) {
        bar_step = 1;
    }
    if (D_00413F80.unkB8 < D_00413F80.unk1A6) {
        D_00413F80.unk1A6 -= bar_step;
        if (D_00413F80.unk1A6 < D_00413F80.unkB8) {
            D_00413F80.unk1A6 = D_00413F80.unkB8;
        }
        if (D_00413F80.unk1A6 < 0) {
            D_00413F80.unk1A6 = 0;
        }
    } else if (D_00413F80.unk1A6 < D_00413F80.unkB8) {
        D_00413F80.unk1A6 += bar_step;
        if (D_00413F80.unkB8 < D_00413F80.unk1A6) {
            D_00413F80.unk1A6 = D_00413F80.unkB8;
        }
    }

    if (D_00413F80.unk3C == 0x24) {
        if (is_frame_multiple(600)) {
            if ((s32)(rand() * 100.0f * 4.656613e-10f) < 70) {
                D_00413F80.timed_flags[0] = 1800;
                D_00413F80.unk2C4 = D_00446ED4;
            }
        }
    }
}
