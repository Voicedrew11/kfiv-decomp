#include "kf_mc.h"

/* Start a request unless one is active; returns 1 if busy, -1 on a bad argument, 0 on success. */
int func_0021AE38(u32 kind, u32 slot, int value_a, int value_b, int value_c)
{
  McState *state = &D_01E4F278;

  if (state->state != 0)
    return 1;
  if (kind >= 3U || slot >= 5U)
    return -1;
  state->timeout = 0x3C;
  state->state = 7;
  state->unk80 = kind;
  state->unk84 = slot;
  state->unkA0 = value_a;
  state->unkB8 = value_b;
  state->unkA8 = value_c;
  state->unk8C = 0;
  state->unk90 = 0;
  return 0;
}
