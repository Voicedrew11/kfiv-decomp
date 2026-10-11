#include "common.h"

int func_001CBAE0();
extern char *D_004471D4;

int func_001CDBC0(u32 arg0, s16 arg1)
{
  char *temp_v0;

  if (arg0 >= 0x11U)
  {
    return -1;
  }
  temp_v0 = D_004471D4 + (arg0 << 5);
  if (*(int *)temp_v0 == 0)
  {
    return -1;
  }
  func_001CBAE0(1, 0x1060, *(s16 *)(temp_v0 + 0x18), arg1);
  return 0;
}
