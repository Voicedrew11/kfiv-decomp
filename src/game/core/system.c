/* What main runs at start-up, and the movies: the system set-up (System_Init), the display set-up
   (Display_Init) and the opening and ending movies (Movie_). core/memory.c and core/display.c follow it; System_Init
   and Display_Init call into core/display.c, so the three were probably one source file. */
#include "core/system.h"
#include "core/display.h"
#include "core/game.h"
#include "input/pad.h"
#include "gfx/gs.h"

extern int opening_movie_skipped; /* set once START was pressed during the opening movie */
extern char *map_file_area;        /* the memory area the movie player is given as its heap */
extern int map_file_area_size;          /* the size it is given for that heap */
extern int context1_buffer_half;          /* the half of the first double buffer in use; each swap flips it */
extern int context2_buffer_half;          /* the same for the second */
extern int display_spare_buffer_half;
extern u32 game_status_flags[];
extern s64 game_dbuff_clear_color[];
extern s64 game_dbuff_context2_clear_color[];
extern char opening_movie_path[];       /* "cdrom0:\DATA\KF4OP.PSS;1" */
extern char ending_movie_path[];        /* "cdrom0:\DATA\KF4ED.PSS;1" */
extern char staff_roll_movie_path[];    /* "cdrom0:\DATA\KF4STAFF.PSS;1" */
extern f32 identity_matrix[16];
extern f32 world_view_matrix[16];
extern f32 world_view_copy_matrix[16];
extern f32 projection_matrix[4][4];
extern GsDoubleBuffer game_dbuff;           /* the game's frame buffers, drawn through GS context 1 */
extern GsDoubleBuffer game_dbuff_context2;  /* the same frame buffers, drawn through GS context 2 */
extern GsRegisterPair frame_dispfb_regs;    /* the DISPFB values of the two frames */
extern GsRegisterPair frame_display_regs;   /* the DISPLAY values of the two frames */

int Movie_Play(char *, u32, u32, int (*)(void), u32, s16, s16, s16);
void Packet_SwapBuffers(void);
void DrawList_Clear(void);
void Packet_OnFrameStart(void);
void GsDraw_ResetDrawTarget(void);
void GsDraw_FillScreen(int, int, int, int, s64);
void Packet_OnFrameEnd(void);
void Packet_SendChain(void);
void Packet_WaitForSend(void);
void SoundDriver_ReleaseAll(void);
void Sound_Init(void);
int sceGsSyncV(int);
void Vram_Clear(int, int);
void sceDevVif0Reset(void);
void sceDevVif1Reset(void);
int sceDevVu0Pause();
int sceDevVu0SetTBit();
int sceDmaReset(int);
int sceGsResetGraph(int, int, int, int);
int sceGsResetPath();
int sceMcInit();
int sceSdRemoteInit();
int sceSifInitIopHeap();
void sceGsSetDefDBuff(void *, s16, s16, s16, s16, s16, s16);
void sceVu0CopyMatrix(void *, void *);

/* The opening movie's per-frame callback (given to Movie_Play by Movie_PlayOpening): polls the controller
   and returns TRUE, ending the movie, when START is pressed. */
static int Movie_PollSkip(void) {
    Pad_Update();
    if (Pad_GetReport(0, 0)->pressed & PAD_START) {
        opening_movie_skipped = TRUE;
        return TRUE;
    }
    return FALSE;
}

/* Plays the opening movie KF4OP.PSS, which START skips (opening_movie_skipped then set), then resets both double buffers'
   frame registers and the draw state, and swaps. */
void Movie_PlayOpening(void) {
    s64 temp;
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */

    opening_movie_skipped = 0;
    Movie_Play(opening_movie_path, (u32)map_file_area, map_file_area_size, Movie_PollSkip, 0x3FFF, 2, 0, 0);
    temp = (s64)0xC000 << 17;
    game_dbuff_clear_color[0] = temp;
    game_dbuff_clear_color[30] = temp;
    game_dbuff_context2_clear_color[0] = temp;
    game_dbuff_context2_clear_color[30] = temp;
    Packet_SwapBuffers();
    Packet_SwapBuffers();
    DrawList_Clear();
    Packet_OnFrameStart();
    context1_buffer_half = 0;
    context2_buffer_half = 0;
    GsDraw_ResetDrawTarget();
    GsDraw_FillScreen(0, 0, 0, 0x80, 0x100000044L);
    Packet_OnFrameEnd();
    context1_buffer_half = 1;
    context2_buffer_half = 1;
    Packet_SendChain();
    Packet_WaitForSend();
    Display_SwapBuffers(0);
    SoundDriver_ReleaseAll();
    Sound_Init();
}

/* Plays the ending movie KF4ED.PSS when bit 11 of game_status_flags is set, the staff roll KF4STAFF.PSS otherwise;
   neither can be skipped. */
void Movie_PlayEnding(void) {
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */

    opening_movie_skipped = 0;
    if ((game_status_flags[0] >> 11) & 1) {
        Movie_Play(ending_movie_path, (u32)map_file_area, map_file_area_size, 0, 0x3FFF, 2, 0, 0);
    } else {
        Movie_Play(staff_roll_movie_path, (u32)map_file_area, map_file_area_size, 0, 0x3FFF, 2, 0, 0);
    }
    sceGsSyncV(0);
    sceGsSyncV(0);
    Vram_Clear(0, 0x4000);
    SoundDriver_ReleaseAll();
    Sound_Init();
}

/* Returns TRUE; the 0x80 bytes of stack are unused. Nothing calls it. */
static int System_ReturnTrueStackPad(void) {
    volatile char buf[0x80];
    return 1;
}

/* The same as System_ReturnTrueStackPad. */
static int System_ReturnTrueStackPadCopy(void) {
    volatile char buf[0x80];
    return 1;
}

/* Reboots the IOP and loads its modules, then starts the memory cards, resets VIF0, VU0, VIF1, DMA and the GS,
   and starts the sound library and the IOP heap. */
void System_Init(void) {
    System_RebootIop();
    sceMcInit();
    sceDevVif0Reset();
    sceDevVu0Pause();
    sceDevVif1Reset();
    sceDevVu0SetTBit();
    sceDmaReset(1);
    sceGsResetPath();
    sceGsResetGraph(0, 1, 2, 0);
    sceSdRemoteInit();
    sceSifInitIopHeap();
}

/* Sets up the game's two double buffers (the second one draws through GS context 2), the view
   matrices and the projection. */
void Display_Init(void) {
    sceGsSetDefDBuff(&game_dbuff, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    game_dbuff.disp[0].dispfb.fbp = 0;
    game_dbuff.disp[1].dispfb.fbp = 0x8C;
    game_dbuff.draw0.frame.fbp = 0x8C;
    game_dbuff.draw1.frame.fbp = 0;
    game_dbuff.draw0.zbuf.zbp = 0x118;
    game_dbuff.draw1.zbuf.zbp = 0x118;
    sceVu0CopyMatrix(world_view_matrix, identity_matrix);
    sceVu0CopyMatrix(world_view_copy_matrix, identity_matrix);
    Display_BuildProjection(projection_matrix, 512.0f, 1.0f, 65536.0f);
    context1_buffer_half = 0;
    display_spare_buffer_half = 0;
    game_dbuff.clear1.rgbaq = game_dbuff.clear0.rgbaq = 0x100000000L;
    sceGsSetDefDBuff(&game_dbuff_context2, 0, 0x280, 0x1C0, 3, 0x3A, 1);
    game_dbuff_context2.disp[0].dispfb.fbp = 0;
    game_dbuff_context2.disp[1].dispfb.fbp = 0x8C;
    game_dbuff_context2.draw0.frame.fbp = 0x8C;
    game_dbuff_context2.draw1.frame.fbp = 0;
    game_dbuff_context2.draw0.zbuf.zbp = 0x118;
    game_dbuff_context2.draw1.zbuf.zbp = 0x118;
    game_dbuff_context2.draw0.frame_addr = 0x4D;
    game_dbuff_context2.draw0.zbuf_addr = 0x4F;
    game_dbuff_context2.draw0.xyoffset_addr = 0x19;
    game_dbuff_context2.draw0.scissor_addr = 0x41;
    game_dbuff_context2.draw0.test_addr = 0x48;
    game_dbuff_context2.clear0.test_a_addr = 0x48;
    game_dbuff_context2.clear0.prim |= 0x200;
    game_dbuff_context2.clear0.test_b_addr = 0x48;
    game_dbuff_context2.draw1.frame_addr = 0x4D;
    game_dbuff_context2.draw1.zbuf_addr = 0x4F;
    game_dbuff_context2.draw1.xyoffset_addr = 0x19;
    game_dbuff_context2.draw1.scissor_addr = 0x41;
    game_dbuff_context2.draw1.test_addr = 0x48;
    game_dbuff_context2.clear1.test_a_addr = 0x48;
    game_dbuff_context2.clear1.prim |= 0x200;
    game_dbuff_context2.clear1.test_b_addr = 0x48;
    context2_buffer_half = 0;
    frame_dispfb_regs.first = *(u64 *)&game_dbuff.disp[0].dispfb;
    frame_dispfb_regs.second = *(u64 *)&game_dbuff.disp[1].dispfb;
    frame_display_regs.first = game_dbuff.disp[0].display;
    frame_display_regs.second = game_dbuff.disp[1].display;
    // match: the four stores below come out in retail order only when written in this one
    game_dbuff_context2.draw1.xyoffset.ofy = 0x7200;
    game_dbuff_context2.draw0.xyoffset.ofy = 0x7200;
    game_dbuff.draw1.xyoffset.ofy = 0x7200;
    game_dbuff.draw0.xyoffset.ofy = 0x7200;
}
