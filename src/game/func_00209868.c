#include "kf_splash.h"
#include <eeregs.h>
#include <libgraph.h>

extern BootTexture D_005FAB60[13];

void FlushCache(int); // match: void here, an int result changes the register after the second call
void func_0020D9A8(void);

/* Sends a texture of the boot screens to GS memory, and its palette when it has 8-bit pixels. */
int func_00209868(int index) {
    sceGsLoadImage load_image;
    int width;
    int height;

    width = 1 << D_005FAB60[index].tex0.tw;
    height = 1 << D_005FAB60[index].tex0.th;
    sceGsSetDefLoadImage(&load_image, D_005FAB60[index].tex0.tbp0, D_005FAB60[index].tex0.tbw, D_005FAB60[index].tex0.psm, 0, 0, width, height);
    FlushCache(0);
    sceGsExecLoadImage(&load_image, (char *)D_005FAB60[index].file + sizeof(TextureHeader));
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
    }
    if (D_005FAB60[index].tex0.psm == 0x13) {
        sceGsSetDefLoadImage(&load_image, D_005FAB60[index].tex0.cbp, 1, D_005FAB60[index].tex0.cpsm, 0, 0, 16, 16);
        FlushCache(0);
        sceGsExecLoadImage(&load_image, (char *)D_005FAB60[index].file + (D_005FAB60[index].file->image_size + sizeof(TextureHeader)));
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00)) {
        }
    }
    func_0020D9A8();
    return 0;
}
