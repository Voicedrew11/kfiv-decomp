#include "kf_mc.h"

/* Start a request unless one is active; returns 1 if busy, -1 on a bad argument, 0 on success. */
int func_0021ADD0(u32 unk80, u32 unk84, int unkA0, int unkB4, int unkA8)
{
  McState *state = &D_01E4F278;

  if (state->state != 0)
    return 1;
  if (unk80 >= 3U || unk84 >= 5U)
    return -1;
  state->timeout = 0x46;
  state->state = 6;
  state->unk80 = unk80;
  state->unk84 = unk84;
  state->unkA0 = unkA0;
  state->unkB4 = unkB4;
  state->unkA8 = unkA8;
  state->unk90 = state->unk8C = 0;
  return 0;
}
