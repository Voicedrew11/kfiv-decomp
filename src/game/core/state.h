/* Mode handlers: the context a per-mode handler table call receives, and the mode state record. */
#ifndef KF_STATE_H
#define KF_STATE_H

#include "common.h"

/* trade_context, smith_context, save_screen_context: passed to the handler picked from a mode table */
typedef struct {
    int pad00;                      /* 0x00 */
    int active_list;                /* 0x04 0 or 1: the list the cursor is in (trade_list_handlers) */
    int arg;                        /* 0x08 */
} HandlerContext;

/* save_screen_state: the save screen SaveScreen_UpdateCardSelect runs each frame */
typedef struct {
    s32 card_update_result;         /* 0x00 SaveCard_Update's result: negative marks a card failure */
    char pad04[0x04];
    s32 free_clusters;              /* 0x08 the selected card's free cluster count when last read */
    s8 selected_card;               /* 0x0C the selected card, 0 or 1 */
    s8 selected_save;               /* 0x0D the yes/no cursor of a question */
    s8 confirm_choice;              /* 0x0E the yes/no cursor of the question: 0 saves, 1 cancels */
    char pad0F[0x02];
    s8 is_leaving;                  /* 0x11 set when the screen is being left */
    s8 dialog_id;                   /* 0x12 what the screen shows, -1: the card list */
    s8 list_step;                   /* 0x13 the step of the save list screen; -1: waiting for input */
    s8 card_op_running;             /* 0x14 set while a card operation runs */
    char pad15[0x03];
    s8 input_locked;                /* 0x18 set while input is ignored */
    char pad19[0x02];
    s8 check_finished;              /* 0x1B 0 until the first card check has finished */
    char pad1C[0x01];
    s8 confirm_press_state;         /* 0x1D how the confirm press is stepped: 0 idle, 3 fires, 1 and 2 wait */
    s8 save_state_0;                /* 0x1E what SaveCard_GetSaveState says of save 0 (1: there is one) */
    s8 save_state_1;                /* 0x1F what SaveCard_GetSaveState says of save 1 */
    s8 save_state_2;                /* 0x20 what SaveCard_GetSaveState says of save 2 */
    s8 confirm_pending;             /* 0x21 set when a choice was confirmed */
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
    u32 fade_level : 8;             /* 0x23 the level its fade runs to */
    char pad24[0x1A];
    u8 kind;                        /* 0x3E the kind */
    s8 pad3F;                       /* 0x3F */
    char pad40[0x02];
    u8 blend_mode;                  /* 0x42 passed to Menu_SetBlendMode while it is drawn */
    char pad43[0x1D];
} Entry60;

#endif
