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
  int pa0;
  int pa4;
  int pa8;
  int pac;
  int pb0;
  int pb4;
  int pb8;
} McState;
extern McState D_01E4F278;

int func_0021AD68(u32 arg0, u32 arg1, int arg2, int arg3, int arg4)
{
  McState *s = &D_01E4F278;

  if (s->p88 != 0)
    return 1;
  if (arg0 >= 3U || arg1 >= 5U)
    return -1;
  s->p98 = 0x3C;
  s->p88 = 5;
  s->p80 = arg0;
  s->p84 = arg1;
  s->pa0 = arg2;
  s->pb8 = arg3;
  s->pa8 = arg4;
  s->pad1[0] = 0;
  s->state = 0;
  return 0;
}
