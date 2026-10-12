#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern u8 D_00369830[];
extern ModelInstance *D_00446978;
extern PlayerShot *D_0044711C;
extern int D_00413F30[];
extern float D_00413F40[];
extern u16 D_0044693C;

void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void *func_001B1390(void);
int func_001B4F30(u16, void *, void *, void *, void *);

/* Handler of an item that shoots: on the button, starts the motion and launches the item's shot. */
void func_001B77E0(u16 held, u16 pressed) {
    PlayerItemDef *def = &D_0041B9B8[D_00413F80.unk3C];

    if ((D_0044693C & pressed) && !((s32)(D_00413F80.unk1C8 >> 39) & 1) && D_00369830[def->unk62] != 0) {
        func_00108A08(D_00446978, 0);
        MODEL_CHANNEL(D_00446978)->param = 0;
        MODEL_CHANNEL(D_00446978)->param_shifted = 0;
        func_00108AC8(D_00446978);
        ((int (*)(ModelInstance *))D_00446978->unk1B4)(D_00446978); // match: called as if it returned a value
        D_00413F80.unk260_0 = 0;
        D_00413F80.unk298 = 0;
        D_00413F80.unk258++;
        D_00413F80.unk263 = 0;
        D_0044711C = func_001B1390();
        D_0044711C->unk00 = 0;
        D_0044711C->unk26 = def->unk52;
        D_0044711C->unk20 = D_00413F80.unkAC;
        D_0044711C->unk22 = D_00413F80.unkB0;
        D_0044711C->unk02 = D_00413F80.unkBC;
        func_001B4F30(def->unk52, D_0044711C, D_00413F30, D_00413F40, &D_00413F80);
    }
}
