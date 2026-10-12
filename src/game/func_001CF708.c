#include "kf_game.h"
#include "kf_gs.h"
#include "kf_texture.h"

extern s32 D_00446F38;
extern u32 *D_004471D8;
extern int D_004471DC;
extern char D_004471E0;
extern char D_004471E1;
extern MessageSlot *D_004471E4;
extern int D_004471EC;
extern MessageBox D_005C58E8;

s32 func_001CFC48(s32);
void func_0010B238(TextureTableHeader *);
void func_00109E88(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_001CFFA0(char, int, int, int, s16 *, int);
void func_001CFCA0(void);

/* Builds and queues the GS packet that draws the message box or the message on screen and the debug log. */
void func_001CF708(void) {
    char unused[0x60]; // match: retail reserves 0x60 bytes it never uses
    u32 *packet;
    u32 *words;
    int textured = TRUE;
    u64 *blend;
    u64 *texture;

    if (D_004471E0 != 0 || D_004471E1 != 0 || D_004471EC != 0) {
        func_0010B238((TextureTableHeader *)func_001CFC48(D_00446F38));
        func_00109E88();
        if (D_004471E0 != 0) {
            D_004471DC += D_005C58E8.typed;
        } else if (D_004471E1 != 0) {
            D_004471DC += D_004471E4->length;
        }
        /* seven quadwords for each glyph after a header of 12 */
        words = packet = func_00109990(D_004471DC * 7 + 0xD);
        *words++ = (D_004471DC * 7 + 0xB) | 0x10000000;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = 0;
        *words++ = (D_004471DC * 7 + 0xA) | 0x51000000;
        /* GIF tag: three register writes */
        *words++ = 0x8003;
        *words++ = 0x10004000;
        *words++ = 0xE;
        *words++ = 0;
        blend = (u64 *)words;
        blend[0] = 0;
        blend[1] = 0x49; /* PABE */
        blend[2] = 0x44;
        blend[3] = GS_REG_ALPHA_1;
        words = (u32 *)(blend + 4);
        *(u64 *)words = 0x30000;
        words += 2;
        *(u64 *)words = GS_REG_TEST_1;
        words += 2;
        if (textured) { // match: see `texture` below
            /* GIF tag: four register writes */
            *words++ = 0x8004;
            *words++ = 0x10004000;
            *words++ = 0xE;
            *words++ = 0;
        }
        // match: the tag after these registers is only addressed from `texture` (and not from `packet`) when the
        // compiler loses track of where `texture` points, which a condition it works out later does
        if (textured) {
            texture = (u64 *)words;
            texture[0] = 0;
            texture[1] = 0; /* PRIM */
            texture[4] = 0x4007FC0665423B40L;
            texture[5] = GS_REG_TEX0_1;
            texture[2] = 0;
            texture[3] = GS_REG_TEX1_1;
            texture[6] = 0;
            texture[7] = 8; /* CLAMP_1 */
        }
        /* GIF tag: the glyphs, seven registers each (PRIM, a register write, then RGBAQ, ST, XYZF2, ST, XYZF2) */
        D_004471D8 = (u32 *)(texture + 8);
        *D_004471D8++ = D_004471DC | 0x8000;
        *D_004471D8++ = 0x70000000;
        *D_004471D8++ = 0x42421E0;
        *D_004471D8++ = 0;
        if (D_004471E0 == 1) {
            func_001CFFA0(0, D_005C58E8.font, D_005C58E8.left, D_005C58E8.top, D_005C58E8.start, D_005C58E8.typed);
        } else if (D_004471E1 == 1) {
            func_001CFFA0(1, D_004471E4->unk1C, D_004471E4->unk14, D_004471E4->unk18, (s16 *)D_004471E4->text,
                          D_004471E4->length);
        }
        if (D_004471EC > 0) {
            func_001CFCA0();
        }
        func_00109650(packet, D_004471D8);
        D_004471DC = 0;
        D_004471EC = 0;
    }
}
