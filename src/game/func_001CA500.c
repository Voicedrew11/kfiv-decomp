#include "common.h"
#include "kf_game.h"

extern ListNode *D_004469D4;

/* Pushes a node onto the front of the list at D_004469D4. */
void func_001CA500(ListNode *node) {
    ListNode *head = D_004469D4;

    node->prev = NULL;
    node->next = head;
    if (head != NULL) {
        head->prev = node;
    }
    D_004469D4 = node;
}
