#include "kf_state.h"

extern ModeState D_005CF410[];

int func_001F9C38(void);
int func_001FA028(void);
int func_001FA568(void);
int func_001FA7C0(void);
int func_001FA9B8(void);
int func_001FAFC0(void);

/* Runs the handler for the current state; a result of 1 resets the state. */
int func_001FAEC0(void) {
    int result;

    switch (D_005CF410->state) {
    case 4:
        result = func_001FA028();
        break;
    case 5:
        result = func_001FA568();
        break;
    case 2:
        result = func_001FA7C0();
        break;
    case 3:
        result = func_001F9C38();
        break;
    case 6:
        result = func_001FA9B8();
        break;
    case 0x63:
        result = func_001FAFC0();
        break;
    case 0:
    default:
        result = 1;
        break;
    }
    if (result == 1) {
        D_005CF410->state = 0;
        D_005CF410->step = 0;
        D_005CF410->retries = 0;
    }
    return result;
}
