#include "kf_splash.h"
#include "kf_gs.h"

int sceGsSyncV(int);
int sceGsResetGraph(int, int, int, int);
void sceGsSetDefDBuff(void *, s16, s16, s16, s16, s16, s16);
void func_00209D68(void);
void func_002097E0(char *, int);
void func_0020D9A8(void);
void func_00209A28(int, int, int, int, int, void *, void *);
void func_00217008(void);
void *func_00217760(int, int);

extern GsDoubleBuffer D_0042C4C0;
extern BootSpriteTrack D_0042C400[2];
extern BootSpriteTrack D_0042C470;
extern BootSpriteTrack D_0042C498;
extern BootSprite D_005FAEF0[2];
extern BootSprite D_005FAF28;
extern BootSprite D_005FAF48;
extern char D_00441280[];
extern char D_00441298[];
extern char D_004412B0[];
extern char D_004412C8[];
extern char *D_00446F50;
extern TextureHeader *D_004473B4;
extern int D_004473DC;
extern s8 D_004473E0;

/* Sets up the two-choice boot screen: its frame buffers, its four textures and its sprites. */
int func_0020A710(void) {
    int i;

    sceGsSyncV(0);
    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuff(&D_0042C4C0, 0, 0x280, 0xE0, 3, 0, 1);
    D_0042C4C0.clear1.rgbaq = D_0042C4C0.clear0.rgbaq = 0x180000000L;
    ((int (*)(void))func_00209D68)(); // match: called as if it returned a value, which keeps $2 busy
    D_004473B4 = (TextureHeader *)D_00446F50;
    func_002097E0(D_00441280, 0x10);
    func_002097E0(D_00441298, 0x11);
    func_002097E0(D_004412B0, 0x12);
    func_002097E0(D_004412C8, 0x16);
    func_0020D9A8();
    D_004473DC = 0;
    D_004473E0 = 0;
    func_00209A28(0x10, -0x80, -0x30, 0x100, 0x40, &D_005FAEF0[0], &D_0042C400[0]);
    func_00209A28(0x11, -0x80, -0x10, 0x100, 0x40, &D_005FAEF0[1], &D_0042C400[1]);
    func_00209A28(0x12, -0x40, 0x94, 0x80, 0x40, &D_005FAF28, &D_0042C470);
    func_00209A28(0x16, -0x80, -0x80, 0x100, 0x100, &D_005FAF48, &D_0042C498);
    for (i = 0; i < 2; i++) {
        if (D_004473DC == i) {
            D_0042C400[i].length = 0x78;
        } else {
            D_0042C400[i].length = 0;
        }
    }
    func_00217008();
    func_00217760(0, 0);
    return 0;
}
