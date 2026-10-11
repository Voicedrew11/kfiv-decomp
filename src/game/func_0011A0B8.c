#include "common.h"

typedef struct {
    float x;
    float y;
    float z;
    float w;
    s64 v;
    char pad[8];
} Fog;

typedef struct {
    char pad[0x40];
    Fog a;
    Fog b;
} Obj0C;

typedef struct {
    char pad[0xF0];
    float r;
    float g;
    float b;
} Obj10;

typedef struct {
    char pad[0xC];
    Obj0C *p0C;
    Obj10 *p10;
    char pad2[0x4C];
} Entry;

extern u8 D_0034A0D5[];
extern Entry D_0038CD20[];

void func_0011A0B8(int arg0) {
    Entry *e = &D_0038CD20[arg0];

    if (D_0034A0D5[0] == 1) {
        Obj0C *p = e->p0C;
        Obj10 *q = e->p10;

        p->a.x = p->a.y = 0.0f;
        p->a.z = -127.5f;
        p->a.w = 382500.0f;
        p->a.v = 0;
        p->b.x = p->b.y = 0.0f;
        p->b.z = -127.5f;
        p->b.w = 382500.0f;
        p->b.v = 0;
        q->r = 0.29803923f;
        q->g = 0.29803923f;
        q->b = 0.30980393f;
    }
}
