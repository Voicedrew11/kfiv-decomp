/* What main runs at start-up, and the movies: the system set-up (System_Init), the display set-up
   (Display_Init) and the opening and ending movies (Movie_). kf_memory.c and kf_display.c follow it; System_Init
   and Display_Init call into kf_display.c, so the three were probably one source file. */
#include "kf_system.h"
#include "kf_display.h"
#include "kf_game.h"
#include "kf_pad.h"
#include "kf_gs.h"

#define PAD_BUTTON_START 0x0008

extern int D_00446E90;          /* set once START was pressed during the opening movie */
extern char *D_00446F50;        /* the memory area the movie player is given as its heap */
extern int D_00446F20;          /* the size it is given for that heap */
extern int D_00446F78;          /* the half of the first double buffer in use; each swap flips it */
extern int D_00446F7C;          /* the same for the second */
extern int D_00446F80;
extern u32 D_0038CC74[];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern char D_004412E0[];       /* "cdrom0:\DATA\KF4OP.PSS;1" */
extern char D_00441300[];       /* "cdrom0:\DATA\KF4ED.PSS;1" */
extern char D_00441320[];       /* "cdrom0:\DATA\KF4STAFF.PSS;1" */
extern f32 D_00431AF0[16];
extern f32 D_00432320[16];
extern f32 D_004323A0[16];
extern f32 D_00432560[4][4];
extern GsDoubleBuffer D_00432D20;
extern GsDoubleBuffer D_00432F50;
extern GsRegisterPair D_00433180;
extern GsRegisterPair D_00433190;

int Movie_Play(char *, u32, u32, int (*)(void), u32, s16, s16, s16);
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_0010A008(void);
void func_0010A4E8(s64, s64, s64, s64, s64); // match: retail passes the last value as a 64-bit constant (0x100000044)
void func_001095D8(void);
void func_001096E8(void);
void func_00109800(void);
void func_001CB9A0(void);
void Sound_Init(void);
int sceGsSyncV(int);
void Vram_Clear(int, int);
int func_0022B5B8();
int func_0022B8E8();
int sceDevVu0Pause();
int sceDevVu0SetTBit();
int sceDmaReset(int);
int sceGsResetGraph(int, int, int, int);
int sceGsResetPath();
int sceMcInit();
int sceSdRemoteInit();
int sceSifInitIopHeap();
void sceGsSetDefDBuff(void *, s16, s16, s16, s16, s16, s16);
void sceVu0CopyMatrix(void *, void *);

/* The opening movie's per-frame callback (given to Movie_Play by Movie_PlayOpening): polls the controller
   and returns TRUE, ending the movie, when START is pressed. */
static int Movie_PollSkip(void) {
    Pad_Update();
    if (Pad_GetReport(0, 0)->pressed & PAD_BUTTON_START) {
        D_00446E90 = TRUE;
        return TRUE;
    }
    return FALSE;
}

/* Plays the opening movie KF4OP.PSS, which START skips (D_00446E90 then set), then resets both double buffers'
   frame registers and the draw state, and swaps. */
void Movie_PlayOpening(void) {
    s64 temp;
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */

    D_00446E90 = 0;
    Movie_Play(D_004412E0, (u32)D_00446F50, D_00446F20, Movie_PollSkip, 0x3FFF, 2, 0, 0);
    temp = (s64)0xC000 << 17;
    D_00432E20[0] = temp;
    D_00432E20[30] = temp;
    D_00433050[0] = temp;
    D_00433050[30] = temp;
    func_001095E0();
    func_001095E0();
    func_0010B000();
    func_001095D0();
    D_00446F78 = 0;
    D_00446F7C = 0;
    func_0010A008();
    func_0010A4E8(0, 0, 0, 0x80, 0x100000044L);
    func_001095D8();
    D_00446F78 = 1;
    D_00446F7C = 1;
    func_001096E8();
    func_00109800();
    Display_SwapBuffers(0);
    func_001CB9A0();
    Sound_Init();
}

/* Plays the ending movie KF4ED.PSS when bit 11 of D_0038CC74 is set, the staff roll KF4STAFF.PSS otherwise;
   neither can be skipped. */
void Movie_PlayEnding(void) {
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */

    D_00446E90 = 0;
    if ((D_0038CC74[0] >> 11) & 1) {
        Movie_Play(D_00441300, (u32)D_00446F50, D_00446F20, 0, 0x3FFF, 2, 0, 0);
    } else {
        Movie_Play(D_00441320, (u32)D_00446F50, D_00446F20, 0, 0x3FFF, 2, 0, 0);
    }
    sceGsSyncV(0);
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    func_001CB9A0();
    Sound_Init();
}

/* Returns TRUE; the 0x80 bytes of stack are unused. Nothing calls it. */
static int func_0020AF18(void) {
    volatile char buf[0x80];
    return 1;
}

/* The same as func_0020AF18. */
static int func_0020AF28(void) {
    volatile char buf[0x80];
    return 1;
}

/* Reboots the IOP and loads its modules, then starts the memory cards, resets VIF0, VU0, VIF1, DMA and the GS,
   and starts the sound library and the IOP heap. */
void System_Init(void) {
    System_RebootIop();
    sceMcInit();
    func_0022B5B8();
    sceDevVu0Pause();
    func_0022B8E8();
    sceDevVu0SetTBit();
    sceDmaReset(1);
    sceGsResetPath();
    sceGsResetGraph(0, 1, 2, 0);
    sceSdRemoteInit();
    sceSifInitIopHeap();
}

/* Sets up the game's two double buffers (the second one draws through GS context 2), the view
   matrices and the projection. */
void Display_Init(void) {
    sceGsSetDefDBuff(&D_00432D20, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    D_00432D20.disp[0].dispfb.fbp = 0;
    D_00432D20.disp[1].dispfb.fbp = 0x8C;
    D_00432D20.draw0.frame.fbp = 0x8C;
    D_00432D20.draw1.frame.fbp = 0;
    D_00432D20.draw0.zbuf.zbp = 0x118;
    D_00432D20.draw1.zbuf.zbp = 0x118;
    sceVu0CopyMatrix(D_00432320, D_00431AF0);
    sceVu0CopyMatrix(D_004323A0, D_00431AF0);
    Display_BuildProjection(D_00432560, 512.0f, 1.0f, 65536.0f);
    D_00446F78 = 0;
    D_00446F80 = 0;
    D_00432D20.clear1.rgbaq = D_00432D20.clear0.rgbaq = 0x100000000L;
    sceGsSetDefDBuff(&D_00432F50, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    D_00432F50.disp[0].dispfb.fbp = 0;
    D_00432F50.disp[1].dispfb.fbp = 0x8C;
    D_00432F50.draw0.frame.fbp = 0x8C;
    D_00432F50.draw1.frame.fbp = 0;
    D_00432F50.draw0.zbuf.zbp = 0x118;
    D_00432F50.draw1.zbuf.zbp = 0x118;
    D_00432F50.draw0.frame_addr = 0x4D;
    D_00432F50.draw0.zbuf_addr = 0x4F;
    D_00432F50.draw0.xyoffset_addr = 0x19;
    D_00432F50.draw0.scissor_addr = 0x41;
    D_00432F50.draw0.test_addr = 0x48;
    D_00432F50.clear0.test_a_addr = 0x48;
    D_00432F50.clear0.prim |= 0x200;
    D_00432F50.clear0.test_b_addr = 0x48;
    D_00432F50.draw1.frame_addr = 0x4D;
    D_00432F50.draw1.zbuf_addr = 0x4F;
    D_00432F50.draw1.xyoffset_addr = 0x19;
    D_00432F50.draw1.scissor_addr = 0x41;
    D_00432F50.draw1.test_addr = 0x48;
    D_00432F50.clear1.test_a_addr = 0x48;
    D_00432F50.clear1.prim |= 0x200;
    D_00432F50.clear1.test_b_addr = 0x48;
    D_00446F7C = 0;
    D_00433180.first = *(u64 *)&D_00432D20.disp[0].dispfb;
    D_00433180.second = *(u64 *)&D_00432D20.disp[1].dispfb;
    D_00433190.first = D_00432D20.disp[0].display;
    D_00433190.second = D_00432D20.disp[1].display;
    // match: the four stores below come out in retail order only when written in this one
    D_00432F50.draw1.xyoffset.ofy = 0x7200;
    D_00432F50.draw0.xyoffset.ofy = 0x7200;
    D_00432D20.draw1.xyoffset.ofy = 0x7200;
    D_00432D20.draw0.xyoffset.ofy = 0x7200;
}
