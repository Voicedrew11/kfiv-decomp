#include "kf_pad.h"
#include <libpad.h>

int func_00216DD0(PadSlot *, int, int);
void func_00216EA0(void *);

static PadSystem pad_system; /* 0x1E0DB00 */

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
        pad->phase = 0;
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
            if (pad->state == 0) {
                pad->phase = 0;
            }
            pad->request_state = scePadGetReqState(port, slot);
            switch (pad->phase) {
                case 0:
                    pad->mode = PAD_MODE_NONE;
                    if (pad->state == 0) {
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
                        case 4:
                            pad->phase = 0x28;
                            break;
                        case 7:
                            pad->phase = 0x46;
                            break;
                        case 2:
                        case 3:
                        case 5:
                        case 6:
                        case 0x100:
                        case 0x300:
                            break;
                    }
                    continue;
                case 0x28:
                case 0x46:
                    if (pad->phase != 0x28 || scePadInfoMode(port, slot, 4, -1) != 0) {
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
                        pad->phase = 0x6E;
                    } else if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->phase = 0x64;
                    } else if (pad->phase == 0x28) {
                        pad->phase = 0xFD;
                    } else {
                        pad->phase = 0xFE;
                    }
                    continue;
                case 0x64:
                    if (scePadSetMainMode(port, slot, 0, pad->mode_lock) == 1) {
                        pad->phase = 0x65;
                    }
                    continue;
                case 0x65:
                    if (pad->request_state == 1) {
                        pad->phase = 0x64;
                    }
                    if (pad->request_state == 0) {
                        if (pad->state == 6 || pad->state == 2) {
                            pad->actuators_aligned = 0;
                            pad->phase = 0xFD;
                        }
                    }
                    continue;
                case 0x6E:
                    if (scePadSetMainMode(port, slot, 1, pad->mode_lock) == 1) {
                        pad->phase = 0x6F;
                    }
                    continue;
                case 0x6F:
                    if (pad->request_state == 1) {
                        pad->phase = 0x6E;
                    }
                    if (pad->request_state == 0) {
                        if (pad->state == 6 || pad->state == 2) {
                            pad->actuators_aligned = 0;
                            if (scePadInfoPressMode(port, slot) == 1) {
                                pad->features |= PAD_FEATURE_PRESSURE;
                            }
                            pad->phase = 0xFE;
                        }
                    }
                    continue;
                case 0x78:
                    if (scePadEnterPressMode(port, slot) == 1) {
                        pad->phase = 0x79;
                    }
                    continue;
                case 0x79:
                    if (pad->request_state == 1) {
                        pad->phase = 0x78;
                    }
                    if (pad->request_state == 0) {
                        if (pad->state == 6 || pad->state == 2) {
                            pad->phase = 0xFF;
                        }
                    }
                    continue;
                case 0x82:
                    if (scePadExitPressMode(port, slot) == 1) {
                        pad->phase = 0x83;
                    }
                    continue;
                case 0x83:
                    if (pad->request_state == 1) {
                        pad->phase = 0x82;
                    }
                    if (pad->request_state == 0) {
                        if (pad->state == 6 || pad->state == 2) {
                            pad->phase = 0xFE;
                        }
                    }
                    continue;
                case 0xC8:
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
                case 0xC9:
                    if (pad->request_state == 1) {
                        pad->phase = 0xC8;
                    }
                    if (pad->request_state == 0) {
                        pad->actuators_aligned = 1;
                        pad->phase = pad->next_phase;
                    }
                    continue;
                case 0xFF:
                    if (pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request < PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = 0x82;
                        continue;
                    }
                    pad->mode = PAD_MODE_PRESSURE;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                case 0xFE:
                    if (pad->mode_request == PAD_MODE_ANALOG) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = 0;
                        continue;
                    }
                    if (pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = 0x78;
                        continue;
                    }
                    pad->mode = PAD_MODE_ANALOG;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                case 0xFD:
                    if (pad->mode_request == PAD_MODE_DIGITAL) {
                        pad->mode_request = PAD_MODE_NONE;
                    }
                    if (pad->mode_request == PAD_MODE_ANALOG || pad->mode_request == PAD_MODE_PRESSURE) {
                        pad->mode = PAD_MODE_NONE;
                        pad->phase = 0;
                        continue;
                    }
                    pad->mode = PAD_MODE_DIGITAL;
                    if (!pad->actuators_aligned && (pad->features & PAD_FEATURE_ACTUATORS)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                default:
                    continue;
            }
            old_buttons = pad->report.held;
            read_result = func_00216DD0(pad, port, slot);
            if (read_result == 0) {
                continue;
            }
            if (read_result == -1) {
                pad->phase = 0;
            }
            pad->report.pressed = pad->report.held & ~old_buttons;
            func_00216EA0(pad);
            if ((pad->features & PAD_FEATURE_ACTUATORS) && pad->state == 6) {
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
