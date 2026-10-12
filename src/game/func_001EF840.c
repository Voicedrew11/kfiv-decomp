#include "kf_link.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_state.h"

extern LevelRecord D_003679A0[70];
extern LevelRecord D_003682F0[160];
extern u8 D_00413FBC[];
extern LevelDef D_004147F8[380];
extern PlayerItemDef D_0041B9B8[];
extern LevelRecordDef D_0041E118[];
extern f32 D_00425510[0x100];       /* one alpha per menu part; [253] the cursor bar's, [254] and [255] the texts' */
extern f32 D_00425910[0x100];       /* one alpha per part of the selected item's panel */
extern MenuItem *D_00426110[4];
extern MenuItem *D_00426120[4];
extern MenuItem *D_00426130[4];
extern f32 D_00428A30[4];
extern f32 D_0042B7D0[16];
extern const TradeGroupBases D_00440330;
extern u32 D_00446A94;
extern u32 D_00446A98;
extern u32 D_00446A9C;
extern u32 D_00446AA4;              /* how many parts the screen has */
extern s32 D_00446AD4;              /* nonzero: the selected item's panel is shown */
extern SpritePacket *D_00446C0C;
extern SpritePacket *D_00446C10;
extern SpritePacket *D_00446C14;
extern SpritePacket *D_00446C50;
extern MenuColor D_00446C78;
extern MenuColor D_00446C88;
extern LinkNode *D_00446C90;
extern void (*D_00446CC4)(HandlerContext *);
extern s32 D_00446CD4;
extern f32 D_00446CD8;
extern u16 D_00446DE8;
extern s32 D_00446DF4;
extern s32 D_00446DF8;
extern s32 D_00446DFC;
extern s32 D_00446E00;
extern s32 D_00446E04;
extern s32 D_00446E08;
extern s32 D_00446E0C;
extern s32 D_00446E10;
extern s32 D_00446E14;
extern s32 D_00446E1C;
extern s32 D_00446E20;
extern s32 D_00446E24;
extern s32 D_00446E28;
extern s32 D_00446E2C;
extern s32 D_00446E30;
extern s32 D_00446E34;
extern s32 D_00446E3C;
extern s32 D_00446F38;
extern s8 D_00447313;
extern s8 D_00447316;               /* 0: fading in, 1: open, 2 and 3: waiting for the models, 4 and 5: fading out */
extern s8 D_00447317;               /* the same for the selected item's panel */
extern s8 D_00447318;

void *memset(void *, int, u32);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001D45D0(void);
int func_001D6E60(f32 *);
s32 func_001DFC60(HandlerContext *, s32);
int func_001E4FE0(s32, u32);
void func_001E5210(s32); // match: defined as int (u8); retail passes unk42 sign-extended and uses no result
f32 func_001E6A20(s32, s32, f32);
void func_001EEF78(HandlerContext *);
void func_001F0D90(HandlerContext *);
void func_001FC5E8(MenuCursorBar *);
void func_001FDCD0(MenuColor);
int func_001FE9A8(LevelDef *, s32);
void func_00200068(s32, s32);
void func_00207478(int, int, int, int);

/* match: the two level lookups are inline functions taking the record index, so the subtraction is not folded into
   the table address; each caller picks the table itself and uses the level inside its own branch. */
static inline u8 GetWeaponLevel(s32 index) {
    return D_003679A0[index].level;
}

static inline u8 GetItemLevel(s32 index) {
    return D_003682F0[index].level;
}

static inline void SetCorner(GsPackedWord *corner, s32 left, s32 top, s32 depth) {
    corner->f.lo16 = left;
    corner->f.mid16 = top;
    corner->f.mid24 = depth;
}

void func_001EF840(HandlerContext *context) {
    TradeGroupBases bases = D_00440330;
    MenuCursorBar bar;
    f32 rotation[16];
    f32 scale[16];
    GsPackedWord word;
    MenuItem *item;
    MenuItem *selected;
    SpritePacket *parts;
    u32 count;
    u32 i;
    s32 row;
    s32 number;
    s32 index;
    s32 table;
    s32 entry;
    s32 variant;
    f32 angle;
    u8 kind;
    u32 full_alpha;
    MenuItem *node;
    s32 line;
    s32 text_index;
    s32 depth;
    LevelDef *def;

    if (D_00426110[0] == NULL) {
        D_00446CC4 = func_001EEF78;
        return;
    }
    if (D_00447316 < 2) {
        func_001DFC60(context, D_00447318);
    }
    number = D_00426110[0]->unk0C;
    index = number - bases.first_item[D_00426110[0]->group];
    if (D_00426110[0]->group == 0) {
        D_00446E3C = D_003679A0[index].unk14 + 1;
    }
    D_00446E30 = 100;
    if (D_00426110[0]->unk0C < 220) {
        D_00446E34 = GetWeaponLevel(D_00426110[0]->unk0C - 150);
    } else {
        D_00446E34 = GetItemLevel(D_00426110[0]->unk0C - 220);
    }
    for (i = 0; i < D_00446AA4; i++) {
        kind = D_00446C50[i].unk3E;
        func_001E5210((s8)D_00446C50[i].unk42);
        if (kind == 3) {
            switch (D_00446C50[i].slot) {
            case 0:
                func_00207478((int)&D_00446C50[i], 1003, 0, (int)D_00425510[i]);
                break;
            case 1:
                break;
            case 2:
                func_00207478((int)&D_00446C50[i], 1003, 2, (int)D_00425510[i]);
                break;
            case 3:
                func_00207478((int)&D_00446C50[i], 1003, 3, (int)D_00425510[i]);
                break;
            }
        } else {
            func_00207478((int)&D_00446C50[i], kind + 1000, 1, (int)D_00425510[i]);
        }
        func_001E5210(0);
    }
    item = D_00426120[0];
    for (row = 0; row < 7; row++, item = item->prev) {
        if (item == D_00426110[0]) {
            depth = 0x86470;
            bar.first.f.lo16 = 0x6FE0;
            bar.first.f.mid16 = 0x7C30 + row * 0xB0;
            bar.first.f.mid24 = depth;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x1240;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xB0;
            bar.second.f.mid24 = bar.first.f.mid24;
            if (D_00447316 == 1) {
                D_00425510[253] = func_001E6A20(0x10, 0x20, D_00425510[253]);
            }
            bar.alpha = (s32)D_00425510[253];
            func_001FC5E8(&bar);
            break;
        }
    }
    func_001FDCD0(D_00446C78);
    word.raw = 0x0C7A12007C307080;
    for (line = 0, node = D_00426120[0]; line < 7; node = node->prev, line++) {
        def = &D_004147F8[node->unk0C];
        func_001FDB88(word);
        func_001FE9A8(def, 0x1A);
        word.f.mid16 += 0xB0;
        if (D_00426130[0]->prev == node) {
            break;
        }
    }
    memset(scale, 0, 0x40);
    scale[10] = scale[5] = scale[0] = 0.8f;
    scale[15] = 1.0f;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    angle = D_00446CD8 + 3.1415927f;
    D_00446CD8 = angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
    sceVu0RotMatrixY(rotation, scale, D_00446CD8);
    sceVu0TransMatrix(rotation, rotation, D_00428A30);
    func_001D6E60(rotation);
    if ((D_00446DE8 & 0x1000) && D_00447318 == 0 && func_001E4FE0(index, D_00426110[0]->group)) {
        D_00446CD4 = 1;
        if (D_00446AD4 == 0) {
            D_00446AD4 = 1;
        } else {
            D_00447317 = 2;
        }
    }
    if (D_00446AD4 != 0) {
        switch (D_00426110[0]->group) { // match: groups 1 and 10 have no case, so parts and count stay unset for the fade below
        case 11:
        case 12:
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if (D_00446CD4 != 0) {
                func_0019CDC0(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + D_0041E118[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + D_0041E118[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + D_0041E118[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = D_0041E118[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = D_0041E118[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = D_0041E118[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = D_0041E118[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = D_0041E118[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = D_0041E118[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E24 = D_003682F0[index].level;
            D_00446E28 = 100;
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A9C; i++) {
                    full_alpha = D_00446C14[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A9C; i++) {
                func_001E5210((s8)D_00446C14[i].unk42);
                func_00207478((int)&D_00446C14[i], D_00446C14[i].unk3E + 1000, variant, (int)D_00425910[i]);
                func_001E5210(0);
            }
            parts = D_00446C14;
            count = D_00446A9C;
            break;
        case 7:
        case 8:
        case 9:
            if (D_00446CD4 != 0) {
                func_0019CDC0(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003682F0[index].unk00[0] + D_0041E118[index].unk00[0]) * D_003682F0[index].level / 100;
            D_00446DF8 = (D_003682F0[index].unk00[1] + D_0041E118[index].unk00[1]) * D_003682F0[index].level / 100;
            D_00446DFC = (D_003682F0[index].unk00[2] + D_0041E118[index].unk00[2]) * D_003682F0[index].level / 100;
            D_00446E00 = D_0041E118[index].unk00[3] * D_003682F0[index].level / 100;
            D_00446E04 = D_0041E118[index].unk00[4] * D_003682F0[index].level / 100;
            D_00446E08 = D_0041E118[index].unk00[5] * D_003682F0[index].level / 100;
            D_00446E0C = D_0041E118[index].unk00[6] * D_003682F0[index].level / 100;
            D_00446E10 = D_0041E118[index].unk00[7] * D_003682F0[index].level / 100;
            D_00446E14 = D_0041E118[index].unk00[8] * D_003682F0[index].level / 100;
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            variant = D_00413FBC[D_00426110[0]->group];
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A94; i++) {
                    full_alpha = D_00446C0C[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A94; i++) {
                func_001E5210((s8)D_00446C0C[i].unk42);
                func_00207478((int)&D_00446C0C[i], D_00446C0C[i].unk3E + 1000, variant, (int)D_00425910[i]);
                func_001E5210(0);
            }
            parts = D_00446C0C;
            count = D_00446A94;
            break;
        case 0:
            if (D_00446CD4 != 0) {
                func_0019CDC0(1);
            }
            D_00446CD4 = 0;
            D_00446DF4 = (D_003679A0[index].unk00[0] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[0]) * D_003679A0[index].level / 100;
            D_00446DF8 = (D_003679A0[index].unk00[1] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[1]) * D_003679A0[index].level / 100;
            D_00446DFC = (D_003679A0[index].unk00[2] + D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[2]) * D_003679A0[index].level / 100;
            D_00446E00 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[3] * D_003679A0[index].level / 100;
            D_00446E04 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[4] * D_003679A0[index].level / 100;
            D_00446E08 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[5] * D_003679A0[index].level / 100;
            D_00446E0C = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[6] * D_003679A0[index].level / 100;
            D_00446E10 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[7] * D_003679A0[index].level / 100;
            D_00446E14 = D_0041B9B8[index].unk00[D_003679A0[index].unk14].values[8] * D_003679A0[index].level / 100;
            D_00446E20 = (s32)(D_0041B9B8[index].unk68 * 0.03281f * 10.0f);
            D_00446E24 = D_003679A0[index].level;
            D_00446E1C = D_004147F8[D_00426110[0]->unk0C].unk3E;
            D_00446E28 = D_00413FBC[D_00426110[0]->group];
            D_00446E2C = 100;
            if (D_00447317 == 1) {
                for (i = 0; i < D_00446A98; i++) {
                    full_alpha = D_00446C10[i].alpha;
                    D_00425910[i] = full_alpha;
                }
            }
            for (i = 0; i < D_00446A98; i++) {
                func_001E5210((s8)D_00446C10[i].unk42);
                func_00207478((int)&D_00446C10[i], D_00446C10[i].unk3E + 1000, 0, (int)D_00425910[i]);
                func_001E5210(0);
            }
            parts = D_00446C10;
            count = D_00446A98;
            break;
        }
        switch (D_00447317) {
        case 0:
            for (i = 0; i < count; i++) {
                D_00425910[i] += parts[i].alpha / 10.0f;
                if (D_00425910[i] > parts[i].alpha) {
                    for (i = 0; i < count; i++) {
                        full_alpha = parts[i].alpha;
                        D_00425910[i] = full_alpha;
                    }
                    D_00447317 = 1;
                    break;
                }
            }
            break;
        case 2:
        case 3:
            for (i = 0; i < count; i++) {
                D_00425910[i] -= parts[i].alpha / 10.0f;
                if (D_00425910[i] < 0.0f) {
                    memset(D_00425910, 0, 0x400);
                    if (D_00447317 == 2) {
                        D_00446AD4 = 0;
                    }
                    D_00447317 = 0;
                    break;
                }
            }
            break;
        }
    }
    selected = D_00426110[0];
    table = func_001CFC58(D_00446F38);
    text_index = selected->unk0C + 0x1F5;
    entry = ((s32 *)table)[text_index];
    word.raw = 0x000A875083207080;
    func_001FDB88(word);
    if (entry != 0) {
        func_00200068(table + entry, 0x33);
    }
    if ((D_00446DE8 & 0x4000) && D_00447318 == 0) {
        if ((D_00426110[0]->unk0C < 220 ? GetWeaponLevel(D_00426110[0]->unk0C - 150) < 100
                                        : GetItemLevel(D_00426110[0]->unk0C - 220) < 100) ||
            D_00426110[0]->unk0C == 183) {
            func_0019CDC0(0);
            D_00447316 = 2;
            D_00447317 = 3;
            D_00447318 = 1;
        }
    } else if ((D_00446DE8 & 0x2000) && D_00447318 == 0) {
        func_0019CDC0(2);
        D_00447316 = 3;
        D_00447317 = 2;
        D_00447318 = 1;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_00447316 == 2 && D_00446C90 == NULL) {
        D_00447316 = 4;
    }
    if (D_00447316 == 3 && D_00446C90 == NULL) {
        D_00447316 = 5;
    }
    switch (D_00447316) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00425510[253] += 1.6f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] += D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C50[i].alpha) {
                D_00447316 = 1;
                for (i = 0; i < D_00446AA4; i++) {
                    full_alpha = D_00446C50[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                return;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00425510[254] -= 12.8f;
        D_00446C78.alpha = (s32)D_00425510[255];
        if (D_00425510[254] < 0.0f) {
            D_00425510[254] = 0.0f;
        }
        D_00425510[253] -= 1.6f;
        D_00446C88.alpha = (s32)D_00425510[254];
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA4; i++) {
            D_00425510[i] -= D_00446C50[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                if (D_00447316 == 4) {
                    context->unk04 = 1;
                    D_00446CC4 = func_001F0D90;
                    D_00447313 = 0;
                } else if (D_00447316 == 5) {
                    context->unk04 = 0;
                    D_00446CC4 = func_001EEF78;
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                }
                D_00447316 = 0;
                D_00447317 = 0;
                memset(D_00425510, 0, 0x400);
                return;
            }
        }
        break;
    }
}
