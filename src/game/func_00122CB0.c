#include "common.h"

int func_0010EA08(int, void *, int *, int *, float);
void *memset(void *, int, u32);

/* Advances the float at +0x50 toward 100000, then calls func_0010EA08; on a zero result sets state 3 and clears the float. */
void func_00122CB0(char *state_block) {
    int buf_a[4];
    int buf_b[4];
    float *timer; // timer[4] is the float at +0x50

    timer = (float *)(state_block + 0x40);
    memset(buf_a, 0, 0x10);
    buf_a[3] = 0x80;
    memset(buf_b, 0, 0x10);
    buf_b[3] = 8;
    if (timer[4] < 100000.0f) {
        timer[4] += 6.6666f;
    }
    if (func_0010EA08(*(int *)state_block, timer, buf_b, buf_a, timer[4]) == 0) {
        state_block[0x11] = 3;
        timer[4] = 0.0f;
    }
}
