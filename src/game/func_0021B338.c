#include "kf_mc.h"

int sceMcInit();

/* Init the memory card library, then clear the card state block if it succeeded. */
int func_0021B338(void) {
    int result;
    int count;
    int *words;

    result = sceMcInit();
    if (result == 0) {
        count = 3;
        D_01E4F278.unk8C = D_01E4F278.state = 0;
        words = &D_01E4F278.unk00[28];
        do {
            words[-4] = 0;
            count -= 1;
            words[0] = 0;
            words += 1;
        } while (count >= 0);
    }
    return result;
}
