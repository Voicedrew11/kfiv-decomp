/* A 256x256 draw environment the game can switch to and back. */
#ifndef KF_DRAW_ENV_H
#define KF_DRAW_ENV_H

#include "common.h"

/* Puts a 256x256 16-bit draw environment at frame page 0x1C2 with the data block D_0043EB90, waits, and puts
   the normal one back */
void func_0019B360(void);
/* Puts the draw environment of func_0019B360 and waits, without putting the normal one back */
void func_0019B4B0(void);

#endif
