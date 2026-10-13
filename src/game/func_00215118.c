#include "kf_script.h"
#include "kf_message.h"

/* Script command: show a message. */
void func_00215118(ScriptMessageCommand *command) {
    KF_Message_ShowNotice(0, 0, command->font.value, command->kind.value, command->frames.value, command->message_id.value);
    command->active = 0;
}
