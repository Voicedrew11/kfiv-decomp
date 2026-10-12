#include "kf_mc.h"

/* The free clusters of the card less two, or -1 when no card was seen. */
int func_0021B3C0(int port, int slot) {
    int free_clusters;

    if (MC_CARD_INFO->free_clusters[port][slot] == 0 && MC_CARD_INFO->type[port][slot] == 0) {
        D_01E4F278.timeout = 1;
        return -1;
    }
    free_clusters = MC_CARD_INFO->free_clusters[port][slot] - 2;
    if (free_clusters <= 0) {
        D_01E4F278.timeout = 0x1E;
        free_clusters = 0;
    }
    D_01E4F278.timeout = 0; // match: overwrites the 0x1E just stored
    return free_clusters;
}
