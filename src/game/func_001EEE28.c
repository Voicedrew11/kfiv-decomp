#include "kf_state.h"

extern void (*D_00446CC4)(HandlerContext *);

/* Runs the handler in D_00446CC4 with the context this step was given. */
int func_001EEE28(HandlerContext *context) {
    D_00446CC4(context);
    return 1;
}
