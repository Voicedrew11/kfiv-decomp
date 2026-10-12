#include "kf_entity.h"

extern LinkedPoint *D_004470C8[2];

LinkedPoint *func_0019B2B0(LinkedPoint *);
void func_0019B290(LinkedPoint *);

/* Releases every active point of the given kind from both active lists. */
void func_0019AAA0(int kind) {
    int i;
    LinkedPoint *node;
    LinkedPoint *next;

    for (i = 0; i < 2; i++) {
        node = D_004470C8[i];
        while (node != NULL) {
            if (node->active != 0 && node->kind_b == kind) {
                next = func_0019B2B0(node);
                func_0019B290(node);
                node = next;
            } else {
                node = node->next;
            }
        }
    }
}
