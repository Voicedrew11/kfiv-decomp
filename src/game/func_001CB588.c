#include "common.h"

typedef struct {
    char pad0[0xC0];
    float x;
    float y;
    float z;
    char padCC[0xD4 - 0xCC];
    float r;
} Body;

typedef struct {
    u32 flags;
    char pad4[0x10 - 0x4];
    Body *body;
    char pad14[0x24 - 0x14];
    u8 f24;
    char pad25[0x2C - 0x25];
} Obj;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    char r;
    u8 f7;
    u16 f8;
} Rec;

extern char D_0038CD20[];
extern Obj D_005B6100[][16];
extern Rec D_00362928[][16];

void *memset(void *, int, u32);

void func_001CB588(Obj *arg0) {
    u32 idx;
    char *entry;
    Rec *r;
    Body *b;

    idx = (arg0->flags >> 2) & 1;
    entry = D_0038CD20 + idx * 0x60;
    r = &D_00362928[(u8)entry[1]]
                   [((char *)arg0 - (char *)D_005B6100[idx]) / sizeof(Obj)];
    memset(r, 0, sizeof(Rec));
    b = arg0->body;
    r->x = b->x;
    r->y = b->y;
    r->z = b->z;
    r->r = b->r * 16.0f;
    r->f8 = *(u16 *)((char *)arg0 + 2);
    r->f7 = arg0->f24;
}
