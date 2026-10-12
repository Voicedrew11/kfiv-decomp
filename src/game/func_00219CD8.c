#include "kf_mc.h"
#include <libmc.h>

int func_00218188(int, int, int *, int *, int *, int);
int func_00218270(int);
void func_00218340(int);
char *strcpy(char *, const char *);
int sceOpen(const char *, int);
int sceRead(int, void *, int);
int sceLseek(int, int, int);
int sceClose(int);
extern sceMcTblGetDir D_01E4F780;
extern u8 D_01E4F338[0x400];
extern int D_01E4F264[]; /* the file being copied from */
extern int D_01E4F268[]; /* bytes read from it this step */
extern int D_01E4F26C[]; /* its size */
extern int D_01E4F270[]; /* bytes copied so far */

/* Steps the "copy a file to the card" request: 0 while it runs, 1 when the file is written, negative on
   an error. The file is read 0x400 bytes at a time. */
int func_00219CD8(void) {
    char path[0x40];
    int result;
    int port = D_01E4F278.unk80;
    int slot = D_01E4F278.unk84;
    int message;
    int available;
    int reserve;
    int flags;
    int source;

    switch (D_01E4F278.unk8C) {
    case 0:
        D_01E4F334.file_fd = 0;
        D_01E4F26C[0] = 0;
        D_01E4F270[0] = 0;
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
        break;
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
        D_01E4F278.unk8C++;
        break;
    case 2:
        if (MC_CARD_INFO->type[port][slot] != 2) {
            D_01E4F278.state = 0;
            D_01E4F278.timeout = 0x3D;
            return -4;
        }
        D_01E4F278.unk8C++;
        break;
    case 3:
        if (sceMcGetDir(port, slot, (char *)D_01E4F278.unkA0, 0, 1, &D_01E4F780) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 100;
                return -1;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 4:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    if (result == -2) {
                        D_01E4F278.timeout = 3;
                    } else if (result == -4) {
                        D_01E4F780.FileSizeByte = 0;
                        D_01E4F278.unk8C++;
                        break;
                    } else {
                        D_01E4F278.timeout = 100;
                    }
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                if (result == 0) {
                    D_01E4F780.FileSizeByte = 0;
                }
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 5:
        available = ((D_01E4F780.FileSizeByte + 0x3FF) >> 10) + MC_CARD_INFO->free_clusters[port][slot];
        if (D_01E4F780.FileSizeByte == 0 && available < 3) {
            D_01E4F278.state = 0;
            D_01E4F278.timeout = 0x1F;
            return -6;
        }
        strcpy(path, (char *)D_01E4F278.unkA4);
        source = sceOpen(path, 1);
        D_01E4F264[0] = source;
        if (source < 0) {
            D_01E4F278.state = 0;
            D_01E4F278.timeout = 0x3D;
            return source;
        }
        if (D_01E4F278.unkAC != 0) {
            D_01E4F26C[0] = D_01E4F278.unkAC;
        } else {
            D_01E4F26C[0] = sceLseek(source, 0, 2);
            sceLseek(D_01E4F264[0], 0, 0);
        }
        reserve = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserve = 0;
        }
        if (available - reserve < (D_01E4F26C[0] + 0x3FF) / 0x400) {
            D_01E4F278.timeout = 0x3D;
            D_01E4F278.state = 0;
            return -101;
        }
        D_01E4F278.unk8C = 8;
        D_01E4F278.unk9C = 0;
        break;
    case 8:
        flags = 3;
        if (D_01E4F780.FileSizeByte == 0) {
            flags = 0x203;
        }
        if (sceMcOpen(port, slot, (char *)D_01E4F278.unkA0, flags) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x3D;
                return -11;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 9:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    func_00218340(result);
                    if (D_01E4F278.timeout == 100) {
                        D_01E4F278.timeout = 0x3D;
                    }
                    D_01E4F278.unk8C = 100;
                    D_01E4F278.unk90 = -1;
                    break;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F334.file_fd = result;
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 10:
        if ((D_01E4F268[0] = sceRead(D_01E4F264[0], D_01E4F338, 0x400)) <= 0) {
            D_01E4F278.unk8C = 13;
        } else {
            D_01E4F278.unk9C = 0;
            D_01E4F278.unk8C++;
        }
        break;
    case 11:
        if (sceMcWrite(D_01E4F334.file_fd, D_01E4F338, D_01E4F268[0]) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.unk90 = -1;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 12:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2 || result == -8) {
                    D_01E4F278.unk90 = -1;
                    D_01E4F278.unk8C = 100;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F270[0] += D_01E4F268[0];
                D_01E4F278.unk9C = 0;
                if (D_01E4F270[0] < D_01E4F26C[0]) {
                    D_01E4F278.unk8C -= 2;
                } else {
                    D_01E4F278.unk8C++;
                }
            }
        }
        break;
    case 13:
        sceClose(D_01E4F264[0]);
        D_01E4F278.unk8C++;
        break;
    case 14:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.unk90 = -1;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 15:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.state = 0;
                    D_01E4F278.timeout = 0x3D;
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F278.timeout = 0x3E;
                D_01E4F278.state = 0;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F264[0] != 0) {
            sceClose(D_01E4F264[0]);
        }
        if (D_01E4F334.file_fd < 0) {
            D_01E4F278.timeout = 0x3D;
            D_01E4F278.state = 0;
            return -999;
        }
        D_01E4F278.unk9C = 0; // match: the step is not advanced here, so the request stays in step 100
        break;
    case 101:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x3D;
                return -999;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.timeout = 0x3D;
            D_01E4F278.state = 0;
            return -999;
        }
        break;
    }
    return 0;
}
