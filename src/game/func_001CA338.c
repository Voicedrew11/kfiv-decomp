#include "kf_model.h"

void *memset(void *, int, u32);
extern MapItemDef *D_004469B8[2];
extern s16 *D_004469C0[2];
extern s32 D_00447190[2];
extern char D_0057E040[2][0x100];
extern ItemNode D_0057E240[2][64];
extern ItemNode D_00580040[2][16];
extern ItemNode D_005807C0[2][32];
extern ModelInstance D_005816C0[2][64];
extern ModelInstance D_0058F6C0[2][16];
extern ModelInstance D_00592EC0[2][32];
extern s8 D_005B5EC0[2][0x100];

/* Clears everything the item code keeps for a map slot. */
void func_001CA338(int map) {
    int i;

    D_00447190[map] = 0; // match: this order of the three gives the retail register for each table
    D_004469B8[map] = NULL;
    D_004469C0[map] = NULL;
    memset(D_0057E040[map], 0, sizeof(D_0057E040[0]));
    memset(D_0057E240[map], 0, sizeof(D_0057E240[0]));
    memset(D_00580040[map], 0, sizeof(D_00580040[0]));
    memset(D_005807C0[map], 0, sizeof(D_005807C0[0]));
    memset(D_005816C0[map], 0, sizeof(D_005816C0[0]));
    memset(D_0058F6C0[map], 0, sizeof(D_0058F6C0[0]));
    memset(D_00592EC0[map], 0, sizeof(D_00592EC0[0]));
    for (i = 0xFF; i >= 0; i--) {
        D_005B5EC0[map][i] = -1;
    }
}
