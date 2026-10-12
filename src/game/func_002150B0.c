#include "kf_state.h"

int func_001CF980(void);
void func_001CF988(int, int, int, int, int, int, ...);
void func_00103858(int);

/* Starts the one-time setup for the task, then clears active once func_001CF980 reports done. */
void func_002150B0(SetupTask *task) {
    if (task->started == 0) {
        func_00103858(task->unk08);
        func_001CF988(0, 0, task->unk10, 0, task->unk18, task->unk08);
        task->started = 1;
    }
    if (func_001CF980() == 0) {
        task->active = 0;
    }
}
