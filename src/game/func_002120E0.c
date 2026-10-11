#include "kf_script.h"

int func_00208168();
int func_00208170(int);

void func_002120E0(ScriptWaitCommand *command)
{
  if (command->started == 0)
  {
    if (func_00208170(command->object_id.value) != 0)
    {
      command->started = 1;
    }
  }
  else if (func_00208168() == 0)
  {
    command->active = 0;
  }
}
