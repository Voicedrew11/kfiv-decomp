#include "input/pad.h"
#include "core/game.h"

void *memset(void *, int, u32);
void Input_ReadPort(int);
void Input_StartRumble(u8, u8, u8);
void Input_Update(void);

extern PadState pad_state;
extern PadState pad_states[2];
extern u16 forward_button;
extern u16 back_button;
extern u16 turn_right_button;
extern u16 turn_left_button;
extern u16 step_right_button;
extern u16 step_left_button;
extern u16 look_down_button;
extern u16 look_up_button;
extern u16 attack_button;
extern u16 magic_button;
extern u16 action_button;
extern u16 menu_button;
extern u16 shortcut_button;
extern u16 pause_button;
extern s8 sfx_volume[];
extern u8 rumble_active;
extern u8 rumble_strength;
extern u8 rumble_frames_left;
extern u8 rumble_frames;
extern u8 rumble_fades;
extern u8 rumble_paused;

/* Opens controller port 0 and clears both pad states. */
void Input_Init(void) {
    Pad_Init(0);
    Pad_OpenPort(0, 0);
    Pad_Update();
    memset(&pad_state, 0, 0x10);
    memset(pad_states, 0, 0x20);
}

/* Gives every control its default button (the pad masks forward_button to pause_button). */
void Input_SetDefaultControls(void) {
    forward_button = PAD_UP;
    back_button = PAD_DOWN;
    turn_right_button = PAD_RIGHT;
    turn_left_button = PAD_LEFT;
    step_right_button = PAD_R1;
    step_left_button = PAD_L1;
    look_down_button = PAD_L2;
    look_up_button = PAD_R2;
    attack_button = PAD_SQUARE;
    magic_button = PAD_TRIANGLE;
    action_button = PAD_CROSS;
    menu_button = PAD_CIRCLE;
    shortcut_button = PAD_SELECT;
    pause_button = PAD_START;
}

/* Once a frame: reads the controller and runs the rumble that Input_StartRumble asked for. */
void Input_Update(void) {
    int type = Pad_GetMode(0, 0);
    int scaled;

    if (type != 1 && type != 0xFF) {
        Pad_RequestAnalog(0, 0, 3);
    }
    Pad_Update();
    Input_ReadPort(0);
    pad_state = pad_states[0];
    if (sfx_volume[4] != 0) {
        if (rumble_active != 0 && rumble_paused == 0) {
            if (rumble_frames_left != 0) {
                if (rumble_fades != 0) {
                    scaled = rumble_strength * rumble_frames_left / rumble_frames;
                    if (scaled == 0) {
                        scaled = 1;
                    }
                    Pad_SetRumble(0, 0, (u8)scaled);
                } else {
                    Pad_SetRumble(0, 0, rumble_strength);
                }
                rumble_frames_left--;
            } else {
                Pad_SetRumble(0, 0, 0);
                rumble_active = 0;
            }
        }
    } else if (rumble_active != 0) {
        Pad_SetRumble(0, 0, 0);
        rumble_active = 0;
    }
}

/* Reads the controller in a port into its PadState; the sticks count as direction buttons unless
   the matching buttons are held. */
void Input_ReadPort(int port) {
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
                pad->look_buttons = PAD_R2;
            }
            if (report->right_stick_y > 0xC0) {
                pad->look_buttons |= PAD_L2;
            }
            if (!(report->held & (PAD_L2 | PAD_R2))) {
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
                pad->move_buttons = PAD_LEFT;
            }
            if (report->left_stick_x > 0xC0) {
                pad->move_buttons |= PAD_RIGHT;
            }
            if (report->left_stick_y < 0x40) {
                pad->move_buttons |= PAD_UP;
            }
            if (report->left_stick_y > 0xC0) {
                pad->move_buttons |= PAD_DOWN;
            }
            if (!(report->held & PAD_DIRECTIONS)) {
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

/* Stops the rumble at once: an empty request, then a controller update. */
void Input_StopRumble(void) {
    Input_StartRumble(0, 0, 0);
    Input_Update();
}

/* Requests a controller rumble of strength for duration frames (only when the option at sfx_volume[4] is on);
   a nonzero last argument makes it fade out over the frames. */
void Input_StartRumble(u8 strength, u8 duration, u8 delay) {
    if (sfx_volume[4] != 0) {
        rumble_strength = strength;
        rumble_active = 1;
        rumble_frames_left = rumble_frames = duration;
        rumble_fades = delay;
    }
}

/* Holds the rumble (the game is paused): the motor stops and the request waits. */
void Input_PauseRumble(void) {
    if (rumble_paused == 0) {
        rumble_paused = 1;
        Pad_SetRumble(0, 0, 0);
    }
}

/* Lets a held rumble go on. */
void Input_ResumeRumble(void) {
    rumble_paused = 0;
}

/* Empty. */
void func_001B5808(void) {
}
