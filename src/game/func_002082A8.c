#include "common.h"

extern s32 D_00447358;
extern void (*D_0042C008[7])(void);

/* Run the handler for the splash image's current state. */
void func_002082A8(void) {
    D_0042C008[D_00447358]();
}
