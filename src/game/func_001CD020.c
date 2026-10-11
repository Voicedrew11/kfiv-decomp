#include "common.h"

extern int D_004469E8;
extern int D_004471C0;
extern int D_00446704;
extern int (*const D_0043F670[])(int *, int *);
extern int D_00420940[];

/* Runs the handler for the current step with a packed argument block, then advances the step. */
int func_001CD020(int word0, int word1, int word2, int word3, int word4, int word5, int word6) {
    int args[7];
    int advance;

    if (D_004469E8 == 0) {
        D_004471C0 = word3;
    }
    args[0] = word0;
    args[1] = word1;
    args[2] = word2;
    args[3] = D_004471C0;
    args[4] = word4;
    args[5] = word5;
    args[6] = word6;
    advance = D_0043F670[D_004469E8](args, D_00420940);
    D_004471C0 = args[3];
    D_004469E8 += advance;
    if (D_004469E8 == 6) {
        D_004469E8 = 0;
    }
    if (D_004469E8 < 0) {
        D_004469E8 = 0;
        D_00446704++;
    }
    if (D_004469E8 == 0) {
        return D_00420940[5];
    }
    return -3;
}
