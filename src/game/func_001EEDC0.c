#include "kf_state.h"

void func_001D3608(int);
extern int D_00446CC8;
extern int (*const D_004404C8[])(HandlerContext *);
extern HandlerContext D_005CF340;

/* Calls the handler selected by D_00446CC8 with the context at D_005CF340, storing value in it first. */
int func_001EEDC0(int value) {
    int (*handler)(HandlerContext *);

    func_001D3608(0);
    handler = D_004404C8[D_00446CC8];
    D_005CF340.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF340);
    }
    return 1;
}
