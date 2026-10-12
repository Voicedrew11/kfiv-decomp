#include "kf_script.h"

void func_001BB360(void);

/* Run func_001BB360 (sets PlayerScreenTint.unk00 to 10). */
void func_00213910(ScriptObjectCommand *command) {
    func_001BB360();
    command->active = 0;
}
