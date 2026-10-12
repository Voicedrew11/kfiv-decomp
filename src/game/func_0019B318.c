#include "kf_entity.h"

extern LinkedPoint *D_004470C8[2];

/* Pushes a point onto the front of one of the two active lists (list is 0 or 1). */
void func_0019B318(LinkedPoint *point, u32 list) {
    LinkedPoint *head;

    if (list < 2) {
        head = D_004470C8[list];
        point->active = TRUE;
        point->list_index = list;
        point->next = head;
        point->prev = NULL;
        if (D_004470C8[list] != NULL) {
            D_004470C8[list]->prev = point;
        }
        D_004470C8[list] = point;
    }
}
