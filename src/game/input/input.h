/* Controller input: reading the ports, rumble, the default controls (input/input.c). */
#ifndef KF_INPUT_H
#define KF_INPUT_H

#include "common.h"

/* input/input.c */
void Input_StartRumble(u8 strength, u8 duration, u8 delay);
void Input_Update(void);
void Input_Init(void);
void Input_SetDefaultControls(void);
void Input_StopRumble(void);
void Input_PauseRumble(void);
void Input_ResumeRumble(void);
void Input_StartupHook(void);

/* data still in asm */
extern u8 rumble_active;
extern u8 rumble_strength;
extern u8 rumble_frames_left;
extern u8 rumble_frames;
extern u8 rumble_fades;
extern u8 rumble_paused;

#endif
