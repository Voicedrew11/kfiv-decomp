#include "kf_sound.h"

int func_001CBAE0();
int func_001CBC30(int, int, u32);
int func_001CBD20(void);
int func_001CD6E0(u32);
int sceSifAllocIopHeap(int);
extern SoundState *D_004471D4;

/* Sound load step 0: clears the request, gets the voice's two data blocks on the IOP and
   uploads the first data to them. */
int func_001CCC28(SoundLoadArgs *args, SoundReloadRequest *request) {
    SoundVoice *voice;
    int voice_index;

    request->voice = NULL;
    request->unk04 = 0;
    request->unk08 = 0;
    *(void **)&request->unk0C = NULL; // match: unk0C and heap_addr are stored as pointers here
    *(void **)&request->heap_addr = NULL; // match: as above
    request->unk14 = 0;
    request->unk18 = 0;
    request->unk1C = 0;
    voice_index = args->unk00;
    voice = &D_004471D4->voices[voice_index];
    voice->unk18 = args->unk00;
    voice->unk1B = args->unk14;
    while (args->unk08 & 0xF) {
        args->unk08++;
    }
    voice->unk10 = args->unk08;
    voice->unk00 = sceSifAllocIopHeap(voice->unk10);
    voice->unk04 = sceSifAllocIopHeap(voice->unk10);
    if (voice->unk00 == 0 || voice->unk04 == 0 || (u32)(voice->unk00 + voice->unk10) > 0x1FFFFF ||
        (u32)(voice->unk04 + voice->unk10) > 0x1FFFFF) {
        func_001CD6E0(voice_index);
        return -1;
    }
    func_001CBC30(voice->unk00, args->unk04, voice->unk10);
    while (func_001CBD20() == 0) {
    }
    func_001CBAE0(1, 0x1070, voice->unk04, voice->unk00, voice->unk10);
    request->voice = voice;
    request->unk14 = voice_index;
    return 1;
}
