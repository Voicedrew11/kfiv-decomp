#include "kf_splash.h"

extern BootTexture D_005FAB60[13];
extern TextureHeader *D_004473B4;

int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);

/* Loads a texture file of the boot screens to the free memory at D_004473B4. */
void func_002097E0(char *name, int index) {
    int file;
    int size;

    file = func_00102238((int)name, 0, 1);
    size = func_001025C0(file, D_004473B4, -1);
    func_001024B0(file);
    D_005FAB60[index].file = D_004473B4;
    D_005FAB60[index].tex0 = *(BootTex0 *)&D_004473B4->tex0;
    D_004473B4 = (TextureHeader *)((char *)D_004473B4 + size);
}
