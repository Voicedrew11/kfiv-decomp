#include "kf_mc.h"

int sceMcGetInfo(int, int, int *, int *, int *);
int sceMcSync(int, int *, int *);

/* One step of checking a card: asks for its information three times (steps 0, 2, 4), waiting for each
   answer (steps 1, 3, 5). Returns the next step, the last answer once step 5 has it, or an error. */
int func_00218188(int port, int slot, int *type, int *free_clusters, int *format, int step) {
    int result;

    switch (step) {
    case 0:
    case 2:
    case 4:
        if (sceMcGetInfo(port, slot, type, free_clusters, format) != 0) {
            if (step == 4) {
                return -100;
            }
            step += 2;
        } else {
            step++;
        }
        break;
    case 1:
    case 3:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result != 0) {
                return result;
            }
            step++;
        }
        break;
    case 5:
        if (sceMcSync(1, NULL, &result) != 0) {
            MC_CARD_INFO->result[port][slot] = result;
            return result;
        }
        break;
    }
    return step;
}
