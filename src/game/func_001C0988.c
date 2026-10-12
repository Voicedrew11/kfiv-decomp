#include "kf_player.h"

void func_00216788(void *, void *);
extern s32 D_0044717C;
extern s32 D_00447180;
extern s32 D_00447188;
extern WeaponTrailKind D_00414660[7];
extern Vector4 D_0057DB40[64];
extern s32 D_0057DF40[64];

/* queues a weapon trail point; it lives as long as the current kind says */
void func_001C0988(void *position) {
    if (D_0044717C + 1 <= 64) {
        func_00216788(&D_0057DB40[D_00447180], position);
        D_0057DF40[D_00447180] = D_00414660[D_00447188].lifetime;
        D_00447180 = (D_00447180 + 1) % 64;
        D_0044717C++;
    }
}
