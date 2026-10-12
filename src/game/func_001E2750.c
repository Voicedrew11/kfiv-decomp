#include "kf_menu.h"
#include "kf_packet.h"

extern const s8 D_004401F0[12];
extern u16 *D_00440200[12];
extern const MenuPadBitTable D_00440230;
extern s8 D_004210B0[12];
extern s8 D_004210C0[12];
extern f32 D_00425510[0x100];
extern u32 D_00446A70;
extern int D_00446AC8;
extern void *D_00446B20;
extern s8 D_00446BE6;
extern s32 D_00446BE8;
extern s32 D_00446BEC;
extern s8 D_00446BF0;
extern float D_00446C04;
extern char D_00446C08;
extern s8 D_00446C70;
extern MenuColor D_00446C78;
extern s8 D_00446C94;
extern u16 D_00446DE8;
extern s32 D_00446DF4;
extern s32 D_00446DF8;
extern s32 D_00446DFC;
extern s32 D_00446E00;
extern s32 D_00446E04;
extern SpritePacket *D_00447264;
extern char D_004472DA;
extern u16 D_005CF318[13];

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
f32 func_001D8300(f32, s32, s32);
void func_001DF308(MenuScreenContext *);
int func_001E5210(u8);
void func_001FC5E8(MenuCursorBar *);
void func_00207478(int, int, int, int);

/* Runs SET with the choice number that goes with a pad button bit. */
#define FOR_PAD_BIT(bit, SET) \
    switch (bit) {            \
    case 0x10:                \
        SET(0);               \
        break;                \
    case 0x40:                \
        SET(1);               \
        break;                \
    case 0x20:                \
        SET(2);               \
        break;                \
    case 0x80:                \
        SET(3);               \
        break;                \
    case 0x400:               \
        SET(4);               \
        break;                \
    case 0x100:               \
        SET(5);               \
        break;                \
    case 0x1000:              \
        SET(6);               \
        break;                \
    case 0x4000:              \
        SET(7);               \
        break;                \
    case 0x2000:              \
        SET(8);               \
        break;                \
    case 0x8000:              \
        SET(9);               \
        break;                \
    case 0x800:               \
        SET(10);              \
        break;                \
    case 0x200:               \
        SET(11);              \
        break;                \
    }
#define SET_CHOICE(n) D_004210C0[i] = D_004210B0[i] = (n)
#define SET_SHOWN(n) D_004210C0[i] = (n)

/* The button screen: picks a button for each of twelve actions, refuses to leave while two actions share one, and fades its parts. */
void func_001E2750(void) {
    MenuPadBitTable pad_bits = D_00440230;
    MenuCursorBar bar;
    s32 has_duplicate;
    s32 last_is_shared;
    s32 bar_drawn;
    s32 i;
    s32 j;
    s32 choice_count = 12; // match: the loop bounds are this variable, not a literal (see the note on the first loop)
    s32 bar_mid24 = 0x493E0; // match: a variable; with the literal in the assignment the bar's 64-bit constants are loaded differently
    s32 line_count;
    s32 wrap;
    u8 kind;
    s8 slot;
    s32 uses;
    s32 is_shared;
    s32 other;
    u32 full_alpha;

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        if (D_00446BF0 == 0) {
            func_0019CDC0(1);
            // match: only this loop, in the function's first run of code, is loop-optimised in retail; the others keep their entry tests until late
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*D_00440200[i], SET_CHOICE)
            }
            D_00446BE8--;
        } else {
            D_00446BEC = (D_00446BEC + 1) & 1;
        }
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        if (D_00446BF0 == 0) {
            func_0019CDC0(1);
            for (i = 0; i < choice_count; i++) {
                FOR_PAD_BIT(*D_00440200[i], SET_CHOICE)
            }
            D_00446BE8++;
        } else {
            D_00446BEC = (D_00446BEC + 1) & 1;
        }
    }
    line_count = choice_count + 1;
    D_00446BE8 += line_count;
    D_00446BE8 %= line_count;

    wrap = choice_count; // match: a second variable, set here, is what retail keeps in $16 for the two divisions
    for (i = 0; i < wrap; i++) {
        if (D_004210B0[i] == -1) {
            FOR_PAD_BIT(*D_00440200[i], SET_SHOWN)
        } else {
            D_004210C0[i] = D_004210B0[i];
        }
    }

    if (D_00446BE8 != choice_count && D_00446BF0 == 0) {
        if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
            func_0019CDC0(1);
            D_004210B0[D_00446BE8] = (s8)(D_004210B0[D_00446BE8] + 1) % wrap;
        } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
            func_0019CDC0(1);
            D_004210B0[D_00446BE8] = (s8)(D_004210B0[D_00446BE8] + 11) % wrap;
        }
    }

    has_duplicate = FALSE;
    for (i = 0; i < choice_count - 1; i++) {
        for (j = i + 1; j < choice_count; j++) {
            if (*D_00440200[i] == *D_00440200[j]) {
                has_duplicate = TRUE;
                i = choice_count;
                break;
            }
        }
    }

    last_is_shared = FALSE;
    j = 0;
    for (i = 0; i < choice_count; i++) {
        if (*D_00440200[11] == *D_00440200[i]) {
            j++;
            if (j != 1) {
                last_is_shared = TRUE;
                break;
            }
        }
    }

    D_00446DFC = 0;
    bar_drawn = FALSE;
    D_00446E00 = 0;
    for (i = 0; i < D_00446A70; i++) {
        kind = D_00447264[i].unk3E;
        slot = D_00447264[i].slot;
        if (!bar_drawn && D_00447264[i].unk43 == 1) {
            bar_drawn = TRUE;
            if (D_00446BF0 == 0) {
                bar.first.f.lo16 = 0x6FE0;
                bar.first.f.mid16 = D_00446BE8 * 0xB0 + 0x7BE0;
                bar.first.f.mid24 = bar_mid24;
                bar.second.f.lo16 = bar.first.f.lo16 + 0xE60;
                bar.second.f.mid16 = bar.first.f.mid16 + 0xA0;
                bar.second.f.mid24 = bar.first.f.mid24;
                bar.red = 0xFF;
                bar.green = 0xFF;
                bar.blue = 0x78;
                if (D_00446C70 == 1) {
                    D_00425510[0xFD] = func_001D8300(D_00425510[0xFD], 0x10, 0x20);
                }
                bar.alpha = D_00425510[0xFD];
                func_001FC5E8(&bar);
            }
        }
        ((void (*)(u8))func_001E5210)(D_00447264[i].unk42); // match: called as returning nothing, which frees $2 for the switch
        switch (kind) {
        case 2:
            if (D_00446BF0 != 0) {
                func_00207478((int)&D_00447264[i], kind + 1000, 0, D_00425510[i]);
            }
            break;
        case 3:
            if (D_00446BF0 == 1) {
                func_00207478((int)&D_00447264[i], kind + 1000, 0, D_00425510[i]);
            } else if (D_00446BF0 == 2) {
                func_00207478((int)&D_00447264[i], kind + 1000, 1, D_00425510[i]);
            }
            break;
        case 0x18:
            is_shared = FALSE;
            for (other = 0, uses = 0; other < choice_count; other++) {
                if (pad_bits.bits[slot] == *D_00440200[other]) {
                    uses++;
                    if (uses != 1) {
                        is_shared = TRUE;
                    }
                }
            }
            if (slot == D_004210C0[D_00446BE8]) {
                D_00446DF4 = 0x20400;
                D_00446DF8 = 0x80FF00;
            } else if (is_shared) {
                D_00446DF4 = 0x4020000;
                D_00446DF8 = 0xFF800000;
                D_00446DFC++;
            } else {
                D_00446DF4 = 0;
                D_00446DF8 = 0;
            }
            func_00207478((int)&D_00447264[i], kind + 1000, slot, D_00425510[i]);
            break;
        case 0x21:
            if (D_00446E00 == 0) {
                j = i;
            }
            D_00446E04 = D_004210C0[D_00446E00];
            // match: called as returning a value, which keeps $2 out of the count's increment
            ((int (*)(int, int, int, int))func_00207478)((int)&(&D_00447264[j])[11 - D_00446E04], kind + 1000, D_00446E00, D_00425510[i]);
            D_00446E00++;
            break;
        default:
            func_00207478((int)&D_00447264[i], kind + 1000, slot, D_00425510[i]);
            break;
        }
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        if (D_00446BF0 != 0) {
            D_00446BF0 = 0;
            D_00446BEC = 0;
        } else if (!has_duplicate && !last_is_shared) {
            D_004472DA = 0;
            D_00446C70 = 2;
        } else if (has_duplicate && last_is_shared) { // match: both arms set the same prompt; retail still tests last_is_shared here
            D_00446BF0 = 1;
        } else {
            D_00446BF0 = 1;
        }
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        func_0019CDC0(0);
        if (D_00446BF0 == 0) {
            if (D_00446BE8 != choice_count) {
                *D_00440200[D_00446BE8] = pad_bits.bits[D_004210B0[D_00446BE8]];
                D_005CF318[D_00446BE8] = pad_bits.bits[D_004210B0[D_00446BE8]];
            } else {
                for (i = 0; i < choice_count; i++) {
                    D_004210B0[i] = D_004401F0[i];
                    *D_00440200[i] = pad_bits.bits[D_004210B0[i]];
                }
            }
        } else if (D_00446BF0 == 1) {
            D_00446BF0 = 0;
        } else if (D_00446BF0 == 2) {
            if (D_00446BEC == 0) {
                D_00446C70 = 2;
                D_004472DA = 0;
            } else {
                D_00446BF0 = 0;
                D_00446BEC = 0;
            }
        }
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[0xFF] += 12.8f;
        if (D_00425510[0xFF] > 128.0f) {
            D_00425510[0xFF] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        D_00425510[0xFD] += 1.6f;
        if (D_00425510[0xFD] > 16.0f) {
            D_00425510[0xFD] = 16.0f;
        }
        for (i = 0; i < D_00446A70; i++) {
            D_00425510[i] += (f32)D_00447264[i].alpha / 10.0f;
            if ((f32)D_00447264[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A70; i++) {
                    full_alpha = D_00447264[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_00446C70 = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[0xFF] -= 12.8f;
        if (D_00425510[0xFF] < 0.0f) {
            D_00425510[0xFF] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[0xFF];
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A70; i++) {
            D_00425510[i] -= (f32)D_00447264[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C94 == 3) {
                    D_00446BE6++;
                } else {
                    D_00446BE6 = 2;
                }
                if (D_00446BE6 == 2) {
                    D_00446BE6 = 0;
                    D_00446C70 = 0;
                    if (D_00446C94 != 3) {
                        D_00446B20 = (void *)func_001DF308;
                    } else {
                        D_00446AC8++;
                    }
                    D_00446C04 = 128.0f;
                    D_00446C08 = 0;
                    D_00446BE8 = 0;
                    D_00446BEC = 0;
                    D_00446BF0 = 0;
                }
                break;
            }
        }
        break;
    }
}
