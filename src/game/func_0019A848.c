#include "common.h"

void *memset(void *, int, u32);

typedef struct Node {
    char pad0[0x2C];
    s16 id;
    char pad1[0x0A];
    struct Node *prev;
    struct Node *next;
} Node;

extern Node D_003E7500[];
extern int D_004470C8[2];
extern Node *D_004470D0;

void func_0019A848(void) {
    Node *n;
    int i;

    memset(D_003E7500, 0, 0x8000);
    n = D_003E7500;
    n->id = 0;
    n->prev = 0;
    n->next = n + 1;
    n++;
    for (i = 1; i < 0x1FF; i++) {
        n->id = i;
        n->prev = n - 1;
        n->next = n + 1;
        n++;
    }
    n->id = 0x1FF;
    n->prev = n - 1;
    n->next = 0;
    for (i = 1; i >= 0; i--) {
        D_004470C8[i] = 0;
    }
    D_004470D0 = D_003E7500;
}
