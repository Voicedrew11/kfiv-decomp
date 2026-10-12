#include "common.h"

int func_0020A710(void);
int func_0020A8C8(void);
void func_00209D68(void);

/* Runs the screen set up by func_0020A710 until its per-frame step returns a result. */
int func_0020A6D8(void) {
    int result;

    func_0020A710();
    do {
        result = func_0020A8C8();
        func_00209D68();
    } while (result == 0);
    return result;
}
