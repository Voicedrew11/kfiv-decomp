#include "kf_sound.h"

s32 func_0021CCC8(s32);
s32 sceSdRemote(s32, ...);

s32 func_0021C350(StreamVoice *voice) {
    voice->unk00 = 3;
    func_0021CCC8(0);
    voice->unk50 = (sceSdRemote(1, 0x80E0, 0, 2, 0, 0) & 0xFFFFFF) - voice->unk44;
    sceSdRemote(1, 0x80D0, 0, 0, voice->unk58, 0x4000, 0x800);
    return 1;
}
