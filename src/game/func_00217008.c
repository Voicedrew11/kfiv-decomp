#include "kf_game.h"
#include <libpad.h>

extern PadSystem D_01E0DB00;

int func_00216DD0(PadSlot *, int, int);
void func_00216EA0(void *);

/* Once a frame, for every open controller slot: steps the set-up state machine (find out what the controller
   can do, switch it to the mode that was asked for, line up the rumble motors) and, once that has settled, reads
   the buttons and sends the rumble.

   PadSlot.phase: 0 nothing known yet; 0x28 a digital pad and 0x46 a DualShock, features being asked;
   0x64/0x65 switching to digital, 0x6E/0x6F to analog, 0x78/0x79 turning pressure mode on, 0x82/0x83 off
   (the odd phase waits for the request to finish, and goes back to the even one when it failed);
   0xC8/0xC9 lining up the motors; 0xFD, 0xFE, 0xFF settled in digital, analog, analog with pressure.
   Only the settled phases leave the switch and read the pad. The other controller kinds (2, 3, 5, 6, 0x100,
   0x300) are named in the first phase and left alone. */
void func_00217008(void) {
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
            if (!((D_01E0DB00.open_slots[port] >> slot) & 1)) {
                continue;
            }
            pad = &D_01E0DB00.slots[port * 4 + slot];
            pad->state = scePadGetState(port, slot);
            if (pad->state == 0) {
                pad->phase = 0;
            }
            pad->request_state = scePadGetReqState(port, slot);
            switch (pad->phase) {
                case 0:
                    pad->mode = 0xFF;
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
                        pad->features |= 1;
                    }
                    pad->actuator_count = scePadInfoAct(port, slot, -1, 0);
                    if (pad->actuator_count != 0) {
                        pad->features |= 2;
                    }
                    if (scePadInfoPressMode(port, slot) == 1) {
                        pad->features |= 4;
                    }
                    if ((pad->features & 1) && (pad->mode_request == 1 || pad->mode_request == 2)) {
                        pad->phase = 0x6E;
                    } else if (pad->mode_request == 0) {
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
                                pad->features |= 4;
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
                    if (pad->mode_request == 2) {
                        pad->mode_request = 0xFF;
                    }
                    if (pad->mode_request < 2) {
                        pad->mode = 0xFF;
                        pad->phase = 0x82;
                        continue;
                    }
                    pad->mode = 2;
                    if (!pad->actuators_aligned && (pad->features & 2)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                case 0xFE:
                    if (pad->mode_request == 1) {
                        pad->mode_request = 0xFF;
                    }
                    if (pad->mode_request == 0) {
                        pad->mode = 0xFF;
                        pad->phase = 0;
                        continue;
                    }
                    if (pad->mode_request == 2) {
                        pad->mode = 0xFF;
                        pad->phase = 0x78;
                        continue;
                    }
                    pad->mode = 1;
                    if (!pad->actuators_aligned && (pad->features & 2)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                case 0xFD:
                    if (pad->mode_request == 0) {
                        pad->mode_request = 0xFF;
                    }
                    if (pad->mode_request == 1 || pad->mode_request == 2) {
                        pad->mode = 0xFF;
                        pad->phase = 0;
                        continue;
                    }
                    pad->mode = 0;
                    if (!pad->actuators_aligned && (pad->features & 2)) {
                        pad->next_phase = pad->phase;
                        pad->phase = 0xC8;
                        continue;
                    }
                    break;
                default:
                    continue;
            }
            old_buttons = pad->buttons;
            read_result = func_00216DD0(pad, port, slot);
            if (read_result == 0) {
                continue;
            }
            if (read_result == -1) {
                pad->phase = 0;
            }
            pad->pressed = pad->buttons & ~old_buttons;
            func_00216EA0(pad);
            if ((pad->features & 2) && pad->state == 6) {
                scePadSetActDirect(port, slot, pad->actuators);
            }
        }
    }
}
