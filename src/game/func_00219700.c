#include "kf_mc.h"
#include <libmc.h>

int func_00218188(int, int, int *, int *, int *, int);
int func_00218270(int);
void func_00218340(int);
extern sceMcTblGetDir D_01E4F780;
extern u32 D_01E4F25C[]; /* bytes written so far */
extern int D_01E4F260[]; /* bytes to hand to sceMcWrite this step */

/* Steps the "write a file as it is" request: 0 while it runs, 1 when the file is written, negative on
   an error. The data goes out 0x400 bytes at a time. */
int func_00219700(void) {
    int result;
    int port = D_01E4F278.unk80;
    int slot = D_01E4F278.unk84;
    int message;
    int available;
    int reserve;
    int flags;

    switch (D_01E4F278.unk8C) {
    case 0:
        D_01E4F334.file_fd = 0;
        D_01E4F25C[0] = 0;
        D_01E4F278.unk8C++;
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
            if (message == 100) {
                D_01E4F278.timeout = 0x3D;
            }
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
        D_01E4F278.unk9C = 0;
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
        reserve = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserve = 0;
        }
        if ((u32)(available - reserve) < (u32)(D_01E4F278.unkA8 + 0x3FF) >> 10) {
            D_01E4F278.timeout = 0x3D;
            D_01E4F278.state = 0;
            return -101;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C = 8;
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
        D_01E4F260[0] = D_01E4F278.unkA8 - D_01E4F25C[0];
        if (D_01E4F260[0] > 0x400) {
            D_01E4F260[0] = 0x400;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 11:
        if (sceMcWrite(D_01E4F334.file_fd, (u8 *)D_01E4F278.unkB8 + D_01E4F25C[0], D_01E4F260[0]) != 0) {
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
                D_01E4F25C[0] += result;
                D_01E4F278.unk9C = 0;
                if (D_01E4F25C[0] >= (u32)D_01E4F278.unkA8) {
                    D_01E4F278.unk8C++;
                } else {
                    D_01E4F278.unk8C = 10;
                }
            }
        }
        break;
    case 13:
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
    case 14:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.state = 0;
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x3D;
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F278.timeout = 0x3E;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334.file_fd < 0) {
            D_01E4F278.timeout = 0x3D;
            D_01E4F278.state = 0;
            return -999;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
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
