#include "kf_game.h"

extern PadSystem D_01E0DB00;

/* The controller's last report: status, id, buttons, analog bytes, then pressed and repeated buttons. */
void * func_00217760(int port, int slot) {
    return &D_01E0DB00.slots[port * 4 + slot].report_status;
}
