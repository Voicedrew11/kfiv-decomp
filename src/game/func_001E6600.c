#include "kf_state.h"

void func_001D3608(int);
extern int D_00446CA4;
extern int (*const D_004402E8[])(HandlerContext *);
extern HandlerContext D_005CF330;

/* Calls the handler selected by D_00446CA4 with the context at D_005CF330, storing value in it first. */
int func_001E6600(int value) {
    int (*handler)(HandlerContext *);

    func_001D3608(0);
    handler = D_004402E8[D_00446CA4];
    D_005CF330.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF330);
    }
    return 1;
}
