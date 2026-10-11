#include "common.h"

typedef struct
{
  int pad0[32];
  int p80;
  int p84;
  int p88;
  int state;
  int pad1[2];
  int p98;
  int p9c;
} McState;
extern McState D_01E4F278;

int func_0021B478(int arg0, int arg1, int arg2)
{
  McState *s = &D_01E4F278;
  int temp_v0 = *(int *)((char *)&D_01E4F278 + 0x20 + (arg0 * 0x10 + arg1 * 4)) - 2;

  if (temp_v0 <= 0)
  {
    s->p98 = 0x1E;
    return -1;
  }
  if ((u32)temp_v0 < (u32)arg2)
  {
    s->p98 = 0x1F;
    return -2;
  }
  return arg2;
}
