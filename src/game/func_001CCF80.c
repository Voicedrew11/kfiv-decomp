#include "common.h"
#include "kf_sound.h"

int func_001CBAE0();
int FlushCache(int);
int func_001CD6E0(u32);
int sceSifFreeIopHeap(int);

int func_001CCF80(s32 unused, SoundReloadRequest *request) {
    SoundVoice *desc;
    s32 heap_src;

    desc = request->voice;
    if (func_001CBAE0(1, 0x80, desc->unk1A, 0) != 0) {
        sceSifFreeIopHeap(request->heap_addr);
        if (desc->unk1C != 0) {
            heap_src = desc->unk00;
        } else {
            heap_src = desc->unk04;
        }
        if (func_001CBAE0(1, 0x1010, desc->unk18, desc->unk0C, heap_src,
                          desc->unk1B) != 0) {
            func_001CD6E0(request->unk14);
            return -1;
        }
        FlushCache(0);
        return 1;
    }
    return 0;
}
