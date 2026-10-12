#include "kf_player.h"

extern PlayerState D_00413F80;
extern int D_00446734;          /* nonzero: a saved game was loaded */

void func_001029A8(void);
void func_001B5810(void);
void func_002077F0(int);
void func_001030B8(void);
void func_001BA310(void);
void func_001127F8(void);
void func_001B1340(void);
void func_0019A848(void);
void func_001128E0(void);
void func_00117C60(void);
void func_0011A430(void);
void func_001BF788(void);
void KF_Entity_ResetAllLists(void);
void func_0011C6E8(void);
void func_001C8238(void);
void func_001A1610(void);
void func_001B4EE0(void);
void func_001CA818(void);
void func_00208158(void);
int func_00208FF8(void);
int func_001060C0(void);
void func_00104EC8(void);
void func_001B5A10(void);
void func_001BF898(void);
void func_001A1260(int);
void func_0019C520(void);

/* Resets every game system for a new session; a loaded save is then applied on top. */
void func_001000C8(int arg) {
    func_001029A8();
    func_001B5810();
    func_002077F0(arg);
    func_001030B8();
    func_001BA310();
    func_001127F8();
    func_001B1340();
    func_0019A848();
    func_001128E0();
    func_00117C60();
    func_0011A430();
    func_001BF788();
    KF_Entity_ResetAllLists();
    func_0011C6E8();
    func_001C8238();
    func_001A1610();
    func_001B4EE0();
    func_001CA818();
    func_00208158();
    func_00208FF8();
    if (D_00446734 != 0) {
        func_001060C0();
        func_00104EC8();
    }
    func_001B5A10();
    func_001BF898();
    func_001A1260(D_00413F80.unk3D);
    func_0019C520();
}
