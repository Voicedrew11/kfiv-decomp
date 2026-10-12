#include "kf_effect.h"

int func_00211D90(void *, double);  // match: retail converts the float argument to double at the call site, as the definition takes float
extern f32 D_0042C6F0[];

/* Converts the angle to a table index and passes its cosine (sine table + 0x400) on. */
void func_00214DC0(AngleTask *task) {
    char *command = task->command;
    int n = (int)(task->angle * 4096.0f / 6.2831855f);
    int i = n + 0x1000;
    int idx = i % 4096;

    func_00211D90(command, D_0042C6F0[idx + 0x400]);
    task->active = 0;
}
