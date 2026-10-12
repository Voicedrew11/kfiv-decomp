#include "kf_script.h"

void func_001CFA70(int, int, int, int, int, int, ...);

/* Script command: show a message. */
void func_00215118(ScriptMessageCommand *command) {
    func_001CFA70(0, 0, command->font.value, command->kind.value, command->frames.value, command->message_id.value);
    command->active = 0;
}
