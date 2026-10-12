#include "kf_model.h"

int func_001C8B58(int, int);
extern u8 *D_004469C8[2];
extern s32 D_00447190[2];
extern ItemNode D_0057E240[2][64];

/* Rolls which of a map's fixed items appear: one roll per group against the map's threshold
   for it, then one per item against its own chance. */
void func_001C87D8(int map) {
    u8 rolls[16];
    u8 *thresholds = D_004469C8[map];
    s32 i;
    ItemNode *node;

    for (i = 0; i < 16; i++) {
        rolls[i] = rand() * 100.0f * 4.656613e-10f;
    }
    node = D_0057E240[map];
    for (i = 0; i < D_00447190[map]; i++, node++) {
        if (rolls[node->def->group] < thresholds[node->def->group]) {
            if ((s32)(rand() * 100.0f * 4.656613e-10f) < node->def->chance) {
                func_001C8B58(map, i);
            }
        }
    }
}
