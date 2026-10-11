#include "common.h"
#include "kf_sound.h"

extern int D_00446858;
extern u8 D_00446896;
extern char D_004FF558[];

int func_001CEF48(u32);
int func_001CEE78(int, s16, s16, s16, s16, s16, s16, int);

int func_0019F188(void *context, int cue_index) {
    SoundCue *cue;

    if (cue_index == 0x12) {
        D_00446858 += 1;
        return 0;
    }
    if (D_00446896 < 4U) {
        func_001CEF48((char)D_00446896);
    }
    cue = (SoundCue *)(D_004FF558 + cue_index * 0x10);
    D_00446896 = func_001CEE78(cue->flags, cue->unk04, cue->unk06, cue->unk08, cue->unk0A, cue->unk0C, cue->unk0E, 0);
    D_00446858 += 1;
    return 0;
}
