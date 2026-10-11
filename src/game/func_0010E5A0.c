#include "common.h"

void func_00216778(void *, void *);
void func_00216BC0(void *, void *, void *);

typedef struct {
    float v[3];
    u8 bone;
    char pad[3];
} Src;

typedef struct {
    float m[4];
} Dst;

void func_0010E5A0(char *arg0) {
    float vec[4];
    int i;
    Src *s;
    Dst *d;

    if (*(Src **)(arg0 + 0x120) != 0) {
        s = *(Src **)(arg0 + 0x120);
        d = *(Dst **)(arg0 + 0x124);
        for (i = 0; i < 32; i++, s++, d++) {
            if (s->bone != 0xFF) {
                func_00216778(vec, s);
                vec[3] = 1.0f;
                func_00216BC0(d, *(char **)(arg0 + 0xF8) + (s->bone << 7), vec);
                func_00216BC0(d, arg0 + 0x40, d);
            } else {
                func_00216778(d, arg0 + 0xC0);
            }
        }
    }
}
