#include "kf_sound.h"

int func_001CBAE0();
void FlushCache(int); // match: void here, an int result changes the register after the first call
int sceSifBindRpc(int *, int, int);
extern SoundState *D_004471D4;
extern int D_005BEE80[];
extern SoundState D_005BF380;
extern SoundUpload D_005BF870;
extern int D_005BF890[];

/* Starts the sound driver: binds its RPC service on the IOP, then resets the driver state,
   every voice, bank and effect slot. */
int func_001CB778(void) {
    SoundVoice *voice;
    SoundBank *bank;
    SoundEffectFields *effect;
    s8 *entry;
    int i;

    do {
        if (sceSifBindRpc(D_005BF890, 0x1010, 0) < 0) {
            while (TRUE) {
            }
        }
        for (i = 10000; i != -1; i--) {
        }
    } while (D_005BF890[9] == 0);
    FlushCache(0);
    D_004471D4 = &D_005BF380;
    D_004471D4->dma_src = func_001CBAE0(1, 0x10);
    *(int *)D_004471D4->unk4D0 = 0x3FFF; // match: the int at 0x4D0; the header has six chars there
    func_001CBAE0(1, 0x30, 0x3FFF, 0x3FFF);
    D_004471D4->load_addr = NULL;
    D_004471D4->unk4C8 = func_001CBAE0(1, 0xA0);
    // match: retail stores these three as pointers (the header has them as integers)
    *(int **)&D_004471D4->dma_dest = D_005BEE80;
    D_005BEE80[0] = 0;
    // match: as above
    *(char **)&D_004471D4->unk4BC = *(char **)&D_004471D4->unk4C0 = (char *)0x5010;
    *(u16 *)&D_004471D4->unk4D0[4] = 1; // match: the u16 at 0x4D4
    D_004471D4->active = 0;
    voice = D_004471D4->voices;
    for (i = 15; i != -1; i--, voice++) {
        voice->unk00 = voice->unk04 = voice->iop_heap = voice->unk0C = 0;
        voice->unk1B = 0;
        voice->unk18 = 0;
        voice->unk1A = 0;
        voice->unk14 = 0;
        voice->unk1C = 1;
    }
    bank = D_004471D4->banks;
    for (i = 3; i != -1; i--, bank++) {
        bank->iop_heap = 0;
        bank->unk0C = 0;
        bank->unk04 = 0;
    }
    entry = D_004471D4->unk240[0];
    for (i = 125; i != -1; i--, entry += 4) {
        entry[0] = entry[1] = entry[2] = entry[3] = -1;
    }
    effect = (SoundEffectFields *)D_004471D4->effects;
    for (i = 3; i != -1; i--, effect++) {
        effect->unk00 = -1;
        effect->unk04 = effect->unk08 = effect->unk0A = effect->unk0C = effect->unk10 = effect->unk14 = effect->unk16 = 0;
    }
    D_005BF870.active = 0;
    FlushCache(0);
    return 0;
}
