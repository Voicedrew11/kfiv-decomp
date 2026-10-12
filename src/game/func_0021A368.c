#include "kf_mc.h"
#include <libmc.h>

int func_00218188(int, int, int *, int *, int *, int);
int func_00218270(int);
void func_00218340(int);
void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
extern u32 D_01E4F274[];
extern u8 D_01E4F338[0x400];

/* Steps the "read a file" request: 0 while it runs, 1 when the data is read, negative on an error. */
int func_0021A368(void) {
    int result;
    int port = D_01E4F278.unk80;
    int slot = D_01E4F278.unk84;
    int message;

    switch (D_01E4F278.unk8C) {
    case 0:
        D_01E4F334.file_fd = 0;
        D_01E4F274[0] = 0;
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
    case 1:
        if ((D_01E4F278.unk90 = func_00218188(port, slot, &MC_CARD_INFO->type[port][slot],
                                              &MC_CARD_INFO->free_clusters[port][slot],
                                              &MC_CARD_INFO->format[port][slot], D_01E4F278.unk90)) > 0) {
            break;
        }
        message = func_00218270(D_01E4F278.unk90);
        D_01E4F278.timeout = message;
        if (message != 0) {
            D_01E4F278.state = 0;
            return -10;
        }
        if (MC_CARD_INFO->type[port][slot] != 2) {
            D_01E4F278.unk8C = 100;
            break;
        }
        D_01E4F278.unk8C++;
        break;
    case 2:
        if (sceMcOpen(port, slot, (char *)D_01E4F278.unkA0, 1) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.unk8C = 100;
                break;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 3:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    func_00218340(result);
                    if (D_01E4F278.timeout == 100) {
                        D_01E4F278.timeout = 0x47;
                    }
                    D_01E4F278.unk8C = 100;
                    D_01E4F278.state = 0;
                    D_01E4F278.unk90 = -1;
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F334.file_fd = result;
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 4:
        memset(D_01E4F338, 0, 0x400);
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 5:
        if (sceMcRead(D_01E4F334.file_fd, D_01E4F338, 0x400) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.unk8C = 100;
                break;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 6:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.unk90 = result;
                    D_01E4F278.unk8C = 100;
                    break;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F278.unk9C = 0;
                if ((u32)D_01E4F278.unkA8 < D_01E4F274[0] + result) { // match: looks inverted: the clipped copy goes back to read again, the other goes on to close
                    result = D_01E4F278.unkA8 - D_01E4F274[0];
                    memcpy((u8 *)D_01E4F278.unkB4 + D_01E4F274[0], D_01E4F338, result);
                    D_01E4F278.unk8C--;
                } else {
                    memcpy((u8 *)D_01E4F278.unkB4 + D_01E4F274[0], D_01E4F338, result);
                    D_01E4F274[0] += result;
                    D_01E4F278.unk8C++;
                }
            }
        }
        break;
    case 7:
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 8:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x47;
                return -1;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 9:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.state = 0; // match: cleared before the result is tested, as in func_002186E0 step 23
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x47;
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F278.timeout = 0x48;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334.file_fd < 0) {
            D_01E4F278.timeout = 0x47;
            D_01E4F278.state = 0;
            return D_01E4F278.unk90;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 101:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x47;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.timeout = 0x47;
            D_01E4F278.state = 0;
            return -100;
        }
        break;
    }
    return 0;
}
