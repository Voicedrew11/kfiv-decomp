#include "kf_splash.h"

extern BootSprite D_005FADA0;
extern BootSprite D_005FADC0;
extern BootSprite D_005FADE0;
extern BootSprite D_005FAE00;
extern BootSprite D_005FAE20;
extern BootSprite D_005FAE60;
extern BootSprite D_005FAE80[4];
extern BootSpriteTrack D_0042C230;
extern BootSpriteTrack D_0042C258;
extern BootSpriteTrack D_0042C280;
extern BootSpriteTrack D_0042C2A8;
extern BootSpriteTrack D_0042C2D0;
extern BootSpriteTrack D_0042C320;
extern BootSpriteTrack D_0042C348[4];
extern int D_004473D4;
extern s8 D_004473D8;

void func_00209A28(int, int, int, int, int, void *, void *);
void func_00209DE0(void);
s32 func_001E4068(void);
void func_00217008(void);
void *func_00217760(int, int);

/* Sets up the sprites of the third boot screen: four choices in a column, the one func_001E4068 returns pulsing. */
int func_0020A290(void) {
    int i;

    func_00209A28(7, -0x100, -0xC0, 0x100, 0x100, &D_005FAE00, &D_0042C2A8);
    func_00209A28(8, 0, -0xC0, 0x100, 0x100, &D_005FAE20, &D_0042C2D0);
    func_00209A28(2, -0x80, 0x20, 0x100, 0x40, &D_005FAE80[0], &D_0042C348[0]);
    func_00209A28(3, -0x80, 0x40, 0x100, 0x40, &D_005FAE80[1], &D_0042C348[1]);
    func_00209A28(4, -0x80, 0x60, 0x100, 0x40, &D_005FAE80[2], &D_0042C348[2]);
    func_00209A28(5, -0x80, 0x80, 0x100, 0x40, &D_005FAE80[3], &D_0042C348[3]);
    func_00209A28(6, -0x140, 0xB4, 0x280, 0x20, &D_005FAE60, &D_0042C320);
    func_00209A28(9, -0x140, -0xE0, 0x100, 0x200, &D_005FADA0, &D_0042C230);
    func_00209A28(10, -0x40, -0xE0, 0x100, 0x200, &D_005FADC0, &D_0042C258);
    func_00209A28(11, 0xC0, -0xE0, 0x80, 0x200, &D_005FADE0, &D_0042C280);
    D_004473D4 = func_001E4068();
    D_004473D8 = 0;
    for (i = 0; i < 4; i++) {
        if (D_004473D4 == i) {
            D_0042C348[i].length = 120;
        } else {
            D_0042C348[i].length = 0;
        }
    }
    func_00209DE0();
    func_00217008();
    func_00217760(0, 0);
    return 0;
}
