/* The second half of the start-up code after kf_memory.c: the IOP reboot that System_Init runs, the projection
   matrix of Display_Init, the memory reset main runs, and the frame buffer swaps (Display_). */
#include "kf_display.h"
#include "kf_game.h"
#include "kf_gs.h"
#include "kf_memory.h"
#include "kf_timer.h"

extern char D_00441350[];           /* "cdrom0:\IOPRP224.IMG;1" */
extern char D_00441368[];           /* the nine IOP modules: SIO2MAN, PADMAN, MCMAN, MCSERV, LIBSD, MODMIDI, */
extern char D_00441388[];           /* MODHSYN, SDRDRV and FSIOPSND.IRX */
extern char D_004413A8[];
extern char D_004413C0[];
extern char D_004413E0[];
extern char D_004413F8[];
extern char D_00441418[];
extern char D_00441438[];
extern char D_00441458[];
extern char D_00446F90[];           /* "": no module arguments */
extern char D_00441478[];           /* "(%f,%f,%f,%f)\n" */
extern char D_01BFAF80[];
extern ClockControl D_00432820;
extern s32 D_00446F38;
extern GsDoubleBuffer D_00432D20;
extern GsDoubleBuffer D_00432F50;
extern int D_00446F78;              /* the half of the first double buffer in use; each swap flips it */
extern int D_00446F7C;              /* the same for the second */
extern int D_00446FA0;              /* TRUE when the last vsync was of field 0 */

void sceSifInitRpc(int);
void func_00102150(void);
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
    func_00102150();
    while (!sceSifRebootIop(D_00441350)) {
    }
    while (!sceSifSyncIop()) {
    }
    sceSifInitRpc(0);
    func_00102150();
    sceFsReset();
    sceSifLoadModule(D_00441368, 0, D_00446F90);
    sceSifLoadModule(D_00441388, 0, D_00446F90);
    sceSifLoadModule(D_004413A8, 0, D_00446F90);
    sceSifLoadModule(D_004413C0, 0, D_00446F90);
    sceSifLoadModule(D_004413E0, 0, D_00446F90);
    sceSifLoadModule(D_004413F8, 0, D_00446F90);
    sceSifLoadModule(D_00441418, 0, D_00446F90);
    sceSifLoadModule(D_00441438, 0, D_00446F90);
    sceSifLoadModule(D_00441458, 0, D_00446F90);
}

/* Prints the four floats at coords as "(x,y,z,w)". Nothing calls it. */
static void Debug_PrintVector4(float *coords) {
    printf(D_00441478, coords[0], coords[1], coords[2], coords[3]);
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
   D_00446F38 at D_01BFAF80. */
void System_InitMemory(void) {
    Memory_Init();
    D_00432820.unk208 = 0;
    D_00432820.unk209 = 0;
    D_00432820.unk20A = 0;
    D_00432820.unk20B = 2;
    D_00446F38 = (s32)D_01BFAF80;
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

    D_00446FA0 = sceGsSyncV(0) == 0;
    if (D_00446F78 != 0) {
        draw_env = &D_00432D20.draw1;
    } else {
        draw_env = &D_00432D20.draw0;
    }
    sceGsSetHalfOffset(draw_env, 0x800, 0x800, (s16)D_00446FA0);
    FlushCache(0);
    sceGsSwapDBuff(&D_00432D20, D_00446F78);
    D_00446F78 = D_00446F78 == 0;
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
    sceGsSwapDBuff(&D_00432D20, D_00446F78);
    D_00446F78 = D_00446F78 == 0;
    FlushCache(0);
    sceGsSwapDBuff(&D_00432F50, D_00446F7C);
    D_00446F7C = D_00446F7C == 0;
    FlushCache(0);
}

/* Waits for a vsync, then sets both double buffers to the half not in use, without flipping D_00446F78 and
   D_00446F7C. Nothing calls it. */
static void func_0020B840(void) {
    do {
    } while (sceGsSyncV(0) == 0);
    FlushCache(0);
    sceGsSwapDBuff(&D_00432D20, D_00446F78 == 0);
    FlushCache(0);
    sceGsSwapDBuff(&D_00432F50, D_00446F7C == 0);
}

/* Empty. */
static void func_0020B8A8(void) {
}
