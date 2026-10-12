#include "kf_splash.h"

extern BootSprite D_005FADA0;
extern BootSprite D_005FADC0;
extern BootSprite D_005FADE0;
extern BootSprite D_005FAE00;
extern BootSprite D_005FAE20;
extern BootSprite D_005FAE40;
extern BootSprite D_005FAE60;
extern BootSpriteTrack D_0042C230;
extern BootSpriteTrack D_0042C258;
extern BootSpriteTrack D_0042C280;
extern BootSpriteTrack D_0042C2A8;
extern BootSpriteTrack D_0042C2D0;
extern BootSpriteTrack D_0042C2F8;
extern BootSpriteTrack D_0042C320;
extern int D_004473C8;
extern s8 D_004473D0;

void func_00209A28(int, int, int, int, int, void *, void *);
void func_00209DE0(void);

/* Sets up the seven sprites of the second boot screen, which stays for 3600 frames. */
int func_00209FC8(void) {
    D_004473C8 = 3600;
    func_00209A28(1, -0x80, 0x40, 0x100, 0x40, &D_005FAE40, &D_0042C2F8);
    func_00209A28(6, -0x140, 0xB4, 0x280, 0x20, &D_005FAE60, &D_0042C320);
    func_00209A28(7, -0x100, -0xC0, 0x100, 0x100, &D_005FAE00, &D_0042C2A8);
    func_00209A28(8, 0, -0xC0, 0x100, 0x100, &D_005FAE20, &D_0042C2D0);
    func_00209A28(9, -0x140, -0xE0, 0x100, 0x200, &D_005FADA0, &D_0042C230);
    func_00209A28(10, -0x40, -0xE0, 0x100, 0x200, &D_005FADC0, &D_0042C258);
    func_00209A28(11, 0xC0, -0xE0, 0x80, 0x200, &D_005FADE0, &D_0042C280);
    D_004473D0 = 0;
    func_00209DE0();
    return 0;
}
