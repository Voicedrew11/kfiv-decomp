#include "kf_sound.h"

int func_001CBAE0();
int func_001CBC30(int, int, u32);
int func_001CBD20(void);
int func_001CE408(int);
int FlushCache(int);
int sceSifAllocIopHeap(int);
extern SoundState *D_004471D4;
extern int D_004469F8;
extern int D_004471C4;
extern SoundBank *D_004471C8;
extern SoundVoice *D_004471CC;
extern int D_004471D0;

/* func_001CE170 in steps, to be called every frame until it stops returning -1: step 0 takes a free
   bank, allocates IOP memory and starts sending the data, step 1 waits for the transfer, step 2 hands
   bank and voice to the driver. Returns the bank's index when done, -2 when it failed. */
int func_001CDF90(int bank_id, int voice_index, int data, u32 size) {
    int index;
    int iop_heap;

    switch (D_004469F8) {
    case 0:
        D_004471C8 = D_004471D4->banks;
        for (index = 0; index < 4; index++) {
            if (D_004471C8->iop_heap == 0) {
                D_004471C4 = index;
                break;
            }
            D_004471C8++;
        }
        if (index >= 4) {
            D_004469F8 = 0;
            return -2;
        }
        if (bank_id == -1) {
            bank_id = D_004471C4;
        } else if ((u32)bank_id >= 4U) {
            return -1;
        }
        D_004471C8->unk0C = bank_id;
        size = (size + 0xF) / 16 * 16;
        D_004471C8->voice = voice_index;
        D_004471CC = &D_004471D4->voices[voice_index];
        D_004471C8->unk04 = size;
        iop_heap = sceSifAllocIopHeap(size);
        D_004471C8->iop_heap = iop_heap;
        if (iop_heap == 0 || (u32)(iop_heap + D_004471C8->unk04) > 0x1FFFFFU) {
            func_001CE408(D_004471C4);
            D_004469F8 = 0;
            return -2;
        }
        func_001CBC30(iop_heap, data, D_004471C8->unk04);
        D_004469F8++;
        break;
    case 1:
        if (func_001CBD20() != 0) {
            D_004469F8++;
        }
        break;
    case 2:
        if (D_004471CC->unk1C != 0) {
            D_004471D0 = D_004471CC->unk00;
        } else {
            D_004471D0 = D_004471CC->unk04;
        }
        if (func_001CBAE0(1, 0x1010, D_004471C8->unk0C, D_004471CC->unk0C, D_004471D0, D_004471CC->unk1B) != 0 ||
            func_001CBAE0(1, 0x2000, D_004471C8->unk0C, D_004471C8->iop_heap) != 0) {
            func_001CE408(D_004471C4);
            D_004469F8 = 0;
            return -2;
        }
        FlushCache(0);
        D_004469F8 = 0;
        break;
    }
    if (D_004469F8 != 0) {
        return -1;
    }
    return D_004471C4;
}
