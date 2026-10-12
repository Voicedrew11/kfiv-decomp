#include "kf_gs.h"

extern u8 D_00349680[0x10];         /* the options block */
extern GsDoubleBufferDc D_00426140;
extern char D_004261A0[];           /* inside D_00426140: the first buffer's draw environments */
extern char D_00426220[];
extern char D_00426310[];           /* inside D_00426140: the second buffer's draw environments */
extern char D_00426390[];
extern void (*D_0043F6E8[])(void);
extern s32 D_00446C98;
extern u16 D_00446DE8;
extern int D_00446F78;
extern int D_00446F7C;
extern u8 D_00447208;

void func_001E5070(s32);
int func_001CEBE0(void);
int func_0020D088(void);
int func_0020D078(void); // match: an int result keeps $2 clear of the pad test after the call
int sceGsSyncPath(int, int);
int sceGsSyncV(int);
void sceGsSetHalfOffset(void *, int, int, s16); // match: void, so the test after the call uses $2
int sceGsSetHalfOffset2(void *, int, int, s16);
int sceGsSwapDBuffDc(GsDoubleBufferDc *, int);
int FlushCache(int);

/* Ends a menu frame: draws the brightness sprite, waits for the vertical blank, sets the half-line
   offset of the buffer drawn next, swaps the buffers and (when asked) runs the option's handler.
   Gives back whether the field just shown was the even one. */
s32 func_001D72F0(s32 run_handler) {
    s32 even_field;

    func_001E5070(D_00349680[10]);
    func_001CEBE0();
    sceGsSyncPath(0, 0);
    D_00446C98 = func_0020D088();
    even_field = sceGsSyncV(0) == 0;
    func_0020D078();
    if (D_00446DE8 & 0x100) {
        D_00447208++;
    }
    D_00447208 %= 3;
    sceGsSetHalfOffset(D_00446F78 != 0 ? D_00426310 : D_004261A0, 0x800, 0x800, even_field);
    sceGsSetHalfOffset2(D_00446F78 != 0 ? D_00426390 : D_00426220, 0x800, 0x800, even_field);
    FlushCache(0);
    sceGsSyncPath(0, 0);
    sceGsSwapDBuffDc(&D_00426140, D_00446F78);
    if (run_handler != 0) {
        D_0043F6E8[(s8)D_00349680[8]]();
    }
    D_00446F78 = D_00446F78 == 0;
    D_00446F7C = D_00446F7C == 0;
    return even_field;
}
