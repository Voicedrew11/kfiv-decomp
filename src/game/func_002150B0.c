#include "kf_state.h"
#include "kf_message.h"

void Game_MarkMessageHeard(int);

/* Starts the one-time setup for the task, then clears active once its message box has closed. */
void func_002150B0(SetupTask *task) {
    if (task->started == 0) {
        Game_MarkMessageHeard(task->unk08);
        Message_OpenBox(0, 0, task->unk10, 0, task->unk18, task->unk08);
        task->started = 1;
    }
    if (Message_IsBoxOpen() == 0) {
        task->active = 0;
    }
}
