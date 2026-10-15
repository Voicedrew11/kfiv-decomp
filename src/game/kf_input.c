#include "kf_pad.h"
#include "kf_game.h"

void *memset(void *, int, u32);
void func_001B5590(int);
void func_001B57A0(u8, u8, u8);
void func_001B5460(void);

extern PadState pad_state;
extern PadState pad_states[2];
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern s16 D_00446938;
extern s16 D_0044693A;
extern u16 D_0044693C;
extern s16 D_0044693E;
extern u16 D_00446940;
extern s16 D_00446942;
extern s16 D_00446944;
extern s16 D_00446946;
extern s8 sfx_volume[];
extern u8 D_00446949;
extern u8 D_0044694A;
extern u8 D_0044694B;
extern u8 D_0044694C;
extern u8 D_0044694D;
extern u8 D_0044694E;

void func_001B5390(void) {
    Pad_Init(0);
    Pad_OpenPort(0, 0);
    Pad_Update();
    memset(&pad_state, 0, 0x10);
    memset(pad_states, 0, 0x20);
}

/* Store a fixed set of 14 bit-mask constants into consecutive globals. */
void func_001B53E8(void) {
    D_0044692C = 0x10;
    D_0044692E = 0x40;
    D_00446930 = 0x20;
    D_00446932 = 0x80;
    D_00446934 = 0x800;
    D_00446936 = 0x400;
    D_00446938 = 0x100;
    D_0044693A = 0x200;
    D_0044693C = 0x8000;
    D_0044693E = 0x1000;
    D_00446940 = 0x4000;
    D_00446942 = 0x2000;
    D_00446944 = 1;
    D_00446946 = 8;
}

/* Once a frame: reads the controller and runs the rumble that func_001B57A0 asked for. */
void func_001B5460(void) {
    int type = Pad_GetMode(0, 0);
    int scaled;

    if (type != 1 && type != 0xFF) {
        Pad_RequestAnalog(0, 0, 3);
    }
    Pad_Update();
    func_001B5590(0);
    pad_state = pad_states[0];
    if (sfx_volume[4] != 0) {
        if (D_00446949 != 0 && D_0044694E == 0) {
            if (D_0044694B != 0) {
                if (D_0044694D != 0) {
                    scaled = D_0044694A * D_0044694B / D_0044694C;
                    if (scaled == 0) {
                        scaled = 1;
                    }
                    Pad_SetRumble(0, 0, (u8)scaled);
                } else {
                    Pad_SetRumble(0, 0, D_0044694A);
                }
                D_0044694B--;
            } else {
                Pad_SetRumble(0, 0, 0);
                D_00446949 = 0;
            }
        }
    } else if (D_00446949 != 0) {
        Pad_SetRumble(0, 0, 0);
        D_00446949 = 0;
    }
}

/* Reads the controller in a port into its PadState; the sticks count as direction buttons unless
   the matching buttons are held. */
void func_001B5590(int port) {
    PadState *pad = &pad_states[port];
    PadReport *report;

    pad->type = Pad_GetMode(port, 0);
    if (pad->type != 0xFF) {
        report = Pad_GetReport(port, 0);
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

void func_001B5778(void) {
    func_001B57A0(0, 0, 0);
    func_001B5460();
}

/* Requests a controller rumble (only when the option at sfx_volume[4] is on). */
void func_001B57A0(u8 strength, u8 duration, u8 delay) {
    if (sfx_volume[4] != 0) {
        D_0044694A = strength;
        D_00446949 = 1;
        D_0044694B = D_0044694C = duration;
        D_0044694D = delay;
    }
}

void func_001B57D8(void) {
    if (D_0044694E == 0) {
        D_0044694E = 1;
        Pad_SetRumble(0, 0, 0);
    }
}

void func_001B5800(void) {
    D_0044694E = 0;
}

void func_001B5808(void) {
}
