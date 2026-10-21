#include "input/pad.h"
#include <libpad.h>

/* scePadGetState: the controller's state (PadSlot.state) */
#define PAD_STATE_DISCONNECTED      0
#define PAD_STATE_FINDCTP1          2
#define PAD_STATE_STABLE            6

/* scePadGetReqState: how the last request ended (PadSlot.request_state) */
#define PAD_REQSTATE_COMPLETE       0
#define PAD_REQSTATE_FAILED         1

/* scePadInfoMode(1): controller type */
#define PAD_TYPE_NEGCON             2
#define PAD_TYPE_KONAMI_GUN         3
#define PAD_TYPE_DIGITAL            4
#define PAD_TYPE_ANALOG_JOYSTICK    5
#define PAD_TYPE_NAMCO_GUN          6
#define PAD_TYPE_DUALSHOCK          7

/* PadSlot.phase: step of the set-up state machine in Pad_Update */
#define PAD_PHASE_IDENTIFY              0x00
#define PAD_PHASE_QUERY_DIGITAL         0x28
#define PAD_PHASE_QUERY_DUALSHOCK       0x46
#define PAD_PHASE_SET_DIGITAL           0x64
#define PAD_PHASE_WAIT_SET_DIGITAL      0x65
#define PAD_PHASE_SET_ANALOG            0x6E
#define PAD_PHASE_WAIT_SET_ANALOG       0x6F
#define PAD_PHASE_ENTER_PRESSURE        0x78
#define PAD_PHASE_WAIT_ENTER_PRESSURE   0x79
#define PAD_PHASE_EXIT_PRESSURE         0x82
#define PAD_PHASE_WAIT_EXIT_PRESSURE    0x83
#define PAD_PHASE_ALIGN_ACTUATORS       0xC8
#define PAD_PHASE_WAIT_ALIGN_ACTUATORS  0xC9
#define PAD_PHASE_SETTLED_DIGITAL       0xFD
#define PAD_PHASE_SETTLED_ANALOG        0xFE
#define PAD_PHASE_SETTLED_PRESSURE      0xFF

static PadSystem pad_system; /* 0x1E0DB00 */

/* Reads the controller into pad->report and turns its button bits round (a set bit is then a pressed button).
   Returns 0 when scePadRead had nothing, -1 when the controller type changed, 1 otherwise. */
static int Pad_Read(PadSlot *pad, int port, int slot) {
    PadReport report;
    int old_type;

    old_type = pad->pad_type;
    if (scePadRead(port, slot, &report) == 0) {
        return 0;
    }
    if (report.status == 0) {
        pad->pad_type = report.type_and_length >> 4;
        if (old_type != 0 && pad->pad_type != old_type) {
            return -1;
        }
        pad->report = report;
        pad->report.held = ~pad->report.held;
    }
    return 1;
}

/* Steps the auto-repeat of the buttons in repeat_mask: report.repeated gets those that went down this frame
   and those whose repeat came round (after delay frames, then every interval frames). */
static void Pad_StepRepeat(PadSlot *pad) {
    int i;
    u16 bit;
    PadRepeatKey *key;

    pad->report.repeated = 0;
    for (i = 0; i < 16; i++) {
        key = &pad->repeat_keys[i];
        bit = 1 << i;
        if (pad->repeat_mask & bit) {
            if (!(pad->report.held & bit)) {
                key->fired = 0;
            } else if (pad->report.pressed & bit) {
                pad->report.repeated |= bit;
                key->fired = 1;
                key->countdown = key->delay;
            } else if (--key->countdown == 0) {
                pad->report.repeated |= bit;
                key->fired = 2;
                key->countdown = key->interval;
            }
        }
    }
}

/* Forgets every controller and starts libpad. */
int Pad_Init(int mode) {
    PadSlot *pad;

    pad_system.open_slots[0] = 0;
    pad_system.open_slots[1] = 0;
    pad = pad_system.slots;
    do {
        pad->report.pressed = 0;
        pad->report.repeated = 0;
        pad->repeat_mask = 0;
        pad->phase = PAD_PHASE_IDENTIFY;
        pad->pad_type = 0;
        pad->mode = PAD_MODE_NONE;
        pad->mode_request = PAD_MODE_NONE;
        pad->mode_lock = 0;
        pad++;
    } while ((int)pad < (int)&pad_system.slots[8]); // match: retail compares signed
    return scePadInit(mode);
}

/* Opens a controller slot and marks it for Pad_Update to poll. */
int Pad_OpenPort(int port, int slot) {
    pad_system.open_slots[port] |= 1 << slot;
    return scePadPortOpen(port, slot, pad_system.slots[port * 4 + slot].dma_buffer);
}

/* Once a frame, for every open controller slot: steps the set-up state machine (find out what the controller
   can do, switch it to the mode that was asked for, line up the rumble motors) and, once that has settled, reads
   the buttons and sends the rumble.

   PadSlot.phase: 0 nothing known yet; 0x28 a digital pad and 0x46 a DualShock, features being asked;
   0x64/0x65 switching to digital, 0x6E/0x6F to analog, 0x78/0x79 turning pressure mode on, 0x82/0x83 off
   (the odd phase waits for the request to finish, and goes back to the even one when it failed);
   0xC8/0xC9 lining up the motors; 0xFD, 0xFE, 0xFF settled in digital, analog, analog with pressure.
   Only the settled phases leave the switch and read the pad. The other controller kinds (2, 3, 5, 6, 0x100,
   0x300) are named in the first phase and left alone. */
void Pad_Update(void) {
    u8 align[6];
    int port;
    int slot;
    PadSlot *pad;
    int type;
    int extended_type;
    int i;
    int old_buttons;
    int read_result;

    for (port = 0; port < 2; port++) {
        for (slot = 0; slot < 4; slot++) {
            if (!((pad_system.open_slots[port] >> slot) & 1)) {
                continue;
            }
            pad = &pad_system.slots[port * 4 + slot];
            pad->state = scePadGetState(port, slot);
            if (pad->state == PAD_STATE_DISCONNECTED) {
                pad->phase = PAD_PHASE_IDENTIFY;
            }
            pad->request_state = scePadGetReqState(port, slot);
            switch (pad->phase) {
                case PAD_PHASE_IDENTIFY:
                    pad->mode = PAD_MODE_NONE;
                    if (pad->state == PAD_STATE_DISCONNECTED) {
                        continue;
                    }
                    type = scePadInfoMode(port, slot, 1, 0);
                    if (type == 0) {
                        continue;
                    }
                    extended_type = scePadInfoMode(port, slot, 2, 0);
                    pad->features = 0;
                    if (extended_type > 0) {
                        type = extended_type;
                    }
                    pad->actuators_aligned = 0;
                    switch (type) {
                        case PAD_TYPE_DIGITAL:
                            pad->phase = PAD_PHASE_QUERY_DIGITAL;
                            break;
                        case PAD_TYPE_DUALSHOCK:
                            pad->phase = PAD_PHASE_QUERY_DUALSHOCK;
                            break;
                        case PAD_TYPE_NEGCON:
                        case PAD_TYPE_KONAMI_GUN:
                        case PAD_TYPE_ANALOG_JOYSTICK:
                        case PAD_TYPE_NAMCO_GUN:
                        case 0x100:
                        case 0x300:
                            break;
                    }
                    continue;
                case PAD_PHASE_QUERY_DIGITAL:
                case PAD_PHASE_QUERY_DUALSHOCK:
                    if (pad->phase != PAD_PHASE_QUERY_DIGITAL || scePadInfoMode(port, slot, 4, -1) != 0) {
                        pad->features |= PAD_FEATURE_MODE_SWITCH;
                    }
                    pad->actuator_count = scePadInfoAct(port, slot, -1, 0);
                    if (pad->actuator_count != 0) {
                        pad->features |= PAD_FEATURE_ACTUATORS;
                    }
                    if (scePadInfoPressMode(port, slot) == 1) {
                        pad->features |= PAD_FEATURE_PRESSURE;
                    }
                    if ((pad->features & PAD_FEATURE_MODE_SWITCH) &&
                        (pad->mode_request == PAD_MODE_ANALOG || pad->mode_request == PAD_MODE_PRESSURE)) {
                        pad->phase = PAD_PHASE_SET_ANALOG;
                    } else if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->phase = PAD_PHASE_SET_DIGITAL;
                    } else if (pad->phase == PAD_PHASE_QUERY_DIGITAL) {
                        pad->phase = PAD_PHASE_SETTLED_DIGITAL;
                    } else {
                        pad->phase = PAD_PHASE_SETTLED_ANALOG;
                    }
                    continue;
                case PAD_PHASE_SET_DIGITAL:
                    if (scePadSetMainMode(port, slot, 0, pad->mode_lock) == 1) {
                        pad->phase = PAD_PHASE_WAIT_SET_DIGITAL;
                    }
                    continue;
                case PAD_PHASE_WAIT_SET_DIGITAL:
                    if (pad->request_state == PAD_REQSTATE_FAILED) {
                        pad->phase = PAD_PHASE_SET_DIGITAL;
                    }
                    if (pad->request_state == PAD_REQSTATE_COMPLETE) {
                        if (pad->state == PAD_STATE_STABLE || pad->state == PAD_STATE_FINDCTP1) {
                            pad->actuators_aligned = 0;
                            pad->phase = PAD_PHASE_SETTLED_DIGITAL;
                        }
                    }
                    continue;
                case PAD_PHASE_SET_ANALOG:
                    if (scePadSetMainMode(port, slot, 1, pad->mode_lock) == 1) {
                        pad->phase = PAD_PHASE_WAIT_SET_ANALOG;
                    }
                    continue;
                case PAD_PHASE_WAIT_SET_ANALOG:
                    if (pad->request_state == PAD_REQSTATE_FAILED) {
                        pad->phase = PAD_PHASE_SET_ANALOG;
                    }
                    if (pad->request_state == PAD_REQSTATE_COMPLETE) {
                        if (pad->state == PAD_STATE_STABLE || pad->state == PAD_STATE_FINDCTP1) {
                            pad->actuators_aligned = 0;
                            if (scePadInfoPressMode(port, slot) == 1) {
                                pad->features |= PAD_FEATURE_PRESSURE;
                            }
                            pad->phase = PAD_PHASE_SETTLED_ANALOG;
                        }
                    }
                    continue;
                case PAD_PHASE_ENTER_PRESSURE:
                    if (scePadEnterPressMode(port, slot) == 1) {
                        pad->phase = PAD_PHASE_WAIT_ENTER_PRESSURE;
                    }
                    continue;
                case PAD_PHASE_WAIT_ENTER_PRESSURE:
                    if (pad->request_state == PAD_REQSTATE_FAILED) {
                        pad->phase = PAD_PHASE_ENTER_PRESSURE;
                    }
                    if (pad->request_state == PAD_REQSTATE_COMPLETE) {
                        if (pad->state == PAD_STATE_STABLE || pad->state == PAD_STATE_FINDCTP1) {
                            pad->phase = PAD_PHASE_SETTLED_PRESSURE;
                        }
                    }
                    continue;
                case PAD_PHASE_EXIT_PRESSURE:
                    if (scePadExitPressMode(port, slot) == 1) {
                        pad->phase = PAD_PHASE_WAIT_EXIT_PRESSURE;
                    }
                    continue;
                case PAD_PHASE_WAIT_EXIT_PRESSURE:
                    if (pad->request_state == PAD_REQSTATE_FAILED) {
                        pad->phase = PAD_PHASE_EXIT_PRESSURE;
                    }
                    if (pad->request_state == PAD_REQSTATE_COMPLETE) {
                        if (pad->state == PAD_STATE_STABLE || pad->state == PAD_STATE_FINDCTP1) {
                            pad->phase = PAD_PHASE_SETTLED_ANALOG;
                        }
                    }
                    continue;
                case PAD_PHASE_ALIGN_ACTUATORS:
                    for (i = 0; i < 6; i++) {
                        align[i] = 0xFF;
                        pad->actuators[i] = 0;
                    }
                    for (i = 0; i < pad->actuator_count; i++) {
                        align[i] = i;
                    }
                    if (scePadSetActAlign(port, slot, align) == 1) {
                        pad->phase++;
                    }
                    continue;
                case PAD_PHASE_WAIT_ALIGN_ACTUATORS:
                    if (pad->request_state == PAD_REQSTATE_FAILED) {
                        pad->phase = PAD_PHASE_ALIGN_ACTUATORS;
                    }
                    if (pad->request_state == PAD_REQSTATE_COMPLETE) {
                        pad->actuators_aligned = 1;
                        pad->phase = pad->next_phase;
                    }
                    continue;
                case PAD_PHASE_SETTLED_PRESSURE:
                    if (pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request < PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = PAD_PHASE_EXIT_PRESSURE;
                        continue;
                    }
                    pad->mode = PAD_MODE_PRESSURE;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = PAD_PHASE_ALIGN_ACTUATORS;
                        continue;
                    }
                    break;
                case PAD_PHASE_SETTLED_ANALOG:
                    if (pad->mode_request == PAD_MODE_ANALOG) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = PAD_PHASE_IDENTIFY;
                        continue;
                    }
                    if (pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = PAD_PHASE_ENTER_PRESSURE;
                        continue;
                    }
                    pad->mode = PAD_MODE_ANALOG;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = PAD_PHASE_ALIGN_ACTUATORS;
                        continue;
                    }
                    break;
                case PAD_PHASE_SETTLED_DIGITAL:
                    if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request == PAD_MODE_ANALOG || pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = PAD_PHASE_IDENTIFY;
                        continue;
                    }
                    pad->mode = PAD_MODE_DIGITAL;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = PAD_PHASE_ALIGN_ACTUATORS;
                        continue;
                    }
                    break;
                default:
                    continue;
            }
            old_buttons = pad->report.held;
            read_result = Pad_Read(pad, port, slot);
            if (read_result == 0) {
                continue;
            }
            if (read_result == -1) {
                pad->phase = PAD_PHASE_IDENTIFY;
            }
            pad->report.pressed = pad->report.held & ~old_buttons;
            Pad_StepRepeat(pad);
            if ((pad->features & PAD_FEATURE_ACTUATORS) && pad->state == PAD_STATE_STABLE) {
                scePadSetActDirect(port, slot, pad->actuators);
            }
        }
    }
}

/* The controller's scePadGetState value as of the last Pad_Update. */
int Pad_GetState(int port, int slot) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];

    return pad->state;
}

/* The controller's scePadGetReqState value as of the last Pad_Update. */
int Pad_GetRequestState(int port, int slot) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];

    return pad->request_state;
}

/* The controller's last report: status, id, buttons, analog bytes, then pressed and repeated buttons. */
PadReport *Pad_GetReport(int port, int slot) {
    return &pad_system.slots[port * 4 + slot].report;
}

/* The buttons held on a controller. */
int Pad_GetButtons(int port, int slot) {
    return pad_system.slots[port * 4 + slot].report.held;
}

/* The buttons that went down this frame. */
int Pad_GetPressed(int port, int slot) {
    return pad_system.slots[port * 4 + slot].report.pressed;
}

/* The buttons that fired this frame, auto-repeat included. */
int Pad_GetRepeated(int port, int slot) {
    return pad_system.slots[port * 4 + slot].report.repeated;
}

/* Turns auto-repeat on for the given buttons: the first repeat after delay frames, then every interval. */
void Pad_SetRepeat(int port, int slot, u16 buttons, u16 delay, u16 interval) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];
    int i;

    pad->repeat_mask |= buttons;
    for (i = 0; i < 16; i++) {
        if ((buttons >> i) & 1) {
            pad->repeat_keys[i].delay = delay;
            pad->repeat_keys[i].interval = interval;
            pad->repeat_keys[i].countdown = 0;
            pad->repeat_keys[i].fired = 0;
        }
    }
}

/* Turns auto-repeat off for the given buttons. */
void Pad_ClearRepeat(int port, int slot, u16 buttons) {
    pad_system.slots[port * 4 + slot].repeat_mask &= ~buttons;
}

/* Sets the six actuator bytes Pad_Update sends to the controller. */
void Pad_SetActuators(int port, int slot, u8 *values) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];
    int i;

    for (i = 0; i < 6; i++) {
        pad->actuators[i] = values[i];
    }
}

/* Sets the rumble: the big motor runs at the given power and the small motor is on whenever that is not zero. */
void Pad_SetRumble(int port, int slot, int power) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];
    u8 big_motor = power;

    if (big_motor != 0) {
        pad->actuators[0] = 1;
    } else {
        pad->actuators[0] = 0;
    }
    pad->actuators[1] = big_motor;
}

/* Asks for digital mode. */
void Pad_RequestDigital(int port, int slot, int lock) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];

    pad->mode_lock = lock;
    pad->mode_request = PAD_MODE_DIGITAL;
}

/* Asks for analog mode; FALSE when the controller cannot switch modes. */
int Pad_RequestAnalog(int port, int slot, int lock) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];

    if (!(pad->features & PAD_FEATURE_MODE_SWITCH)) {
        return FALSE;
    }
    pad->mode_lock = lock;
    pad->mode_request = PAD_MODE_ANALOG;
    return TRUE;
}

/* Asks for pressure-sensitive mode; FALSE when the buttons do not sense pressure. */
int Pad_RequestPressure(int port, int slot) {
    PadSlot *pad = &pad_system.slots[port * 4 + slot];

    if (!(pad->features & PAD_FEATURE_PRESSURE)) {
        return FALSE;
    }
    pad->mode_request = PAD_MODE_PRESSURE;
    return TRUE;
}

/* The controller's mode: PAD_MODE_*, PAD_MODE_NONE while not known yet. */
int Pad_GetMode(int port, int slot) {
    return pad_system.slots[port * 4 + slot].mode;
}
