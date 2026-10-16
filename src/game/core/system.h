/* Start-up: the system and display set-up, and the opening and ending movies (core/system.c). */
#ifndef KF_SYSTEM_H
#define KF_SYSTEM_H

#include "common.h"

/* Plays the opening movie, which START skips, then sets the display up again. */
void Movie_PlayOpening(void);

/* Plays the ending movie or the staff roll. */
void Movie_PlayEnding(void);

/* Starts the IOP, the memory cards, VU0, DMA, the GS and the sound. */
void System_Init(void);

/* Sets up the two double buffers, the view matrices and the projection. */
void Display_Init(void);

#endif
