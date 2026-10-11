#include "common.h"

typedef struct { char pad[0xD5]; char idx; char pad2[0x180 - 0xD6]; } Obj;
typedef struct { char a; u8 b; char pad[0x5E]; } Ent;
extern Ent D_0038CD20[];
extern Obj D_003BEDF0[][0x80];
Obj *func_001267B0(int);
void func_00211CF8(char *, int);
int func_00103548(int, int);

void func_00212388(char *arg0)
{
    Obj *p = func_001267B0(*(int *)(arg0 + 8));

    if (p != 0) {
        int idx = p->idx;
        Ent *e = &D_0038CD20[idx];
        int r = func_00103548(e->b, ((char *)p - (char *)D_003BEDF0[idx]) / sizeof(Obj));
        func_00211CF8(arg0 + 0xC, r);
    } else {
        func_00211CF8(arg0 + 0xC, -1);
    }
    *(int *)arg0 = 0;
}
