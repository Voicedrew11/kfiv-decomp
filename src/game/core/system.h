/* Start-up: the system and display set-up, and the opening and ending movies (core/system.c). */
#ifndef KF_SYSTEM_H
#define KF_SYSTEM_H

#include "common.h"
#include "gfx/gs.h"

/* Plays the opening movie, which START skips, then sets the display up again. */
void Movie_PlayOpening(void);

/* Plays the ending movie or the staff roll. */
void Movie_PlayEnding(void);

/* Starts the IOP, the memory cards, VU0, DMA, the GS and the sound. */
void System_Init(void);

/* Sets up the two double buffers, the view matrices and the projection. */
void Display_Init(void);

/* data still in asm */
extern GsRegisterPair frame_dispfb_regs;  /* the DISPFB values of the two frames */
extern GsRegisterPair frame_display_regs;  /* the DISPLAY values of the two frames */
extern char opening_movie_path[];  /* "cdrom0:\DATA\KF4OP.PSS;1" */
extern char ending_movie_path[];  /* "cdrom0:\DATA\KF4ED.PSS;1" */
extern char staff_roll_movie_path[];  /* "cdrom0:\DATA\KF4STAFF.PSS;1" */
extern int opening_movie_skipped;  /* set once START was pressed during the opening movie */
extern int display_spare_buffer_half;  /* zeroed beside context1_buffer_half; nothing else uses it */

#endif
