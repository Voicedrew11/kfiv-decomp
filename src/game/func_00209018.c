#include "kf_game.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_splash.h"

extern char D_0038A440[];
extern PlayerState D_00413F80;
extern EventRunner D_00420D00;
extern ItemUseEffectDef D_0042BBE0[];
extern char D_0042C110[];
extern char *D_005F9B40[7];
extern int D_00446770;
extern int D_00447388;
extern int D_0044738C;
extern int D_00447390;
extern ModelInstance *D_00447394;
extern ModelInstance *D_00447398;
extern ModelInstance *D_0044739C;
extern ItemUseEffectDef *D_004473A4;
extern int D_004473A8;
extern int D_004473AC;
extern int D_004473B0;

void KF_Vector_Scale(float *, float *, float);
void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void func_0010DB40(ModelInstance *);
void func_0010E8E0(void *, int, int, int, int);
void func_002095D0(ModelInstance *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);

/* Starts the effect of using item `kind` (0x46 to 0x52): one of two models, with the texture of the kind,
   is put 75 units ahead of the camera, turned like the player, and its motion started. */
void func_00209018(int kind, int marked) {
    ModelInstance *model;
    int i;

    if (D_00447388 != 0) {
        return;
    }
    if ((u32)(kind - 0x46) >= 13) {
        return;
    }
    D_004473AC = D_00447388 = 1;
    D_004473B0 = marked;
    D_004473A4 = &D_0042BBE0[kind];
    D_0044738C = kind;
    D_00447390 = 0;
    if (kind >= 0x4D) {
        model = D_00447398;
        model->unk190 = 2;
        model->unk194 = (int)D_005F9B40[kind - 0x4C];
        D_0044739C = model;
    } else {
        model = D_00447394;
        model->unk190 = 2;
        model->unk194 = (int)D_005F9B40[kind - 0x46];
        D_0044739C = model;
    }
    D_0044739C->unk1A8 = func_0010DB40;
    D_0044739C->draw = func_002095D0;
    func_00108A08(D_0044739C, 0);
    func_00108AC8(D_0044739C);
    D_004473A8 = 0x80;
    func_0010E8E0(D_0044739C, 0x80, 0x80, 0x80, D_004473A8);
    KF_Vector_Scale((float *)&D_0044739C->position, D_00420D00.unk10, 75.0f);
    func_00216C48(&D_0044739C->position, &D_0044739C->position, D_00420D00.unk00);
    func_00216778(&D_0044739C->rotation, &D_00413F80.rotation);
    D_0044739C->unkE0 = D_0044739C->unkE4 = D_0044739C->unkE8 = 1.0f;
    D_0044739C->unkEC = 1.0f;
    D_0044739C->unk148 = (s32)D_0042C110;
    D_0044739C->unk14C = &D_0038A440[D_00446770 * 0x400];
    for (i = 0; i < D_0044739C->part_count; i++) {
        if (i == 0) {
            MODEL_PART_FLAG_BITS(&D_0044739C->parts[0])->bit41 = TRUE;
        } else {
            MODEL_PART_FLAG_BITS(&D_0044739C->parts[i])->bit41 = FALSE;
        }
    }
}
