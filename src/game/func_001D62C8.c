#include "kf_game.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include <eeregs.h>

struct sceCdCLOCK;
int sceCdReadClock(struct sceCdCLOCK *);
int sceGsSyncV(int);
int mktime(int *);
void func_0019BDA0(int, int);
void func_0019CCB0(void);
void func_0019CD10(void);
void func_0019DC00(void);
void func_0019E8B0(void);
void func_0019E920(void);
void func_001A15B8(void);
void func_001A15C0(void);
void func_001B5460(void);
int func_001CEBE0(void);
s32 func_001D72F0(s32);
void func_001FBDC8(void);
s16 func_00206C40(void);
int func_002179F8(int, int);

extern GsDrawEnv D_00432D80;
extern int (*D_0043F6E0[])(void); // match: two handlers (func_001D6FB0, func_001D77F0); with its size it would be reached through $gp
extern MenuWeekdayNames D_0043FC08;
extern s32 D_00446AC4;
extern s8 D_00446C94;
extern u16 D_00446DE8;
extern int D_00446F78;
extern int D_00446F7C;
extern s32 D_00447200;
extern s32 D_00447204;
extern int D_004472D0;
extern int D_004472D4;
extern MenuScreenState D_005CF2F0;

/* Runs the menu from opening to closing: sets up, calls the handler D_00446AC4 names every frame until it gives 0,
   then shows the last frames. `mode` (D_00446C94) is which menu; with mode 1 the result is 1 when
   D_005CF2F0.unk16 was set. */
int func_001D62C8(s8 mode) { // match: callers declare the parameter int; retail sign-extends a byte here
    ClockTime clock;
    int tm_fields[9];
    MenuWeekdayNames weekday_names;
    u32 *packet;
    s32 status; // match: one variable for the card state in the loop and the menu number after it

    sceGsSyncPath(0, 0);
    sceGsSyncV(0);
    func_0019BDA0(0x3D00, 0x3F00);
    func_001A15C0();
    func_0019DC00();
    func_0019CCB0();
    func_0019E8B0();
    sceCdReadClock((struct sceCdCLOCK *)&clock);
    weekday_names = D_0043FC08; // match: never read
    // match: the clock bytes are BCD and are used as they are, the day is lowered by one, and the result is not used
    tm_fields[0] = clock.sec;
    tm_fields[1] = clock.min;
    tm_fields[2] = clock.hour;
    tm_fields[3] = clock.mday - 1;
    tm_fields[4] = clock.mon - 1;
    tm_fields[5] = clock.year + 100;
    mktime(tm_fields);
    D_00446C94 = mode;
    D_004472D0 = D_004472D4 = func_002179F8(0, 0);
    while (func_001CEBE0(), D_0043F6E0[D_00446AC4]() != 0) {
        sceGsSyncPath(0, 0);
        D_00447200 = func_001D72F0(1);
        func_001B5460();
        status = func_002179F8(0, 0);
        D_004472D0 = status;
        if (status != D_004472D4) {
            D_00447204 = 1;
        }
        D_004472D4 = status;
        func_00206C40();
        if (D_00447204 > 0) {
            D_00447204++;
            if (D_00447204 > 20) {
                D_00447204 = 0;
            } else {
                D_00446DE8 = 0;
            }
        }
    }
    if (D_00446C94 == 0 && D_005CF2F0.unk16 == 0) {
        packet = func_001FBE70();
        packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
        packet[2] = 0xE;
        packet[3] = 0;
        *(u64 *)&packet[4] = 0x66802B240;
        packet[6] = 6;
        packet[7] = 0;
        *(u64 *)&packet[8] = D_00432D80.frame;
        packet[10] = 0x4C;
        packet[11] = 0;
        packet[12] = 0;
        packet[13] = 1;
        packet[14] = 0x4E;
        packet[15] = 0;
        packet[16] = 0x30000;
        packet[17] = 0;
        packet[18] = 0x47;
        packet[19] = 0;
        packet[20] = 0;
        packet[21] = 0;
        packet[22] = 0x18;
        packet[23] = 0;
        packet[24] = 0;
        packet[25] = 0;
        packet[26] = 2;
        packet[27] = 0;
        packet[28] = 0;
        packet[29] = 0;
        packet[30] = 5;
        packet[31] = 0;
        *(f32 *)&packet[32] = 0.625f;
        *(f32 *)&packet[33] = 0.4375f;
        packet[34] = 2;
        packet[35] = 0;
        packet[36] = 0x1C002800;
        packet[37] = 0;
        packet[38] = 5;
        packet[39] = 0;
        packet[40] = 0;
        packet[41] = 0;
        packet[42] = 0x3F;
        packet[43] = 0;
        packet[0] = 0x800A;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 11;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        D_00446F7C = D_00446F78 = 1;
    }
    D_00446AC4 = 0;
    func_0019E920();
    func_001A15B8();
    func_0019CD10();
    status = D_00446C94;
    if (status == 0) {
        sceGsSyncPath(0, 0);
        while (sceGsSyncV(0) != 0);
    }
    switch (D_00446C94) {
    case 2:
    case 3:
    case 4:
    case 5:
        D_005CF2F0.unk16 = 0;
        sceGsSyncPath(0, 0);
        func_001D72F0(0);
        sceGsSyncV(0);
        sceGsSyncPath(0, 0);
        func_001D72F0(0);
        sceGsSyncV(0);
        return 0;
    case 0:
        if (D_005CF2F0.unk16 != 0) {
            D_005CF2F0.unk16 = 0;
            sceGsSyncPath(0, 0);
            func_001D72F0(0);
            sceGsSyncV(0);
            sceGsSyncPath(0, 0);
            func_001D72F0(0);
            sceGsSyncV(0);
        }
        return 0;
    case 1:
        func_001FBDC8();
        sceGsSyncPath(0, 0);
        func_001D72F0(0);
        sceGsSyncV(0);
        sceGsSyncPath(0, 0);
        func_001D72F0(0);
        sceGsSyncV(0);
        if (D_005CF2F0.unk16 != 0) {
            D_005CF2F0.unk16 = 0;
            return 1;
        }
        return 0;
    }
}
