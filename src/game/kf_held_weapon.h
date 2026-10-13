#ifndef KF_HELD_WEAPON_H
#define KF_HELD_WEAPON_H

#include "common.h"

void KF_HeldWeapon_Init(void);
void KF_Player_LoadArmModels(void);
void KF_Player_UpdateArmModels(void);
void KF_WeaponTrail_Update(void);
void KF_Player_DrawArm(void);
void KF_WeaponTrail_PickKind(void);

#endif
