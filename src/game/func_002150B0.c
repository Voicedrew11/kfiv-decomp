#include "kf_state.h"
#include "kf_message.h"

void func_00103858(int);

/* Starts the one-time setup for the task, then clears active once its message box has closed. */
void func_002150B0(SetupTask *task) {
    if (task->started == 0) {
        func_00103858(task->unk08);
        KF_Message_OpenBox(0, 0, task->unk10, 0, task->unk18, task->unk08);
        task->started = 1;
    }
    if (KF_Message_IsBoxOpen() == 0) {
        task->active = 0;
    }
}
