/* Map loader step handlers, state, request and screen fade. */
#include "map/map.h"
#include "gfx/screen.h"

/* the step handlers (map/map_load.c and map/map_change.c) */
void MapLoad_StartRequest();
void MapLoad_WaitTimer();
void MapLoad_OpenFile();
void MapLoad_ReadChunk();
void MapLoad_WaitForChunk();
void MapLoad_WaitForSetUp();
void MapLoad_SetUpMap();
void MapLoad_SkipStep();
void MapLoad_ActivateMap();
void MapChange_FadeOutCapture();
void MapChange_DrawBlack();
void MapChange_FadeMapToBlack();
void MapChange_Idle();
void MapChange_WaitForIdle();
void MapChange_UnloadAll();
void MapChange_OpenFile();
void MapChange_WaitForRead();
void MapChange_SetUpMap();
void MapChange_EndWhenTimerDone();

void (*map_load_steps[12])(void) = { /* 0x38D5E0 what MapLoadState.step runs */
    MapLoad_StartRequest, MapLoad_WaitTimer,
    MapLoad_OpenFile, MapLoad_ReadChunk,
    MapLoad_WaitForChunk, MapLoad_WaitForSetUp,
    MapLoad_SetUpMap, MapLoad_SkipStep,
    MapLoad_ActivateMap, NULL,
    NULL, NULL,
};

MapLoadState map_load_state = { 0 }; /* 0x38D610 the background load of the other map slot */

MapLoadRequest map_load_request = { 0 }; /* 0x38D640 the map the game asked to change to */

void (*screen_fade_steps[8])(void) = { /* 0x38D650 what ScreenFade.step runs */
    MapChange_Idle, MapChange_WaitForIdle,
    MapChange_UnloadAll, MapChange_OpenFile,
    MapChange_WaitForRead, MapChange_SetUpMap,
    MapChange_EndWhenTimerDone, NULL,
};

void (*screen_fade_draw_steps[4])(void) = { /* 0x38D670 what ScreenFade.draw_step runs */
    MapChange_FadeOutCapture, MapChange_DrawBlack,
    MapChange_FadeMapToBlack, NULL,
};

// match: gfx/screen.h's ScreenFade is 0x2C bytes, retail's screen fade 0x30
typedef struct {
    ScreenFade fade;                /* 0x00 */
    int unk2C;                      /* 0x2C the four bytes retail keeps after the fade */
} ScreenFadeSlot;                   /* 0x30 */

ScreenFadeSlot screen_fade = { 0 }; /* 0x38D680 the screen fade of a map change */
