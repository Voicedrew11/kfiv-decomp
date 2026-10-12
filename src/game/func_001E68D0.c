#include "kf_state.h"

extern void (*D_00446CA0)(HandlerContext *);

/* Runs the handler in D_00446CA0 with the context this step was given. */
int func_001E68D0(HandlerContext *context) {
    D_00446CA0(context);
    return 1;
}
