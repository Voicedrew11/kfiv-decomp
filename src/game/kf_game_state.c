/* The game status block and the loaded maps' state. */
#include "kf_game.h"
#include "kf_map.h"

GameStatus D_0038CC50 = { 0 }; /* 0x38CC50 the status of the running game */
/* Field aliases other files still use by their own names. */
asm(".globl D_0038CC57\nD_0038CC57 = D_0038CC50 + 0x7");  /* 0x38CC57 unk07 (kf_enemy_aqualizard.c) */
asm(".globl D_0038CC74\nD_0038CC74 = D_0038CC50 + 0x24");  /* 0x38CC74 the flag word */
asm(".globl D_0038CC80\nD_0038CC80 = D_0038CC50 + 0x30");  /* 0x38CC80 unk30 */
asm(".globl D_0038CC84\nD_0038CC84 = D_0038CC50 + 0x34");  /* 0x38CC84 save_area */

/* 0x38CCA0 default light block for map objects: one light (125, 110, 83) */
MapSlotUnk10Lights D_0038CCA0 __attribute__((aligned(16))) = {
    { { 0 } },
    { { 0 }, { 0 }, { 0 }, { 125.0f / 255.0f, 110.0f / 255.0f, 83.0f / 255.0f, 0.0f } },
};

MapSlot D_0038CD20[2] = { 0 }; /* 0x38CD20 the two loaded maps */
asm(".globl D_0038CD28\nD_0038CD28 = D_0038CD20 + 0x8");  /* 0x38CD28 slot 0's header (kf_sound_game.c) */

/* 0x38CDE0 the prop of each prop id, per map slot */
MapProp *D_0038CDE0[2][0x100] = { 0 };
