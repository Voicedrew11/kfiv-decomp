#include "kf_game.h"
#include "kf_map.h"
#include "kf_player.h"
#include "kf_sound.h"

extern u8 D_00349680[0x10];
extern PlayTime D_0036A138[];   /* the play time in the loaded save (D_0036A130 + 8) */
extern SoundLoadQueue D_0038A130;
extern GameStatus D_0038CC50;
extern MapSlot D_0038CD20[];
extern MapLoadState D_0038D610;
extern u16 D_00413DA8[];
extern PlayerState D_00413F80;
extern f32 D_004323E0[16];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern int D_00446734;
extern u32 D_00446ED4;

void func_00112948(int);
int func_001D20A0(void);
void func_001D2048(PlayTime);
void func_00106178(int, int, int, int);
void func_001095E0(void);
void func_0020B780(int);
int func_0020D088(void);
void func_0020D078(void);
void func_001096E8(void);
void func_0010B000(void);
void func_00111D58(void);
void func_001B5460(void);
int func_00210D48(void);
void func_001B5A48(void);
void func_00100D10(void);
void func_001D0970(void);
void func_00117CC8(void);
void func_001BFCC8(void);
void func_00112BD0(void);
void func_0011D180(void);
void func_00123288(void);
void func_001256C0(void);
void func_001C95B0(void);
void func_001CAD80(void);
void func_001BC2D0(void);
void func_001B7DF8(void);
void func_001A1E88(void);
void func_0019B0B8(void);
int func_001D19D0(void);    // match: the flag test after the call uses $3 only if the call leaves a value in $2
void func_00101618(void);
void func_0019C540(void);
void func_001061E8(void);
void func_002082A8(void);
void func_00209218(void);
void func_00100FC8(void);
void func_001012D8(int, int);
void func_0010A7E0(void *);
void func_001C00D8(void);
int func_001005E8(int);
void func_00109800(void);
void func_001C9C00(void);
void func_001161A8(void);
void func_001017A8(void);
void func_00118958(void);
void func_00101F90(void);
void func_001B5778(void);
void func_001CFC98(void);
void func_001CFC80(void);
void func_0019C7F0(int);
void func_0019C820(int);
int sceGsSyncV(int);
int func_0019F6B0(void);
void func_00102558(void);

/* The game proper: places the player (new game) or resumes a loaded one, runs the main loop until
   GameStatus.bit9 is set, then waits for the sound loads to end. */
void func_001001C0(int arg) {
    MapSlotUnk08 *header;
    u8 saved;

    func_00112948(arg);
    if (D_00446734 == 0) {
        func_001D20A0();
        header = (MapSlotUnk08 *)D_0038CD20[0].header;
        D_00413F80.position.x = header->unk60;
        D_00413F80.position.y = header->unk64;
        D_00413F80.position.z = header->unk68;
        D_00413F80.rotation.y = header->unk6C;
    } else {
        D_00446734 = 0;
        func_001D2048(D_0036A138[0]);
        func_00106178(0, 0, 0, 0x1E);
    }
    D_0038D610.flags.raw |= 1;
    func_001095E0();
    func_001095E0();
    while (!D_0038CC50.bit9) {
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_0020B780(1);
        }
        D_00446ED4++;
        func_0020D088();
        func_0020D078();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_001096E8();
            func_0020D088();
            func_001095E0();
            func_0010B000();
            func_00111D58();
            func_001B5460();
        }
        if (D_0038CC50.bit12 && !D_0038CC50.bit13) {
            if (D_00413DA8[1] & 8) {
                D_00413DA8[1] = 0;
                D_0038CC50.bit13 = TRUE;
                D_00413DA8[0] = 0;
            }
        }
        if (!(D_0038CC50.unk30 & 0x40) && D_00413F80.unk1A8 == 0) {
            func_00210D48();
        }
        func_001B5A48();
        func_00100D10();
        func_001D0970();
        func_00117CC8();
        func_001BFCC8();
        func_00112BD0();
        func_0011D180();
        func_00123288();
        func_001256C0();
        func_001C95B0();
        func_001CAD80();
        func_001BC2D0();
        func_001B7DF8();
        func_001A1E88();
        func_0019B0B8();
        func_001D19D0();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_00101618();
            func_0019C540();
        }
        func_001061E8();
        func_002082A8();
        func_00209218();
        func_00100FC8();
        func_001012D8(0, 0);
        func_0010A7E0(D_004323E0);
        func_001C00D8();
        if (!(D_0038CC50.bit12 && D_0038CC50.bit13)) {
            func_001005E8(1);
            func_00109800();
        }
        func_001C9C00();
        func_001161A8();
        func_001017A8();
        func_00118958();
        func_00101F90();
    }
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    func_001B5778();
    func_001CFC98();
    func_001CFC80();
    saved = D_00349680[1];
    D_00349680[1] = 0;
    func_0019C7F0(0);
    func_0019C820(0);
    while (D_0038A130.unk0C != 0 ||
           ((D_0038A130.unk04 != 0 || D_0038A130.unk18 != 0) &&
            (D_0038A130.unk18 != 0 || D_0038A130.unk04 == 0 || func_0019F6B0() == 0))) {
        while (sceGsSyncV(0) != 1) {
        }
        func_0020D078();
        func_00101618();
        func_0019C540();
    }
    D_00349680[1] = saved;
    D_0038CC50.bit9 = FALSE;
    func_00102558();
    func_001095E0();
    func_00109800();
}
