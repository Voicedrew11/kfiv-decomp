/* Map loader step handlers, state, request and screen fade. */
#include "kf_map.h"
#include "kf_screen.h"

/* the step handlers (kf_map_load.c and kf_map_change.c) */
void func_001181B8();
void func_00118270();
void func_001182A0();
void func_00118458();
void func_001184F8();
void func_00118538();
void func_00118540();
void func_00118668();
void func_00118680();
void func_00118A98();
void func_00118C28();
void func_00118CA8();
void func_00118D78();
void func_00118D80();
void func_00118E20();
void func_00118F40();
void func_00119068();
void func_001190C8();
void func_00119340();

void (*map_load_steps[12])(void) = { /* 0x38D5E0 what MapLoadState.step runs */
    func_001181B8, func_00118270,
    func_001182A0, func_00118458,
    func_001184F8, func_00118538,
    func_00118540, func_00118668,
    func_00118680, NULL,
    NULL, NULL,
};

MapLoadState map_load_state = { 0 }; /* 0x38D610 the background load of the other map slot */

MapLoadRequest map_load_request = { 0 }; /* 0x38D640 the map the game asked to change to */

void (*screen_fade_steps[8])(void) = { /* 0x38D650 what ScreenFade.step runs */
    func_00118D78, func_00118D80,
    func_00118E20, func_00118F40,
    func_00119068, func_001190C8,
    func_00119340, NULL,
};

void (*screen_fade_draw_steps[4])(void) = { /* 0x38D670 what ScreenFade.draw_step runs */
    func_00118A98, func_00118C28,
    func_00118CA8, NULL,
};

// match: kf_screen.h's ScreenFade is 0x2C bytes, retail's screen fade 0x30
typedef struct {
    ScreenFade fade;                /* 0x00 */
    int unk2C;                      /* 0x2C the four bytes retail keeps after the fade */
} ScreenFadeSlot;                   /* 0x30 */

ScreenFadeSlot screen_fade = { 0 }; /* 0x38D680 the screen fade of a map change */
