#include "common.h"

extern s8 D_00446958;

int Sound_StopVoice(int); // match: an int return (the definition is void) keeps -1 in $3

/* Stops the sound slot D_00446958 holds, if any. */
void func_001BB590(void) {
    if (D_00446958 >= 0) {
        Sound_StopVoice(D_00446958);
        D_00446958 = -1;
    }
}
