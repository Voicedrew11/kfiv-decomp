#include "common.h"

int sceSifCheckStatRpc(int *);
extern int D_005BF890[];

/* The state of the sound driver's RPC client: 1 while a call is still running. */
int func_001CBAC0(void) {
    return sceSifCheckStatRpc(D_005BF890);
}
