#include "kf_game.h"
#include "kf_state.h"

extern ModeState D_005CF410[];
extern GameStatus D_0038CC50;
extern int D_00446734;
extern char D_00428B00[];
extern char D_00440980[];
extern char D_004409B0[];
extern char D_00446D18[];
extern char D_00446D28[];

int sprintf(char *, const char *, ...);
s32 func_0021B0C0(void);
int func_0021ACB8(u32, u32);
s32 func_0021B468(void);
int func_0021ADD0(u32, u32, int, int, int);
s32 func_001FAFC0(void);

s32 func_001FA568(void) {
    ModeState *mode = D_005CF410;
    SlotRecord *slot = &D_00428B80[mode->slot];
    s32 result = 0;
    s32 file = mode->file;
    s32 status;
    s32 code;

    switch (mode->step) {
    case 0:
        if (func_0021B0C0() <= 0) {
            if (func_0021ACB8(mode->slot, 0) != 0) {
                if (++mode->retries == 5) {
                    result = func_001FAFC0();
                }
            } else {
                mode->retries = 0;
                mode->step++;
            }
        }
        break;
    case 1:
        if (func_0021B0C0() <= 0) {
            if (func_0021B468() != 0) {
                return func_001FAFC0();
            }
            mode->step++;
        }
        break;
    case 2:
        if (func_0021B0C0() > 0) {
            break;
        }
        sprintf(D_00428B00, D_004409B0, D_00440980, D_00446D18, D_00440980, D_00446D28, file);
        mode->step++;
    case 3:
        if (func_0021ADD0(mode->slot, 0, (int)D_00428B00, (int)slot->unk08, 0x20000) != 0) {
            if (++mode->retries == 5) {
                result = func_001FAFC0();
            }
        } else {
            mode->retries = 0;
            mode->step++;
        }
        break;
    case 4:
        status = func_0021B0C0();
        if (status > 0) {
            break;
        }
        code = func_0021B468();
        if (code != 0x48 || status < 0) {
            slot->unk07 = 1;
            result = func_001FAFC0();
        } else {
            D_00446734 = 1;
            result = 1;
            D_0038CC50.bit9 = 1;
        }
        break;
    }
    return result;
}
