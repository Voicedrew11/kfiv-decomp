#include "common.h"

int sceSifCheckStatRpc(int *);
int printf(const char *, ...);
extern char D_0043F640[];
extern int D_005BF890[];

/* Polls the SIF RPC status until it is no longer 1, giving up after 0x989680 tries. */
int func_001CBA58(void) {
    int retries = 0x989680;

    while (sceSifCheckStatRpc(D_005BF890) == 1) {
        if (retries < 0) {
            printf(D_0043F640);
            return -1;
        }
        retries--;
    }
    return 1;
}
