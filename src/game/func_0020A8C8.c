#include "kf_splash.h"
#include "kf_game.h"

void func_00217008(void);
void *func_00217760(int, int);
s32 func_0019CDC0(s32);
int func_00209A48(float, void *, int); // match: an int return keeps $2 busy after the calls
int func_001CEBE0(void);

extern BootSpriteTrack D_0042C400[2];
extern BootSpriteTrack D_0042C448;
extern BootSprite D_005FAEF0[2];
extern BootSprite D_005FAF28;
extern BootSprite D_005FAF48;
extern u16 D_00446E8C[2];
extern int D_004473C8;
extern int D_004473DC;
extern s8 D_004473E0;

/* One frame of the two-choice boot screen: moves the choice with the pad, and once one is confirmed
   fades the screen out and returns the choice plus one. Returns 0 until then. */
int func_0020A8C8(void) {
    f32 brightness;
    PadReport *report;
    u16 stick;
    u16 stick_pressed;
    int i;

    func_00217008();
    brightness = 1.0f;
    report = func_00217760(0, 0);
    stick = (report->left_stick_y > 0x3FU) ? 0 : 0x10;
    if (report->left_stick_y > 0xC0) {
        stick |= 0x40;
    }
    stick_pressed = stick & ~D_00446E8C[1];
    D_00446E8C[1] = stick;
    if (D_004473E0) {
        D_004473C8--;
        if (D_004473C8 <= 0) {
            D_00446E8C[1] = 0xFFFF;
            return D_004473DC + 1;
        }
        brightness = D_004473C8 / 30.0f;
    } else if (report->pressed & 0x4008) {
        func_0019CDC0(0);
        D_004473E0 = 1;
        D_004473C8 = 30;
        D_005FAEF0[D_004473DC].track = &D_0042C448;
    } else if ((stick_pressed | report->pressed) & 0x50) {
        func_0019CDC0(1);
        D_0042C400[D_004473DC].length = 0;
        // match: read through the union, or the reload moves ahead of the store above
        if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & 0x10) {
            D_004473DC--;
        } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & 0x40) {
            D_004473DC++;
        }
        D_004473DC &= 1;
        D_005FAEF0[D_004473DC].frame = 60;
        D_0042C400[D_004473DC].length = 120;
    }
    for (i = 0; i < 2; i++) {
        func_00209A48(brightness, &D_005FAEF0[i], 0);
    }
    func_00209A48(brightness, &D_005FAF48, 0);
    func_00209A48(brightness, &D_005FAF28, 0);
    func_001CEBE0();
    return 0;
}
