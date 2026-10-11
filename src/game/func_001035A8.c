#include "common.h"

extern u8 D_00354928[];
void func_001035A8(u32 x, int y)
{
  u32 t;
  u32 i;
  u32 s;
  u32 c;
  int new_var;
  if (((x < 0x80) && (y >= 0)) && (y < 0x80))
  {
    c = (i = D_00354928[(y / 2) + (x << 6)]);
    if (y & 1)
    {
      c = c >> 4;
      c++;
      t = c << 4;
      D_00354928[(y / 2) + (x << 6)] = (s = i & 0xF);
      c = t & 0xFF;
    }
    else
    {
      c &= 0xF;
      c++;
      D_00354928[(y / 2) + (x << 6)] = i & 0xF0;
    }
    new_var = c | D_00354928[(y / 2) + (x << 6)];
    D_00354928[(y / 2) + (x << 6)] = new_var;
  }
}
