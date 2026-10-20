#ifndef KF_HELD_WEAPON_H
#define KF_HELD_WEAPON_H

#include "common.h"
#include "player/player.h"

void HeldWeapon_Init(void);
void Player_LoadArmModels(void);
void Player_UpdateArmModels(void);
void WeaponTrail_Update(void);
void Player_DrawArm(void);
void WeaponTrail_PickKind(void);

/* player/held_weapon.c */
extern PlayerArmPoints held_item_arm_points;
extern PlayerArmPoints held_item_arm_points_prev;
void WeaponTrail_Reset(void);
void WeaponTrail_SetMinLife(s32 min_life);
void WeaponTrail_Age(void);
void WeaponTrail_Push(void *position);
void WeaponTrail_GetPoint(void *dst, s32 index);
s32 WeaponTrail_GetLife(s32 index);
int WeaponTrail_GetCount(void);

/* data still in asm */
extern char arm_phong_texture_path[];
extern char item_arm_path_format[];
extern char default_held_item_path[];
extern char default_arm_path[];
extern int loaded_arm_gauntlet;
extern int loaded_held_weapon;

#endif
