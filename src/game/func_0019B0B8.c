#include "kf_entity.h"

void func_00216778(void *, void *);

extern LinkedPoint *D_004470C8[2];

void func_0019B0B8(void)
{
  int i;
  LinkedPoint *n;

  for (i = 0; i < 2; i++)
  {
    for (n = D_004470C8[i]; n != NULL; n = n->next)
    {
      if (n->follow != NULL)
      {
        float saved = n->position.w;
        func_00216778(n, n->follow);
        n->position.w = saved;
      }
    }
  }
}
