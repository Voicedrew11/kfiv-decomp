#include "common.h"

void func_00190A30(char *);
void func_00195898(char *, int);
int func_0019A310(char *, void (*)(void), int);
void *func_001267B0(int);
extern void func_001816A0(void);
typedef struct 
{
  char pad0[0x5C];
  int f5C;
  char pad1[0xD8 - 0x60];
  u64 flags;
} Actor;
void func_00167358(char *arg0)
{
  Actor *a = (Actor *) arg0;
  char *s = arg0 + 0xE0;
  u64 f;
  char *t;
  func_00190A30(arg0);
  f = a->flags;
  if (((f >> 36) & 3) == 2)
  {
    a->f5C = 0;
    a->flags &= ~0x200;
    func_0019A310(arg0, func_001816A0, 0);
    if (s[0x7C])
    {
      t = func_001267B0(s[0x7B]);
      if (t != 0)
      {
        t[0x15A] = -1;
      }
    }
    s[0x7C] = 0;
  }
  func_00195898(arg0, 2);
}
