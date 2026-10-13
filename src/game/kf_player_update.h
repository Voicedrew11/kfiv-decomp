#ifndef KF_PLAYER_UPDATE_H
#define KF_PLAYER_UPDATE_H

#include "common.h"

/* Held weapon model vtable layout for KF_Player_UpdateWeaponAim. */
struct PlayerWeaponModel {
    char pad0[0x1B4];
    void *(*update_aim)(void *);
    void *(*get_point)(void *, int);
};

void KF_Player_Update(void);
void KF_Player_DrawScreenTints(void);
void KF_Player_UpdateScreenTints(void);
void KF_Player_UpdateActions(u16 held, u16 pressed);
void KF_Player_SwingWeapon(u16 held, u16 pressed);
void KF_Player_InvokeTargetHandler(u16 target_id, u16 command);
void KF_Player_FireProjectile(u16 held, u16 pressed);
void KF_Player_DrawBow(u16 held, u16 pressed);
void KF_Player_PlayEquipSound(void);
void KF_Player_FinishWeaponMotion(void);
void KF_Player_HolsterWeapon(void);
int KF_Player_CanFireAtAim(void);
void KF_Player_UpdateWeaponAim(void);
void KF_Player_Move(u16 held, u16 pressed); // match: retail passes both pad words; only held is used

#endif
