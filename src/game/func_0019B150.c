#include "common.h"
#include "kf_entity.h"

void func_00216C48(void *, void *, void *);
extern LinkedPoint *D_004470C8[2];

void func_0019B150(void *transform) {
    int i;
    LinkedPoint *node;

    i = 0;
    do {
        node = D_004470C8[i];
        if (node != NULL) {
            do {
                if (node->follow != NULL) {
                    func_00216C48(node, node, transform);
                }
                node = node->next;
            } while (node != NULL);
        }
        i++;
    } while (i < 2);
}
