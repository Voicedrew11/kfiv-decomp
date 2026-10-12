#include "common.h"
#include "kf_sound.h"

extern SoundScriptState D_0043EE78;
extern int D_00446868;
extern u8 D_00446895;
extern char D_004FF958[];

int func_001CEF48(u32);
int func_001CEE78(int, s16, s16, s16, s16, s16, s16, int);

/* Runs one sound script step: stops the previous cue and starts the one the script state names. */
int func_001A0DF8(void) {
    SoundScriptState state;
    SoundCue *cue;

    state = D_0043EE78;
    if (D_00446895 < 4U) {
        func_001CEF48((char)D_00446895);
    }
    cue = (SoundCue *)(D_004FF958 + state.cue_index * 0x10);
    D_00446895 = func_001CEE78(cue->flags, cue->unk04, cue->unk06, cue->unk08, cue->unk0A, cue->unk0C, cue->unk0E, 1);
    D_00446868 += 1;
    return 0;
}
