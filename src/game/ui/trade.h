/* The trade, smith and save screens (ui/trade.c). */
#ifndef KF_TRADE_H
#define KF_TRADE_H

#include "common.h"
#include "core/state.h"
#include "ui/menu.h"

/* Runs the trade screens of trade table `value` until they are left. */
void Trade_Run(s32 value);

/* Runs the smith's screens until they are left. */
void Smith_Run(void);

/* Runs the save screen until it is left. */
void SaveScreen_Run(void);

/* data still in asm */
extern f32 message_part_alphas[0x100];  /* the fade level of each part of the shop's (smith's) message window */
extern s32 save_line_slots[3];  /* the save slot each of the three lines shows */
extern s32 save_line_write_slots[3];  /* the save slot a save from each line is written to */
extern TradeValue bones_label[4];
extern TradeValue scales_label[4];
extern TradeValue claws_label[4];
extern TradeValue trade_values_1[];
extern TradeValue trade_values_2[];
extern TradeValue trade_values_3[];
extern TradeValue trade_values_4[];
extern TradeValue trade_values_5[];
extern TradeValue trade_values_6[];
extern f32 trade_stock_list_model_position[4];  /* where Trade_UpdateStockList draws the item's model */
extern f32 trade_take_amount_model_position[4];
extern f32 trade_owned_list_model_position[4];
extern f32 trade_give_amount_model_position[4];
extern f32 smith_upgrade_list_model_position[4];
extern f32 smith_upgrade_confirm_model_position[4];
extern f32 smith_confirm_list_model_position[4];
extern f32 smith_item_screen_model_position[4];
extern f32 smith_collect_list_model_position[4];
/* the ring of items that may be handed over (item_list_cursor; the smith's ring of items to hand in) */
extern MenuItem *handover_list_first;  /* its first node */
extern MenuItem *handover_list_last_page;  /* the row the window starts at on the ring's last page */
extern s32 handover_list_count;  /* how many nodes the ring has */
extern u8 menu_draw_color_alpha;  /* the alpha byte of menu_draw_color */
extern u8 model_draw_color_alpha;  /* the alpha byte of model_draw_color */
extern u8 trade_panel_shown;  /* TRUE while the selected item's panel is up */
extern void (*trade_handler)(HandlerContext *);  /* the screen Trade_RunHandler runs */
extern int trade_step;  /* which of trade_steps runs */
extern s32 trade_run_step;  /* which of trade_run_steps runs */
extern s32 trade_stock_list_panel_sound_pending;  /* TRUE until the panel's opening sound has played */
extern f32 trade_stock_list_model_angle;  /* the model's turn */
extern s32 trade_take_amount_panel_sound_pending;
extern f32 trade_take_amount_model_angle;
extern f32 trade_owned_list_model_angle;
extern f32 trade_give_amount_model_angle;
extern void (*smith_handler)(HandlerContext *);  /* the screen Smith_RunHandler runs */
extern s32 smith_step;  /* which of smith_steps runs */
extern s32 smith_run_step;  /* which of smith_run_steps runs */
extern s8 smith_choices_active;  /* nonzero once the cursor is on the three lines */
extern s32 smith_upgrade_list_panel_sound_pending;
extern f32 smith_upgrade_list_model_angle;
extern s32 smith_upgrade_confirm_panel_sound_pending;
extern f32 smith_upgrade_confirm_model_angle;
extern s32 smith_confirm_list_panel_sound_pending;
extern f32 smith_confirm_list_model_angle;
extern f32 smith_item_screen_model_angle;
extern s32 smith_collect_list_panel_sound_pending;
extern f32 smith_collect_list_model_angle;
extern int (*save_screen_handler)();  /* the screen SaveScreen_RunHandler runs */
extern int save_screen_step;  /* which of save_screen_steps runs */
extern s32 save_screen_run_step;  /* which of save_screen_run_steps runs */
extern s32 trade_pad_change_frames;  /* counts 20 frames without buttons after the pad type changed */
extern s32 smith_pad_change_frames;  /* counts 20 frames without buttons after the pad type changed */
extern int save_screen_pad_change_frames;  /* counts 20 frames without buttons after the pad type changed */
extern s8 trade_amount_down_locked;  /* TRUE stops LEFT lowering the take amount; the game only clears it */
extern s8 trade_screen_state;  /* 0: fading in, 1: open, 2 and up: fading out to the next screen */
extern s8 trade_panel_state;  /* the panel's fade: 0 in, 1 open, 2 out and close, 3 out */
extern s8 smith_confirm_shown;  /* TRUE while Smith_UpdateListWithConfirm shows its question */
extern s8 smith_unread_flag;  /* cleared with the smith screen state; nothing sets or reads it */
extern s8 smith_upgrade_choice;  /* which of the three lines of Smith_UpdateItemScreen is picked */
extern s8 smith_screen_state;  /* 0: fading in, 1: open, 2 to 4: fading out to a list, 5: fading out to leave */
extern s8 smith_panel_state;  /* the same for the selected item's panel */
extern s8 modal_input_locked;  /* TRUE while a trade or smith screen fades: the buttons are not read */
extern s32 modal_pad_mode;  /* Pad_GetMode of pad 0 this frame; 0xFF: no pad */
extern s32 modal_pad_mode_last;  /* the same, last frame */
extern s32 save_lines_reorder_pending;  /* TRUE while a save into an empty slot runs: the lines are reordered after it */
extern s32 modal_even_field;  /* what Menu_EndFrame returned (the field); never read */
extern HandlerContext trade_context;  /* what the trade steps and handlers get */
extern HandlerContext smith_context;  /* what the smith steps and handlers get */
extern HandlerContext save_screen_context;  /* what the save screen steps get */
extern MenuItem *trade_stock_list_top[4];  /* the stock list's first row on screen */
extern MenuItem *trade_stock_list_page_mark[4];  /* the stock list's page mark: its prev is the last row on screen */
extern MenuItem *trade_stock_list_cursor[4];  /* the stock item under the cursor */
extern MenuItem *smith_collect_list_top[4];  /* the same three for the list of items to collect */
extern MenuItem *smith_collect_list_page_mark[4];
extern MenuItem *smith_collect_list_cursor[];

#endif
