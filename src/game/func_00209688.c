#include "kf_splash.h"
#include <libgraph.h>

extern sceGsDBuff D_0042C4C0;
extern char D_00441140[];
extern char D_00441150[];
extern char D_00441168[];
extern char D_00441180[];
extern char D_00441198[];
extern char D_004411B0[];
extern char D_004411C8[];
extern char D_004411D8[];
extern char D_004411F0[];
extern char D_00441208[];
extern char D_00441220[];
extern char D_00441238[];
extern char D_00441250[];
extern char *D_00446F50;
extern TextureHeader *D_004473B4;

void func_002097E0(char *, int);
int func_00209D68(void); // match: an int result keeps $2 busy, so the pointer below goes through $3
void func_0020D9A8(void);

/* Sets up the display for the boot screens and loads their thirteen textures. */
int func_00209688(void) {
    sceGsSyncV(0);
    sceGsResetGraph(0, 1, 2, 1);
    sceGsSetDefDBuff(&D_0042C4C0, 0, 640, 224, 3, 0, 1);
    D_0042C4C0.clear1.rgbaq = D_0042C4C0.clear0.rgbaq = 0x180000000;
    func_00209D68();
    D_004473B4 = (TextureHeader *)D_00446F50;
    func_002097E0(D_00441140, 0);
    func_002097E0(D_00441150, 1);
    func_002097E0(D_00441168, 2);
    func_002097E0(D_00441180, 3);
    func_002097E0(D_00441198, 4);
    func_002097E0(D_004411B0, 5);
    func_002097E0(D_004411C8, 6);
    func_002097E0(D_004411D8, 7);
    func_002097E0(D_004411F0, 8);
    func_002097E0(D_00441208, 9);
    func_002097E0(D_00441220, 10);
    func_002097E0(D_00441238, 11);
    func_002097E0(D_00441250, 12);
    func_0020D9A8();
    return 0;
}
