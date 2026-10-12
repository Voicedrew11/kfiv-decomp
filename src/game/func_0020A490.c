#include "kf_splash.h"
#include "kf_game.h"

void func_00217008(void);
void *func_00217760(int, int);
int func_00209A48(float, void *, int); // match: an int return keeps $2 busy after the calls

extern BootSpriteTrack D_0042C348[4];
extern BootSpriteTrack D_0042C3D8;
extern BootSprite D_005FADA0;
extern BootSprite D_005FADC0;
extern BootSprite D_005FADE0;
extern BootSprite D_005FAE00;
extern BootSprite D_005FAE20;
extern BootSprite D_005FAE60;
extern BootSprite D_005FAE80[4];
extern u16 D_00446E8C[2];
extern int D_004473C8;
extern int D_004473D4;
extern s8 D_004473D8;

/* One frame of the four-choice boot screen: moves the choice with the pad, and once one is confirmed
   fades the screen out and returns the choice plus one. Returns 0 until then. */
int func_0020A490(void) {
    f32 brightness;
    PadReport *report;
    u16 stick;
    u16 stick_pressed;
    int i;

    func_00217008();
    brightness = 1.0f;
    report = func_00217760(0, 0);
    // match: pressed is read through the union below, so that the reads alias the stores
    stick = (report->left_stick_y > 0x3FU) ? 0 : 0x10;
    if (report->left_stick_y > 0xC0) {
        stick |= 0x40;
    }
    stick_pressed = stick & ~D_00446E8C[0];
    D_00446E8C[0] = stick;
    if (D_004473D8) {
        D_004473C8--;
        if (D_004473C8 <= 0) {
            D_00446E8C[0] = 0xFFFF;
            return D_004473D4 + 1;
        }
        brightness = D_004473C8 / 30.0f;
    } else if (((PadReportUnion *)report)->pressed.buttons & 0x4008) {
        D_004473D8 = 1;
        D_004473C8 = 30;
        D_005FAE80[D_004473D4].track = &D_0042C3D8;
    } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & 0x50) {
        D_0042C348[D_004473D4].length = 0;
        if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & 0x10) {
            D_004473D4--;
        } else if ((stick_pressed | ((PadReportUnion *)report)->pressed.buttons) & 0x40) {
            D_004473D4++;
        }
        D_004473D4 &= 3;
        D_005FAE80[D_004473D4].frame = 60;
        D_0042C348[D_004473D4].length = 120;
    }
    func_00209A48(brightness, &D_005FADA0, 2);
    func_00209A48(brightness, &D_005FADC0, 2);
    func_00209A48(brightness, &D_005FADE0, 2);
    func_00209A48(brightness, &D_005FAE00, 2);
    func_00209A48(brightness, &D_005FAE20, 2);
    for (i = 0; i < 4; i++) {
        func_00209A48(brightness, &D_005FAE80[i], 0);
    }
    func_00209A48(brightness, &D_005FAE60, 0);
    return 0;
}
