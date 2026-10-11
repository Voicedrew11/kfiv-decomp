#include "common.h"

int func_0011E1F0(int, int);
int func_00211CF8(char *, int);
extern int D_00446770;

typedef struct
{
  int a00;
  int a04;
  int a08;
} Obj;

typedef struct
{
  char pad[0x44];
  s16 s44;
  s16 s46;
  s16 s48;
  s16 s4A;
} Sub;

typedef struct
{
  int a00;
  u8 *p04;
  Sub *p08;
} Entry;

void func_00212F70(Obj *arg0)
{
  Entry *temp_v0;

  temp_v0 = (Entry *)func_0011E1F0(D_00446770, arg0->a08);
  if (temp_v0 != 0 && temp_v0->a00 != 0 && *temp_v0->p04 == 9)
  {
    func_00211CF8((char *)arg0 + 0xC, temp_v0->p08->s44);
    func_00211CF8((char *)arg0 + 0x14, temp_v0->p08->s46);
    func_00211CF8((char *)arg0 + 0x1C, temp_v0->p08->s48);
    func_00211CF8((char *)arg0 + 0x24, temp_v0->p08->s4A);
  }
  arg0->a00 = 0;
}
