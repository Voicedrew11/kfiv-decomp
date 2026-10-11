#include "common.h"

extern u8 D_00349D55[];
extern char D_0038CD20[];

typedef struct {
    float x, y, z, w;
    s64 v;
    char pad[0x8];
} Pos;

typedef struct {
    Pos a;
    Pos b;
    char pad40[0x40];
    Pos c;
} Cam;

void func_00119D90(int arg0)
{
    char *e = D_0038CD20 + arg0 * 0x60;
    Cam *t;

    if (D_00349D55[0] == 1) {
        t = *(Cam **)(e + 0xC);
        t->a.x = t->a.y = 0.0f;
        t->a.z = -9.705887f;
        t->a.w = 476470.62f;
        t->a.v = 0;
        t->b.x = t->b.y = 0.0f;
        t->b.z = 2.0588226f;
        t->b.w = 455294.1f;
        t->b.v = 0;
        t->c.x = t->c.y = 0.0f;
        t->c.z = -9.705887f;
        t->c.w = 476470.62f;
        t->c.v = 0;
    }
}
