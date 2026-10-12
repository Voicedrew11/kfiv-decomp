#include "kf_game.h"

/* Index of the largest of the seven values (the first one of equals). Nothing calls this. */
int func_0020D008(Unk20D008Owner *owner) {
    s16 *values = owner->unk70;
    int largest = values[0];
    int largest_index = 0;
    int i;

    for (i = 1; i < 7; i++) {
        if (largest < values[i]) {
            largest = values[i];
            largest_index = i;
        }
    }
    return largest_index;
}
