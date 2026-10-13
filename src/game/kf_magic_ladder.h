#ifndef KF_MAGIC_LADDER_H
#define KF_MAGIC_LADDER_H

#include "kf_entity.h"

int Player_CastMagic(int magic_id);
void Player_HoldMagic(int magic_id);
void Ladder_Start(ObjectReference *object, int from_far_end);
void Ladder_CancelApproach(void);
int Ladder_DispatchPad(u16 held, u16 pressed);
void Ladder_WalkToFoot(void);
void Ladder_WaitOnRung(u16 buttons);
void Ladder_ClimbRungUp(u16 buttons);
void Ladder_ClimbRungDown(u16 buttons);
int Ladder_PlayStepSound(void);

#endif
