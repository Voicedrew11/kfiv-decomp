#include "kf_gold.h"
#include "kf_held_weapon.h"
#include "kf_item_effect.h"
#include "kf_linked_point.h"
#include "kf_player.h"
#include "kf_shot.h"
#include "kf_splash.h"
#include "kf_effect.h"

extern PlayerState D_00413F80;
extern int D_00446734;          /* nonzero: a saved game was loaded */

void func_001029A8(void);
void func_001B5810(void);
void Ini_Load(int);
void func_001030B8(void);
void func_001BA310(void);
void func_001127F8(void);
void func_001128E0(void);
void func_00117C60(void);
void func_0011A430(void);
void Entity_ResetAllLists(void);
void func_0011C6E8(void);
void func_001C8238(void);
void func_001B4EE0(void);
int func_001060C0(void);
void func_00104EC8(void);
void func_001B5A10(void);
void Sound_LoadMgBank(int);
void func_0019C520(void);

/* Resets every game system for a new session; a loaded save is then applied on top. */
void func_001000C8(int arg) {
    func_001029A8();
    func_001B5810();
    Ini_Load(arg);
    func_001030B8();
    func_001BA310();
    func_001127F8();
    Shot_ClearAll();
    LinkedPoint_Init();
    func_001128E0();
    func_00117C60();
    func_0011A430();
    HeldWeapon_Init();
    Entity_ResetAllLists();
    func_0011C6E8();
    func_001C8238();
    Effect_Init();
    func_001B4EE0();
    Gold_Init();
    Splash_Reset();
    ItemEffect_Reset();
    if (D_00446734 != 0) {
        func_001060C0();
        func_00104EC8();
    }
    func_001B5A10();
    Player_LoadArmModels();
    Sound_LoadMgBank(D_00413F80.equipped_magic);
    func_0019C520();
}
