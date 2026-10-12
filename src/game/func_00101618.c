#include "kf_map.h"
#include "kf_sound.h"

extern SoundLoadQueue D_0038A130;

int func_0019F6B0(void);
void func_0019EA98(int);
int func_001A1048(int, int);
int func_001A0C18(int, int, int);
int func_0019F628(int, int);

/* Runs the queued sound loads, one at a time. */
void func_00101618(void) {
    if (D_0038A130.unk10 == NULL && D_0038A130.unk14 != NULL) {
        D_0038A130.unk10 = D_0038A130.unk14;
        D_0038A130.unk0C = 1;
        D_0038A130.unk14 = NULL;
    }
    if (D_0038A130.step == 3 && (D_0038A130.unk0C != 0 || D_0038A130.unk18 != 0)) {
        if (func_0019F6B0() != 0) {
            D_0038A130.step = 0;
        }
    }
    if (D_0038A130.step == 0) {
        if (D_0038A130.unk0C != 0) {
            D_0038A130.step = 1;
            func_0019EA98(0);
        } else if (D_0038A130.unk18 != 0) {
            D_0038A130.step = 2;
        } else if (D_0038A130.unk04 != 0) {
            D_0038A130.step = 3;
        }
    }
    switch (D_0038A130.step) {
    case 1:
        D_0038A130.unk0C = func_001A1048((int)D_0038A130.unk10, D_0038A130.unk0C);
        if (D_0038A130.unk0C == 0) {
            D_0038A130.unk10 = NULL;
            D_0038A130.step = 0;
        }
        break;
    case 2:
        if (D_0038A130.unk18 != 0) {
            D_0038A130.unk18 = func_001A0C18(D_0038A130.unk1C, D_0038A130.unk20, D_0038A130.unk24 + 7);
            if (D_0038A130.unk18 == 0) {
                D_0038A130.step = 0;
            }
        }
        break;
    case 3:
        if (D_0038A130.unk04 != 0) {
            D_0038A130.unk04 = func_0019F628(D_0038A130.unk00, D_0038A130.unk04);
            if (D_0038A130.unk04 == 0) {
                D_0038A130.step = 0;
            }
        } else {
            D_0038A130.step = 0;
        }
        break;
    }
}
