#include "common.h"

void *memset(void *, int, u32);
void func_001D7C90(void);
extern char D_00425510[];
extern char D_00425910[];
extern void *D_00446B20;
extern float D_00446C04;
extern char D_00446C08;
extern char D_004472CE;
extern char D_004472DA;

void func_001D7C30(void)
{
  D_004472CE = 0;
  D_00446C04 = 128.0f;
  D_00446C08 = 0;
  D_004472DA = 0;
  memset(D_00425510, 0, 0x400);
  memset(D_00425910, 0, 0x400);
  D_00446B20 = (void *)func_001D7C90;
}
