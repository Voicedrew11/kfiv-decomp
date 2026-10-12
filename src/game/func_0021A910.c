#include "kf_mc.h"

int sceMcChdir(int, int, char *, char *);
int sceMcSync(int, int *, int *);

/* Steps the "change directory" request: 0 while it runs, 1 when done, negative on an error. */
int func_0021A910(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->unk8C;
    int port = mc_state->unk80;
    int slot = mc_state->unk84;

    switch (step) {
    case 0:
        mc_state->unk8C = 1;
        mc_state->unk9C = 0;
        break;
    case 1:
        if (sceMcChdir(port, slot, (char *)mc_state->unkA0, (char *)mc_state->unkA4) != 0) {
            if (mc_state->unk9C == 2) {
                mc_state->timeout = 0x64;
                return -10;
            }
            mc_state->unk9C++;
        } else {
            mc_state->unk8C++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result == 0) {
                mc_state->state = 0;
                mc_state->timeout = 0;
                return 1;
            }
            if (mc_state->unk9C == step) {
                mc_state->state = 0;
                if (result == -2) {
                    mc_state->timeout = 3;
                } else {
                    mc_state->timeout = 0x64;
                }
                return result;
            }
            mc_state->unk9C++;
            mc_state->unk8C--;
        }
        break;
    }
    return 0;
}
