#include "common.h"

int WaitSema(int);
int SignalSema(int);
int func_0021D9F0(void *, void *);

typedef struct {
    s64 a;
    s64 b;
    int c;
    int d;
} Entry;

typedef struct {
    char pad0[0x40];
    int sema;
    char pad1[0xC];
    Entry *buf;
    int cap;
    int count;
    int head;
} Queue;

int func_0021DBB8(Queue *arg0, Entry *arg1) {
    int var_s2 = 0;
    Entry *slot;

    WaitSema(arg0->sema);
    if (arg0->count < arg0->cap) {
        func_0021D9F0(arg0, arg1);
        var_s2 = 1;
        slot = (Entry *) (arg0->head * 0x18 + (int) arg0->buf);
        slot->c = arg1->c;
        slot->a = arg1->a;
        arg0->buf[arg0->head].d = arg1->d;
        slot->b = arg1->b;
        arg0->count = arg0->count + 1;
        arg0->head = (arg0->head + 1) % arg0->cap;
    }
    SignalSema(arg0->sema);
    return var_s2;
}
