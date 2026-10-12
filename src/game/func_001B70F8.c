#include "kf_effect.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern LevelRecord D_003679A0[];
extern FxTableEntry D_00402DA8[];
extern ModelInstance *D_00446978;
extern s32 D_00447114;              /* frames the magic cast by the swing still has */
extern s32 D_00447118;              /* that magic's number */
extern u16 D_0044693C;
extern u16 D_0044693E;
extern u16 D_00446940;

void *memset(void *, int, u32);
void func_00108A08(ModelInstance *, int);
void func_00108AC8(void *);
void func_0019CE00(int mode);
void func_001A3918(int);
s32 func_001B1470(Unk1B1470Request *, Unk1B1470Extra *);
int func_001BC9E0(int);
void func_001C06E8(void);
void func_00216778(void *, void *);

/* Handler of a weapon that is swung: the button starts the swing; while its motion runs the
   swing hits what it reaches, plays its sound and casts the weapon's two magics when they were
   asked for in time. */
void func_001B70F8(u16 held, u16 pressed) {
    Unk1B1470Request request;
    LevelRecord *record = &D_003679A0[D_00413F80.unk3C];
    PlayerItemDef *def = &D_0041B9B8[D_00413F80.unk3C];
    s32 level = record->unk14;
    PlayerShot *shot;

    if (D_00413F80.unk258 == 0) {
        if (def->unk42 != 0 && def->unk42 != 0xFFFF && D_00413F80.unkAC >= def->unk8A && D_00413F80.unkB0 >= def->unk8C) {
            if (D_00413F80.unk25C != 0) {
                D_00413F80.unk25C--;
            }
            if (D_00446940 & pressed) {
                D_00413F80.unk25C = 15;
            }
        } else {
            D_00413F80.unk25C = 0;
        }
        D_00413F80.unk25D = level >= 2 && def->unk52 != 0 && def->unk52 != 0xFFFF;
        if (D_0044693C & pressed) {
            shot = &D_00413F80.unk268;
            memset(shot, 0, sizeof(PlayerShot));
            shot->unk00 = 0;
            shot->unk20 = D_00413F80.unkAC;
            shot->unk22 = D_00413F80.unkB0;
            shot->unk02 = D_00413F80.unkBC;
            shot->unk24 = D_00413F80.unk3C;
            func_00108A08(D_00446978, 0);
            D_00413F80.unk260_0 = 0;
            D_00413F80.unk260_1 = 0;
            D_00413F80.unk260_2 = 0;
            D_00413F80.unk258 = 1;
            D_00413F80.unkC4 = def->unk7E;
            D_00447114 = 0;
            D_00413F80.unkBC = 0;
            D_00413F80.unk25B = 0;
            D_00413F80.unk25A = 0;
            D_00413F80.unk261 = 0;
            D_00413F80.unk262 = 0;
            D_00413F80.unk25E = 0;
            D_00413F80.unk25F = 0;
            D_00413F80.unk263 = 0;
            func_001C06E8();
        }
        return;
    }
    if (D_00413F80.unk261 == 0) {
        if (D_00413F80.unk263 != 0) {
            D_00447114 = 0;
        }
        if (D_00447114 <= 0) {
            func_00108AC8(D_00446978);
        } else {
            D_00447114--;
            if (((s32)(D_00413F80.unk1C8 >> 39) & 1) ||
                (((s32)(D_00413F80.unk1C8 >> 38) & 1) &&
                 D_00413F80.position.y - D_00402DA8[D_00447118].unk108 > D_00413F80.unk1DC)) {
                func_001A3918(D_00447118);
                D_00447114 = 0;
            }
        }
        if (MODEL_CHANNEL(D_00446978)->param_shifted <= 0) {
            D_00413F80.unk258 = 0;
            return;
        }
        if (D_00413F80.unk263 != 0) {
            return;
        }
        if (MODEL_CHANNEL(D_00446978)->param >= def->unk78 && MODEL_CHANNEL(D_00446978)->param <= def->unk7A && D_00447114 == 0) {
            memset(&request, 0, sizeof(request));
            func_00216778(request.position, &D_00413F80);
            request.owner = (Entity *)&D_00413F80;
            request.unk4C_0 = 3;
            request.position[1] -= 220.0f;
            request.unk30 = def->unk74 + 100.0f;
            request.unk34 = 230.0f;
            request.unk38 = D_00413F80.rotation.y;
            request.unk3C = def->unk70 * 0.5f * 0.017453292f;
            request.unk40 = D_00413F80.rotation.x;
            request.unk44 = def->unk6C * 0.5f * 0.017453292f;
            request.unk4D = 0x3E;
            *(PlayerShotStats *)&request.unk50 = D_00413F80.unk60;
            *(PlayerItemRates *)&request.unk62 = D_00413F80.unk72;
            request.unk6E = def->unk7F;
            request.unk6F = def->unk86;
            request.unk78 = D_00413F80.position.y - 230.0f - 50.0f;
            request.unk7C = D_00413F80.position.y + 50.0f;
            func_001B1470(&request, (Unk1B1470Extra *)&D_00413F80.unk268);
        }
        if (!D_00413F80.unk260_0 && def->unk80 != 0xFFFF && MODEL_CHANNEL(D_00446978)->param >= def->unk82) {
            ((int (*)(int))func_0019CE00)(def->unk80); // match: called as if it returned a value
            D_00413F80.unk260_0 = 1;
        }
        if (D_0044693E & pressed) {
            if (D_00413F80.unk25C != 0 && D_00413F80.unk25F == 0 && def->unk42 != 0xFFFF &&
                MODEL_CHANNEL(D_00446978)->param >= def->unk4A && MODEL_CHANNEL(D_00446978)->param <= def->unk4C) {
                D_00413F80.unk25D = 0;
                D_00413F80.unk25B = 1;
                D_00413F80.unk25F = 1;
            }
            if (D_00413F80.unk25D != 0 && D_00413F80.unk25A == 0 && def->unk52 != 0xFFFF &&
                MODEL_CHANNEL(D_00446978)->param >= def->unk5A && MODEL_CHANNEL(D_00446978)->param <= def->unk5C) {
                D_00413F80.unk25A = 1;
                D_00413F80.unk25E = 1;
            }
            D_00413F80.unk25C = 0;
            D_00413F80.unk25D = 0;
        }
        if (D_00413F80.unk25F == 1 && MODEL_CHANNEL(D_00446978)->param >= def->unk44) {
            D_00413F80.unk25F = 2;
            if (func_001BC9E0(def->unk42)) {
                D_00447118 = def->unk42;
                D_00447114 = def->unk4E;
            }
        }
        if (D_00413F80.unk25E == 1 && MODEL_CHANNEL(D_00446978)->param >= def->unk54) {
            D_00413F80.unk25E = 2;
            if (func_001BC9E0(def->unk52)) {
                D_00447118 = def->unk52;
                D_00447114 = def->unk5E;
            }
        }
        if (D_00413F80.unk25F != 0 && !D_00413F80.unk260_1 && def->unk46 != 0xFFFF &&
            MODEL_CHANNEL(D_00446978)->param >= def->unk48) {
            D_00413F80.unk260_1 = 1;
        }
        if (D_00413F80.unk25E != 0 && !D_00413F80.unk260_2 && def->unk56 != 0xFFFF &&
            MODEL_CHANNEL(D_00446978)->param >= def->unk58) {
            D_00413F80.unk260_1 = 1; // match: bit 1 again, though bit 2 was tested
        }
    } else {
        func_00108AC8(D_00446978);
        if (MODEL_CHANNEL(D_00446978)->value == 0) {
            D_00413F80.unk258 = 0;
        }
    }
}
