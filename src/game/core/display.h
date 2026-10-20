/* IOP start-up, the projection matrix, the memory reset and the frame buffer swaps (core/display.c). */
#ifndef KF_DISPLAY_H
#define KF_DISPLAY_H

#include "common.h"
#include "core/game.h"
#include "gfx/gs.h"

/* Reboots the IOP with its replacement image and loads the game's nine IOP modules. */
void System_RebootIop(void);

/* Builds a perspective matrix from a screen scale and the near and far plane distances. */
void Display_BuildProjection(f32 (*matrix)[4], f32 screen_scale, f32 near_plane, f32 far_plane);

/* Lays out the memory areas and resets the clock control bytes at 0x208. */
void System_InitMemory(void);

/* Swaps both double buffers after the vsync, or at once when allow_late_swap is set and the frame is late. */
void Display_SwapBuffers(int allow_late_swap);

/* data still in asm */
extern ClockControl game_clock_control;
extern GsDoubleBuffer game_dbuff;  /* the game's frame buffers, drawn through GS context 1 */
extern GsDoubleBuffer game_dbuff_context2;  /* the same frame buffers, drawn through GS context 2 */
extern char ioprp_image_path[];  /* "cdrom0:\IOPRP224.IMG;1" */
/* the paths of the nine IOP modules, "cdrom0:\IOP\SIO2MAN.IRX;1" and so on */
extern char sio2man_module_path[];
extern char padman_module_path[];
extern char mcman_module_path[];
extern char mcserv_module_path[];
extern char libsd_module_path[];
extern char modmidi_module_path[];
extern char modhsyn_module_path[];
extern char sdrdrv_module_path[];
extern char fsiopsnd_module_path[];
extern char vector4_print_format[];  /* "(%f,%f,%f,%f)\n" */
extern s32 menu_heap;  /* the base of the menus' heap */
extern int context2_buffer_half;  /* the same as context1_buffer_half, for the second double buffer */
extern char empty_module_args[];  /* "": no module arguments */
extern int vsync_even_field;  /* TRUE when the last vsync was of field 0 */

#endif
