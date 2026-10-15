/* Debug dumps to host0 (VRAM, CLUT, screenshots, data) and the VRAM clear. */
#ifndef KF_SCREEN_DUMP_H
#define KF_SCREEN_DUMP_H

#include "common.h"

/* Fills VRAM from first_row up to end_row with zeros in 128x128 blocks, then flushes the texture cache */
void Vram_Clear(int first_row, int end_row);
/* Stores the screen into a 16-byte aligned buffer, turns it into the rows of a .bmp behind it (bottom row
   first, blue first) and writes that to the first unused numbered screenshot file on host0 */
void ScreenDump_Bmp(u8 *buffer);

#endif
