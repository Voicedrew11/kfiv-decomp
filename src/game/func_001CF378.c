#include "kf_game.h"
#include "kf_file.h"
#include "kf_gs.h"

extern char D_0043F698[];
extern char D_0043F6A8[];
extern s32 D_00446F38;
extern int D_004471DC;
extern char D_004471E0;
extern char D_004471E1;
extern MessageSlot *D_004471E4;
extern MessageSlot *D_004471E8;
extern int D_004471EC;
extern char D_005BF8C8[];
extern char D_005C0CC8[];
extern char D_005C50C8[];
extern u64 D_005C58C8[4];
extern MessageSlot D_005C5940[6];

s32 func_001CFC48(s32);
s32 func_001CFC58(s32);
void func_0020B2E8(void *, u32);

/* Loads the two text files, empties the message ring and the debug log, and sets up the two GS
   register writes (TEX0 value, register number) the two fonts are drawn with. */
void func_001CF378(void) {
    char unused[0x60]; // match: retail reserves 0x60 bytes it never uses
    int file_handle;
    int i;

    file_handle = func_00102238(D_0043F698, 0, 1);
    func_001025C0(file_handle, (void *)func_001CFC48(D_00446F38), -1);
    func_001024B0(file_handle);
    file_handle = func_00102238(D_0043F6A8, 0, 1);
    func_001025C0(file_handle, (void *)func_001CFC58(D_00446F38), -1);
    func_001024B0(file_handle);
    D_004471DC = 0;
    D_004471E8 = D_005C5940;
    D_004471E4 = D_005C5940;
    D_004471EC = 0;
    D_004471E0 = 0;
    D_004471E1 = 0;
    for (i = 0; i < 5; i++) {
        D_005C5940[i].text = &D_005BF8C8[i * 0x400];
        D_005C5940[i].next = &D_005C5940[i + 1];
    }
    D_005C5940[5].text = D_005C0CC8;
    D_005C5940[5].next = D_005C5940;
    // match: the callee returns a value here; $2 is then taken when the table address is formed
    ((int (*)(void *, u32))func_0020B2E8)(D_005C50C8, 0x200);
    D_005C58C8[0] = 0x4007FC0665423B40L;
    D_005C58C8[1] = GS_REG_TEX0_1;
    D_005C58C8[2] = 0x4007FC2665423B40L;
    D_005C58C8[3] = GS_REG_TEX0_1;
}
