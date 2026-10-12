#include "kf_model.h"
#include "kf_player.h"
#include "kf_splash.h"

extern LevelRecord D_003679A0[];
extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern void (*D_0042C190[4])(void);
extern int D_00447388;
extern int D_00447390;
extern ModelInstance *D_0044739C;
extern ItemUseEffectDef *D_004473A4;
extern int D_004473AC;
extern int D_004473B0;

void *memset(void *, int, u32);
void func_0010B5D0(ModelPart *);
int func_001A1C58(int, void *, void *, void *);
int func_001BF6E8(void);
void func_001CFA70(int, int, int, int, int, int, ...);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);

/* One frame of the item effect func_00209018 started: runs its current step; once the motion reaches
   the kind's frame, starts its two effects and sets the player's unkC0; then poses the model. */
void func_00209218(void) {
    Vector4 origin;
    PlayerItemDef *def;
    ModelPart *part;
    int count;
    int i;

    memset(&origin, 0, sizeof(origin));
    origin.w = 1.0f;
    if (D_00447388 == 0) {
        return;
    }
    D_0042C190[D_00447390]();
    if (D_00447388 == 0) {
        return;
    }
    if (D_004473AC != 0 && D_0044739C != NULL && MODEL_CHANNEL(D_0044739C)->param >= D_004473A4->unk08) {
        func_001A1C58(D_004473A4->unk0C, &origin, &origin, D_0044739C);
        func_001A1C58(0x26A, &D_00413F80.position, &D_00413F80.rotation, D_0044739C);
        def = &D_0041B9B8[D_00413F80.unk3C];
        D_004473AC = 0;
        if (D_00413F80.unk3D != 0 || def->unk42 != 0xFFFF || (def->unk52 != 0xFFFF && D_003679A0[D_00413F80.unk3C].unk14 + 1 >= 3) ||
            func_001BF6E8() != 0) {
            D_00413F80.unkC0 = 5000;
        }
        if (D_004473B0 != 0) {
            func_001CFA70(0, 0, 0, 0, 0x2D, 0x29);
            D_004473B0 = 0;
        }
    }
    func_0020C300(D_0044739C, &D_0044739C->position, &D_0044739C->rotation, &D_0044739C->unkE0);
    count = D_0044739C->part_count;
    part = D_0044739C->parts;
    for (i = 0; i < count; i++, part++) {
        func_0020B8B0(D_0044739C, part, part);
    }
    part = D_0044739C->parts;
    for (i = 0; i < count; i++, part++) {
        func_0010B5D0(part);
    }
}
