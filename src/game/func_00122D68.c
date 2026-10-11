#include "common.h"

int func_0010EA08(int, void *, int *, int *, float);
void *memset(void *, int, u32);

/* Advances the float at +0x50 toward 2000, then calls func_0010EA08; on a zero result sets state 1 and clears the float. */
void func_00122D68(char *actor) {
    int buf_a[4];
    int buf_b[4];
    float *timer;

    timer = (float *)(actor + 0x40);
    memset(buf_a, 0, 0x10);
    memset(buf_b, 0, 0x10);
    buf_b[3] = -8;
    if (timer[4] < 2000.0f) {
        timer[4] += 4.0f;
    }
    if (func_0010EA08(*(int *)actor, timer, buf_b, buf_a, timer[4]) == 0) {
        actor[0x11] = 1;
        timer[4] = 0.0f;
    }
}
