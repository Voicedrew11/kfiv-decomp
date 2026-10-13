#include "kf_game.h"
#include "kf_pad.h"

extern PadState D_00413DB8[2];


/* Reads the controller in a port into its PadState; the sticks count as direction buttons unless
   the matching buttons are held. */
void func_001B5590(int port) {
    PadState *pad = &D_00413DB8[port];
    PadReport *report;

    pad->type = KF_Pad_GetMode(port, 0);
    if (pad->type != 0xFF) {
        report = KF_Pad_GetReport(port, 0);
        pad->held = report->held;
        pad->pressed = report->pressed;
        if (pad->type == 1 && ((PadReportAxes *)report)->right_stick != 0) {
            pad->prev_look_buttons = pad->look_buttons;
            pad->look_buttons = 0;
            if (report->right_stick_y < 0x40) {
                pad->look_buttons = 0x200;
            }
            if (report->right_stick_y > 0xC0) {
                pad->look_buttons |= 0x100;
            }
            if (!(report->held & 0x300)) {
                pad->held |= pad->look_buttons;
                pad->pressed |= pad->look_buttons & ~pad->prev_look_buttons;
            }
        } else {
            pad->look_buttons = 0;
            pad->prev_look_buttons = 0;
        }
        if (pad->type == 1 && ((PadReportAxes *)report)->left_stick != 0) {
            pad->prev_move_buttons = pad->move_buttons;
            pad->move_buttons = 0;
            if (report->left_stick_x < 0x40) {
                pad->move_buttons = 0x80;
            }
            if (report->left_stick_x > 0xC0) {
                pad->move_buttons |= 0x20;
            }
            if (report->left_stick_y < 0x40) {
                pad->move_buttons |= 0x10;
            }
            if (report->left_stick_y > 0xC0) {
                pad->move_buttons |= 0x40;
            }
            if (!(report->held & 0xF0)) {
                pad->held |= pad->move_buttons;
                pad->pressed |= pad->move_buttons & ~pad->prev_move_buttons;
            }
        } else {
            pad->move_buttons = 0;
            pad->prev_move_buttons = 0;
        }
    } else {
        pad->held = 0;
        pad->pressed = 0;
        pad->look_buttons = 0;
        pad->prev_look_buttons = 0;
        pad->move_buttons = 0;
        pad->prev_move_buttons = 0;
    }
}
