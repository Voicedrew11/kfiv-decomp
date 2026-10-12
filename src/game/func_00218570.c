#include "kf_mc.h"

struct sceMcTblGetDir;
int sceMcGetDir(int, int, char *, int, int, struct sceMcTblGetDir *);
int sceMcSync(int, int *, int *);

/* Steps the "read directory" request: 0 while it runs, 1 when done, negative on an error. */
int func_00218570(void) {
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
        if (sceMcGetDir(port, slot, (char *)mc_state->unkA0, mc_state->unkA8, mc_state->unkAC,
                        (struct sceMcTblGetDir *)mc_state->unkB0) != 0) {
            if (mc_state->unk9C == 2) {
                mc_state->timeout = 0x64;
                mc_state->state = 0;
                return -1;
            }
            mc_state->unk9C++;
        } else {
            mc_state->unk8C++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result >= 0) {
                mc_state->unk94 = result;
                mc_state->timeout = 0;
                mc_state->state = 0;
                return 1;
            }
            if (mc_state->unk9C == step) {
                if (result == -2) {
                    mc_state->timeout = 3;
                } else if (result == -4) {
                    mc_state->timeout = 0x64;
                } else {
                    mc_state->timeout = 0x64;
                }
                return result;
            }
            mc_state->unk9C++;
            mc_state->unk8C--;
        }
        break;
    default:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.timeout = 0x64;
            D_01E4F278.state = 0;
            return -1;
        }
        break;
    }
    return 0;
}
