#include "kf_entity.h"
#include "kf_game.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern GameDefinition D_00416D78[];
extern u8 D_00369830[];
extern ModelInstance *D_00446978;
extern PlayerShot *D_0044711C;
extern s32 D_00447120;
extern Unk1B1470Request D_0057A350[32];
extern u16 D_0044693C;

void *memset(void *, s32, u32);
void func_00108AC8(void *);
int func_00109098(ModelInstance *model, u8 index, u16 speed);
void func_0019CDA0(s8 slot);
void func_0019CE00(int mode);
int func_001B7D80(void);

#define SHOT_REQUEST ((Unk1B1470Request *)D_0044711C->unk04)

/* Handler of a drawn weapon while the button is held: the draw motion runs to its last frame and the
   strength builds up; letting go looses the shot, anything else lowers the weapon. */
void func_001B7918(u16 held, u16 pressed) {
    GameDefinition *item = &D_00416D78[D_00413F80.unk3C];
    PlayerItemDef *def = &D_0041B9B8[D_00413F80.unk3C];
    s32 rate;
    s32 frame;
    s32 speed;

    if (MODEL_CHANNEL(D_00446978)->param != 0 && MODEL_CHANNEL(D_00446978)->param < def->unk7A) {
        func_00108AC8(D_00446978);
    }
    if (D_00413F80.unk298 == 0) {
        D_00413F80.unk298 = MODEL_CHANNEL(D_00446978)->param >= def->unk78;
    }
    if (D_00413F80.unk263 == 0 && !((s32)(D_00413F80.unk1C8 >> 39) & 1) && func_001B7D80()) {
        if (!D_00413F80.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(D_00446978)->param >= def->unk82) {
            ((int (*)(int))func_0019CE00)(0); // match: called as if it returned a value
            D_00413F80.unk260_0 = 1;
        }
        if (D_0044693C & held) {
            rate = D_00413F80.unkAC;
            if (rate < 20) {
                rate = 20;
            }
            D_00413F80.unkBC += rate * 42 / (item->unk33 + 1);
            if (D_00413F80.unkBC > 5000) {
                D_00413F80.unkBC = 5000;
            }
            return;
        }
        if (D_00413F80.unk298 != 0) {
            if (D_00413F80.unk260_0) {
                func_0019CDA0(2);
            }
            D_0044711C->unk02 = D_00413F80.unkBC;
            D_0044711C->unk24 = D_00413F80.unk3C;
            D_0044711C->unk04 = &D_0057A350[D_00447120];
            D_00447120 = (D_00447120 + 1) % 32;
            memset(D_0044711C->unk04, 0, sizeof(Unk1B1470Request));
            *(PlayerShotStats *)&SHOT_REQUEST->unk50 = D_00413F80.unk60;
            *(PlayerItemRates *)&SHOT_REQUEST->unk62 = D_00413F80.unk72;
            SHOT_REQUEST->unk6E = def->unk7F;
            SHOT_REQUEST->unk6F = def->unk86;
            if (MODEL_CHANNEL(D_00446978)->param != 0) {
                frame = MODEL_CHANNEL(D_00446978)->param;
            } else {
                frame = def->unk7A;
            }
            speed = (frame - def->unk78) * 3 / (def->unk7A - def->unk78);
            if (speed <= 0) {
                speed = 1;
            }
            func_00109098(D_00446978, 1, speed);
            MODEL_CHANNEL(D_00446978)->unk8F = 0;
            D_00413F80.unk258++;
            D_00369830[def->unk62]--;
            return;
        }
    }
    func_00109098(D_00446978, 0, 0xF);
    MODEL_CHANNEL(D_00446978)->unk8F = 0;
    D_00413F80.unk298 = 0;
    D_00413F80.unkBC = 0;
    D_00413F80.unk258 += 3;
}
