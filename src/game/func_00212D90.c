#include "common.h"

int func_0011E1F0(int, int);
void func_00211CF8(char *, int);
extern int D_00446770;

typedef struct
{
  int a00;
  int a04;
  int a08;
  int a0c;
} Obj;

typedef struct
{
  char pad[0x82];
  s16 s82;
} Sub;

typedef struct
{
  char pad0[0xFC];
  Sub *pFC;
  u16 f100;
} Ctl;

typedef struct
{
  Ctl *p00;
  char pad[0xC];
  int w10;
} Entry;

void func_00212D90(Obj *arg0)
{
  Entry *temp_v0 = (Entry *)func_0011E1F0(D_00446770, arg0->a08);

  if (temp_v0 != 0 && (temp_v0->p00->f100 & 1) && !(temp_v0->w10 & 0x400000))
  {
    func_00211CF8((char *)&arg0->a0c, temp_v0->p00->pFC->s82 < 1);
  }
  else
  {
    func_00211CF8((char *)&arg0->a0c, -1);
  }
  arg0->a00 = 0;
}
