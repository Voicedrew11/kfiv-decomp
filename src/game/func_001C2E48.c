#include "common.h"

int func_001C2D08(void *);
void func_0020E918(f32 *, f32 *, f32 *, f32 *, f32 *, f32);

typedef struct { int count; char *list; int pad; u32 n; } Hdr;

void func_001C2E48(void *arg0, int arg1, int arg2, float arg3) {
    Hdr *h;
    int i, j;
    char *p;
    char *q;

    if (*(int *)((char *)arg0 + 0x10) & 0x10000) {
        h = *(Hdr **)(*(char **)arg0 + 0x110);
        if (h != 0 && h->n >= 4U) {
            func_001C2D08(*(void **)arg0);
            p = h->list;
            i = h->count;
            do {
                q = p + 0x20;
                j = *(u16 *)p - 2;
                do {
                    func_0020E918(*(int *)q, *(int *)(q + 0x10), *(int *)(q + 0x20), arg1, arg2, arg3);
                    if (*(int *)0x70000010 != 0) {
                        return;
                    }
                    q += 0x10;
                } while (--j != 0);
                p = q + 0x20;
            } while (--i != 0);
        }
    }
}
