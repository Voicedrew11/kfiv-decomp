/* Mode handlers: the context a per-mode handler table call receives, and the mode state record. */
#ifndef KF_STATE_H
#define KF_STATE_H

#include "common.h"

/* trade_context, smith_context, save_screen_context: passed to the handler picked from a mode table */
typedef struct {
    int unk00;                      /* 0x00 */
    int unk04;                      /* 0x04 */
    int arg;                        /* 0x08 */
} HandlerContext;

/* save_screen_state: the save screen SaveScreen_UpdateCardSelect runs each frame */
typedef struct {
    s32 unk00;                      /* 0x00 */
    char pad04[0x04];
    s32 unk08;                      /* 0x08 */
    s8 unk0C;                       /* 0x0C the selected card, 0 or 1 */
    s8 unk0D;                       /* 0x0D the yes/no cursor of a question */
    s8 unk0E;                       /* 0x0E */
    char pad0F[0x02];
    s8 unk11;                       /* 0x11 set when the screen is being left */
    s8 unk12;                       /* 0x12 what the screen shows, -1: the card list */
    s8 unk13;                       /* 0x13 */
    s8 unk14;                       /* 0x14 set while a card operation runs */
    char pad15[0x03];
    s8 unk18;                       /* 0x18 set while input is ignored */
    char pad19[0x02];
    s8 unk1B;                       /* 0x1B */
    char pad1C[0x01];
    s8 unk1D;                       /* 0x1D */
    s8 unk1E;                       /* 0x1E */
    s8 unk1F;                       /* 0x1F */
    s8 unk20;                       /* 0x20 */
    s8 unk21;                       /* 0x21 set when a choice was confirmed */
    char pad22[0x0E];
} SaveScreen;

extern SaveScreen save_screen_state;

/* what the sprite_handlers build; MenuSprite_Draw dispatches it by kind */
typedef struct {
    char pad00[0x31];
    u8 kind;                        /* 0x31 picks the function that consumes the object */
} KindObject;

/* the 0x60-byte objects slot_parts points at, drawn by MenuSprite_Draw and MenuSprite_DrawPlain (preshop_parts
   is taken to point at the same kind of object: inferred from the matching offsets, not proven) */
typedef struct {
    char pad00[0x20];
    u32 pad20 : 24;
    u32 unk23 : 8;                  /* 0x23 the level its fade runs to */
    char pad24[0x1A];
    u8 unk3E;                       /* 0x3E the kind */
    s8 unk3F;                       /* 0x3F */
    char pad40[0x02];
    u8 unk42;                       /* 0x42 passed to Menu_SetBlendMode while it is drawn */
    char pad43[0x1D];
} Entry60;

#endif
