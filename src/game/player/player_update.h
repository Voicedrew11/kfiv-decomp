#ifndef KF_PLAYER_UPDATE_H
#define KF_PLAYER_UPDATE_H

#include "common.h"

/* Held weapon model vtable layout for Player_UpdateWeaponAim. */
struct PlayerWeaponModel {
    char pad0[0x1B4];
    void *(*update_aim)(void *);
    void *(*get_point)(void *, int);
};

void Player_Update(void);
void Player_DrawScreenTints(void);
void Player_UpdateScreenTints(void);
void Player_UpdateActions(u16 held, u16 pressed);
void Player_SwingWeapon(u16 held, u16 pressed);
void Player_InvokeTargetHandler(u16 target_id, u16 command);
void Player_FireProjectile(u16 held, u16 pressed);
void Player_DrawBow(u16 held, u16 pressed);
void Player_PlayEquipSound(u16 held, u16 pressed);
void Player_FinishWeaponMotion(u16 held, u16 pressed);
void Player_HolsterWeapon(u16 held, u16 pressed);
int Player_CanFireAtAim(void);
void Player_UpdateWeaponAim(void);
void Player_Move(u16 held, u16 pressed); // match: retail passes both pad words; only held is used

#endif
