#include "kf_entity.h"

void *memset(void *, int, u32);

extern LinkedPoint *D_004470C8[2];
extern LinkedPoint *D_004470D0;

void func_0019A848(void) {
    LinkedPoint *n;
    int i;

    memset(D_003E7500, 0, sizeof(D_003E7500));
    n = D_003E7500;
    n->handle = 0;
    n->prev = NULL;
    n->next = n + 1;
    n++;
    for (i = 1; i < LINKED_POINT_COUNT - 1; i++) {
        n->handle = i;
        n->prev = n - 1;
        n->next = n + 1;
        n++;
    }
    n->handle = LINKED_POINT_COUNT - 1;
    n->prev = n - 1;
    n->next = NULL;
    for (i = 1; i >= 0; i--) {
        D_004470C8[i] = NULL;
    }
    D_004470D0 = D_003E7500;
}
