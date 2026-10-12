#include "kf_state.h"

void func_001D3608(int);
extern int D_00446CFC;
extern int (*const D_00440630[])(HandlerContext *);
extern HandlerContext D_005CF350;

/* Calls the handler selected by D_00446CFC with the context at D_005CF350, storing value in it first. */
int func_001F6EC0(int value) {
    int (*handler)(HandlerContext *);

    func_001D3608(0);
    handler = D_00440630[D_00446CFC];
    D_005CF350.arg = value;
    if (handler != NULL) {
        return handler(&D_005CF350);
    }
    return 1;
}
