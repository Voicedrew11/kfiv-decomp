#ifndef KF_HELD_WEAPON_H
#define KF_HELD_WEAPON_H

#include "common.h"

void HeldWeapon_Init(void);
void Player_LoadArmModels(void);
void Player_UpdateArmModels(void);
void WeaponTrail_Update(void);
void Player_DrawArm(void);
void WeaponTrail_PickKind(void);

#endif
