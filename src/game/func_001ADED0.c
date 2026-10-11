#include "common.h"

int func_001AEC48();
int func_001AED58(void *);
int func_001AF0B0(void *);

typedef struct {
    char pad0[0x4C];
    u16 count;
    char pad1[0x2];
    char *sub;
} Actor;

typedef struct {
    char pad0[0xD0];
    float d0;
    char pad1[0xC];
    float e0;
    float e4;
} Sub;

void func_001ADED0(Actor *arg0) {
    Sub *s;

    func_001AEC48();
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    s = (Sub *)arg0->sub;
    arg0->count = arg0->count + 1;
    s->e0 = s->e0 * s->d0 * 10.0f;
    s->e4 = s->e4 * s->d0 * 10.0f;
}
