#include "kf_game.h"

extern GameStatus D_0038CC50;
extern int D_00447358;
extern int D_0044735C;
extern int D_00447368;
extern int D_00447370;
extern int D_00447374;
extern int D_00447378;
extern int D_0044737C;
extern int D_00447380;
extern int D_00447384;

void func_001B9478(void);

/* Starts splash image splash_id: hold_frames (<0 = forever), skippable, fade-in and fade-out frames. Returns 1 if started. */
int func_00208198(int splash_id, int hold_frames, int skippable, int fade_in_frames, int fade_out_frames) {
    int skip_value;
    int fade_in;
    int fade_out;

    if (!D_0038CC50.bit2 && !(D_0038CC50.bit3 && D_0038CC50.bit5) && !(D_0038CC50.bit4 && !D_0038CC50.bit8) &&
        !D_0038CC50.bit14) {
        if (D_00447358 != 0) {
            return 0;
        }
        if (D_0038CC50.unk30 != 0) {
            return 0;
        }
        D_00447368 = 2;
        D_00447358 = 1;
        skip_value = skippable != 0 ? skippable : 1;
        if (hold_frames < 0) {
            skippable = skip_value;
        }
        fade_in = fade_in_frames > 0 ? fade_in_frames : 1;
        fade_out = fade_out_frames > 0 ? fade_out_frames : 1;
        D_0044735C = splash_id;
        D_00447370 = hold_frames;
        D_00447374 = skippable;
        D_00447378 = fade_in;
        D_00447380 = fade_out;
        D_0044737C = 0;
        D_00447384 = 0;
        D_0038CC50.unk30 = 0x87F;
        func_001B9478();
        return 1;
    }
    return 0;
}
