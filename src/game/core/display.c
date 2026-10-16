/* The second half of the start-up code after core/memory.c: the IOP reboot that System_Init runs, the projection
   matrix of Display_Init, the memory reset main runs, and the frame buffer swaps (Display_). */
#include "core/display.h"
#include "core/game.h"
#include "gfx/gs.h"
#include "core/memory.h"
#include "core/timer.h"

extern char ioprp_image_path[];         /* "cdrom0:\IOPRP224.IMG;1" */
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
extern char empty_module_args[];        /* "": no module arguments */
extern char vector4_print_format[];     /* "(%f,%f,%f,%f)\n" */
static char menu_heap_area[0x200000];   /* 0x1BFAF80 the area the menus' heap (menu_heap) lies in */
extern ClockControl D_00432820;
extern s32 menu_heap;
extern GsDoubleBuffer game_dbuff;           /* the game's frame buffers, drawn through GS context 1 */
extern GsDoubleBuffer game_dbuff_context2;  /* the same frame buffers, drawn through GS context 2 */
extern int context1_buffer_half;              /* the half of the first double buffer in use; each swap flips it */
extern int context2_buffer_half;              /* the same for the second */
extern int vsync_even_field;        /* TRUE when the last vsync was of field 0 */

void sceSifInitRpc(int);
void File_InitDrive(void);
int sceSifRebootIop(const char *);
int sceSifSyncIop(void);
void sceFsReset(void);
int sceSifLoadModule(const char *, int, const char *);
int printf(const char *, ...);
void sceVu0UnitMatrix(f32 (*)[4]);
int FlushCache(int);
int sceGsSetHalfOffset(void *, int, int, s16);
int sceGsSwapDBuff(void *, int);
int sceGsSyncV(int);


/* Reboots the IOP with IOPRP224.IMG and loads the nine IOP modules (pad, memory card, sound and file I/O). */
void System_RebootIop(void) {
    sceSifInitRpc(0);
    File_InitDrive();
    while (!sceSifRebootIop(ioprp_image_path)) {
    }
    while (!sceSifSyncIop()) {
    }
    sceSifInitRpc(0);
    File_InitDrive();
    sceFsReset();
    sceSifLoadModule(sio2man_module_path, 0, empty_module_args);
    sceSifLoadModule(padman_module_path, 0, empty_module_args);
    sceSifLoadModule(mcman_module_path, 0, empty_module_args);
    sceSifLoadModule(mcserv_module_path, 0, empty_module_args);
    sceSifLoadModule(libsd_module_path, 0, empty_module_args);
    sceSifLoadModule(modmidi_module_path, 0, empty_module_args);
    sceSifLoadModule(modhsyn_module_path, 0, empty_module_args);
    sceSifLoadModule(sdrdrv_module_path, 0, empty_module_args);
    sceSifLoadModule(fsiopsnd_module_path, 0, empty_module_args);
}

/* Prints the four floats at coords as "(x,y,z,w)". Nothing calls it. */
static void Debug_PrintVector4(float *coords) {
    printf(vector4_print_format, coords[0], coords[1], coords[2], coords[3]);
}

/* Builds a perspective matrix from a screen scale and the near and far plane distances. */
void Display_BuildProjection(f32 (*matrix)[4], f32 screen_scale, f32 near_plane, f32 far_plane) {
    sceVu0UnitMatrix(matrix);
    matrix[1][1] = matrix[0][0] = screen_scale * 0.0009765625f;
    matrix[2][2] = (far_plane + near_plane) / (far_plane - near_plane);
    matrix[3][2] = far_plane * near_plane * -2.0f / (far_plane - near_plane);
    matrix[2][3] = 1.0f;
    matrix[3][3] = 0.0f;
}

/* Lays out the memory areas (Memory_Init), resets the clock control's four bytes at 0x208 and points
   menu_heap at menu_heap_area. */
void System_InitMemory(void) {
    Memory_Init();
    D_00432820.unk208 = 0;
    D_00432820.unk209 = 0;
    D_00432820.unk20A = 0;
    D_00432820.unk20B = 2;
    menu_heap = (s32)menu_heap_area;
}

/* Empty. */
static void func_0020B6E8(void) {
}

/* Empty. */
static void func_0020B6F0(void) {
}

/* Empty. */
static void func_0020B6F8(void) {
}

/* Empty. */
static void func_0020B700(void) {
}

/* Waits for the vsync, sets the half-pixel offset of the half about to be drawn to for the field, and swaps the
   first double buffer only. Nothing calls it. */
static void Display_SwapMainBuffer(void) {
    void *draw_env;

    vsync_even_field = sceGsSyncV(0) == 0;
    if (context1_buffer_half != 0) {
        draw_env = &game_dbuff.draw1;
    } else {
        draw_env = &game_dbuff.draw0;
    }
    sceGsSetHalfOffset(draw_env, 0x800, 0x800, (s16)vsync_even_field);
    FlushCache(0);
    sceGsSwapDBuff(&game_dbuff, context1_buffer_half);
    context1_buffer_half = context1_buffer_half == 0;
}

/* Swaps both double buffers. It waits for the vsync first, unless allow_late_swap is set and more than 0x4B0C7
   timer ticks have passed since the timer was last restarted. */
void Display_SwapBuffers(int allow_late_swap) {
    int elapsed = Timer_GetTicks();

    if (allow_late_swap == 0 || elapsed <= 0x4B0C7) {
        while (sceGsSyncV(0) != 1) {
        }
    }
    FlushCache(0);
    sceGsSwapDBuff(&game_dbuff, context1_buffer_half);
    context1_buffer_half = context1_buffer_half == 0;
    FlushCache(0);
    sceGsSwapDBuff(&game_dbuff_context2, context2_buffer_half);
    context2_buffer_half = context2_buffer_half == 0;
    FlushCache(0);
}

/* Waits for a vsync, then sets both double buffers to the half not in use, without flipping context1_buffer_half and
   context2_buffer_half. Nothing calls it. */
static void Display_ReapplyLastSwap(void) {
    do {
    } while (sceGsSyncV(0) == 0);
    FlushCache(0);
    sceGsSwapDBuff(&game_dbuff, context1_buffer_half == 0);
    FlushCache(0);
    sceGsSwapDBuff(&game_dbuff_context2, context2_buffer_half == 0);
}

/* Empty. */
static void func_0020B8A8(void) {
}
