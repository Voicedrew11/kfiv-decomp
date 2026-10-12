#include "kf_entity.h"
#include "kf_map.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern void (*D_0038D5E0[])(void);
extern MapLoadState D_0038D610;
extern MapLoadRequest D_0038D640;
extern PlayerState D_00413F80;
extern int D_00446770;
extern int D_00446794;

int func_001005E8(int);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010B000(void);
void func_001152B8(int);
void func_00115878(int);
void func_0011CE80(int);
void func_00125560(int);
void func_0019AAA0(int);
void func_001A15F8(void);
void func_001A3040(int);
void func_001C8938(int);
void func_001CACA0(int);
void func_0020B780(int);

/* Finishes the running map load at once, makes the loaded map current, puts the player at its start
   and frees the old map. */
void func_00117F98(void) {
    while (D_0038D610.step != 5) {
        func_001005E8(0);
        func_00109800();
        func_0020B780(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        D_0038D5E0[D_0038D610.step]();
    }
    D_0038D610.step = 6;
    do {
        D_0038D5E0[D_0038D610.step]();
    } while (D_0038D610.step != 0);
    func_001152B8(D_00446770 == 0);
    D_00413F80.position.x = ((MapSlotUnk08 *)D_0038CD20[D_00446770].header)->unk60;
    D_00413F80.position.y = ((MapSlotUnk08 *)D_0038CD20[D_00446770].header)->unk64;
    D_00413F80.position.z = ((MapSlotUnk08 *)D_0038CD20[D_00446770].header)->unk68;
    D_00413F80.rotation.y = ((MapSlotUnk08 *)D_0038CD20[D_00446770].header)->unk6C;
    D_0038D640.map = 0xFFFF;
    D_00446794 = 0;
    D_00413F80.unk1AC = D_00413F80.unk1B0 = D_00413F80.unk1B4 = 0.0f;
    D_00413F80.unk1B8 = D_00413F80.unk1BC = 0.0f;
    D_0038D640.x = D_0038D640.z = D_0038D640.y = 0.0f;
    PLAYER_STATUS_BITS(&D_00413F80)->bit34 = FALSE;
    D_0038CD20[D_00446770 == 0].unk00 = 0;
    func_00115878(D_00446770 == 0);
    func_001A3040(D_00446770 == 0);
    func_0011CE80(D_00446770 == 0);
    KF_Entity_FreeAllInGroup(D_00446770 == 0);
    func_00125560(D_00446770 == 0);
    func_001CACA0(D_00446770 == 0);
    func_001C8938(D_00446770 == 0);
    func_0019AAA0(D_00446770 == 0);
    func_001A15F8();
}
