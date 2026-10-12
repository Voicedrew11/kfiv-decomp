#include "kf_game.h"
#include "kf_splash.h"

extern BootSprite D_005FADA0;
extern BootSprite D_005FADC0;
extern BootSprite D_005FADE0;
extern BootSprite D_005FAE00;
extern BootSprite D_005FAE20;
extern BootSprite D_005FAE40;
extern BootSprite D_005FAE60;
extern int (*D_004473C4)(void);
extern int D_004473C8;
extern s8 D_004473D0;

int func_00209A48(float, void *, int);
void func_00209E48(void);
void func_00209E78(void);
int func_00209EE8(void);
int func_0020A290(void);
int func_0020A490(void);
void func_00217008(void);
void *func_00217760(int, int);

/* One frame of the second boot screen: it fades in over 30 frames and out over the last 30, then the
   first screen returns; the start button goes on to the third. */
int func_0020A108(void) {
    PadReport *pad;
    f32 brightness;

    D_004473C8--;
    func_00217008();
    brightness = 1.0f;
    pad = func_00217760(0, 0);
    if (D_004473C8 > 3570) {
        brightness = (3600.0f - D_004473C8) / 30.0f;
    }
    if (D_004473C8 <= 30) {
        D_004473D0 = 1;
        brightness = D_004473C8 / 30.0f;
        if (D_004473C8 <= 0) {
            D_004473C4 = func_00209EE8;
            func_00209E48();
            func_00209E78();
            return 0;
        }
    }
    func_00209A48(brightness, &D_005FADA0, 2);
    func_00209A48(brightness, &D_005FADC0, 2);
    func_00209A48(brightness, &D_005FADE0, 2);
    func_00209A48(brightness, &D_005FAE40, 0);
    func_00209A48(brightness, &D_005FAE60, 0);
    func_00209A48(brightness, &D_005FAE00, 2);
    func_00209A48(brightness, &D_005FAE20, 2);
    if (D_004473D0 == 0) {
        if (pad->pressed & 8) {
            D_004473C4 = func_0020A490;
            func_0020A290();
        }
    }
    return 0;
}
