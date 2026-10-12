#include "kf_sound.h"

int func_001CBAE0();
int func_001CD6E0(u32);
int sceSifDmaStat(int);
int sceSifFreeIopHeap(int);
extern SoundDmaRequest D_005BF860;

/* Sound load step: once the piece in flight has arrived it is written to sound memory;
   -1 goes back a step while more is left to send. */
int func_001CCEB8(SoundLoadArgs *args, SoundReloadRequest *request) {
    int sent;

    if (sceSifDmaStat(request->unk1C) >= 0) {
        return 0;
    }
    if (func_001CBAE0(1, 0x90, request->voice->unk1A, 0, request->heap_addr, request->unk0C, D_005BF860.size) < 0) {
        sceSifFreeIopHeap(request->heap_addr);
        func_001CD6E0(request->unk14);
        return -1;
    }
    sent = D_005BF860.size;
    args->unk0C += sent;
    request->unk0C += sent;
    request->unk08 -= sent;
    return (request->unk08 <= 0) ? 1 : -1;
}
