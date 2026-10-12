#include "kf_mc.h"

int sceMcGetInfo(int, int, int *, int *, int *);
int sceMcSync(int, int *, int *);
int func_00218270(int);

/* Steps the "check the card" request: 0 while it runs, 1 when a usable card is there, -10 otherwise. */
int func_002183C0(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->unk8C;
    int port = mc_state->unk80;
    int slot = mc_state->unk84;
    int message;

    switch (step) {
    case 0:
        mc_state->unk8C = 1;
        mc_state->unk9C = 0;
        break;
    case 1:
        if (sceMcGetInfo(port, slot, &MC_CARD_INFO->type[port][slot], &MC_CARD_INFO->free_clusters[port][slot],
                         &MC_CARD_INFO->format[port][slot]) != 0) {
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
            MC_CARD_INFO->result[port][slot] = result;
            if (result < 0) {
                if (mc_state->unk9C == step) {
                    mc_state->timeout = func_00218270(result);
                    return -10;
                }
            } else {
                mc_state->state = 0;
                message = func_00218270(result);
                mc_state->timeout = message;
                if (message == 0) {
                    return 1;
                }
                return -10;
            }
            mc_state->unk9C++;
            mc_state->unk8C--;
        }
        break;
    default:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.timeout = 0x64;
            D_01E4F278.state = 0;
            return -10;
        }
        break;
    }
    return 0;
}
