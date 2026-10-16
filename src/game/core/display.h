/* IOP start-up, the projection matrix, the memory reset and the frame buffer swaps (core/display.c). */
#ifndef KF_DISPLAY_H
#define KF_DISPLAY_H

#include "common.h"

/* Reboots the IOP with its replacement image and loads the game's nine IOP modules. */
void System_RebootIop(void);

/* Builds a perspective matrix from a screen scale and the near and far plane distances. */
void Display_BuildProjection(f32 (*matrix)[4], f32 screen_scale, f32 near_plane, f32 far_plane);

/* Lays out the memory areas and resets the clock control bytes at 0x208. */
void System_InitMemory(void);

/* Swaps both double buffers after the vsync, or at once when allow_late_swap is set and the frame is late. */
void Display_SwapBuffers(int allow_late_swap);

#endif
