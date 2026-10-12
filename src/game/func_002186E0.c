#include "kf_mc.h"
#include <libmc.h>

int func_00218188(int, int, int *, int *, int *, int);
int func_00218270(int);
void func_00218340(int);
void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
extern sceMcTblGetDir D_01E4F780;
extern u32 D_01E4F244 __attribute__((section(".bss")));
extern int D_01E4F248 __attribute__((section(".bss")));
extern u8 D_01E4F338[0x400];

/* Steps the "write a file" request: 0 while it runs, 1 when the file is written, negative on an error.
   The file is the data length, the data and a check byte per 0x400-byte block, an end mark, padding to
   0x20 bytes and a 0x20-byte trailer of zeroes that is written first. */
int func_002186E0(void) {
    int result;
    int step = D_01E4F278.unk8C;
    int slot = D_01E4F278.unk84;
    int port = D_01E4F278.unk80;
    int message;
    int available;
    int reserved;
    int open_mode;
    int quantity; // match: one variable for the data bytes left over and, later, the clusters needed
    int chunk;
    int header_size;
    int position;
    int index;
    u8 checksum;

    switch (step) {
    case 0:
        D_01E4F334.file_fd = 0;
        D_01E4F244 = 0;
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
        D_01E4F248 = 0;
        break;
    case 1:
        if ((D_01E4F278.unk90 = func_00218188(port, slot, &MC_CARD_INFO->type[port][slot],
                                             &MC_CARD_INFO->free_clusters[port][slot],
                                             &MC_CARD_INFO->format[port][slot], D_01E4F278.unk90)) > 0) {
            break;
        }
        if ((D_01E4F278.timeout = func_00218270(D_01E4F278.unk90)) != 0) {
            D_01E4F278.state = 0;
            return -1;
        }
        D_01E4F278.unk8C++;
        break;
    case 2:
        if (MC_CARD_INFO->type[port][slot] != 2) {
            if (MC_CARD_INFO->type[port][slot] == 3) {
                D_01E4F278.timeout = 1;
            } else {
                D_01E4F278.timeout = 1;
            }
            D_01E4F278.state = 0;
            return -4;
        }
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
        break;
    case 3:
        if (sceMcGetDir(port, slot, (char *)D_01E4F278.unkA0, 0, 1, &D_01E4F780) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x64;
                D_01E4F278.state = 0;
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
                        D_01E4F278.timeout = 0x64;
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
        quantity = D_01E4F278.unkA8;
        if (D_01E4F780.FileSizeByte == 0 && available < 3) {
            D_01E4F278.timeout = 0x1F;
            D_01E4F278.state = 0;
            return -6;
        }
        quantity -= 0x3FB;
        D_01E4F240.total_size = D_01E4F278.unkA8 + 6;
        if (quantity > 0) {
            D_01E4F240.total_size += (quantity + 0x3FE) / 0x3FF;
        }
        D_01E4F240.total_size = (D_01E4F240.total_size + 0x1F) & 0xFFFFFFE0;
        quantity = (u32)(D_01E4F240.total_size + 0x3FF) >> 10;
        reserved = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserved = 0;
        }
        if (available - reserved < quantity) {
            D_01E4F278.timeout = 0x1F;
            D_01E4F278.state = 0;
            return -6;
        }
        D_01E4F278.unk8C = 9;
        break;
    case 9:
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
    case 10:
        open_mode = 3;
        if (D_01E4F780.FileSizeByte == 0) {
            open_mode = 0x203;
        }
        if (sceMcOpen(port, slot, (char *)D_01E4F278.unkA0, open_mode) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.state = 0;
                return -10;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 11:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    func_00218340(result);
                    if (D_01E4F278.timeout == 0x64) {
                        D_01E4F278.timeout = 0x3D;
                    }
                    D_01E4F278.state = 0;
                    return -5;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F334.file_fd = result;
                D_01E4F278.unk8C++;
                D_01E4F278.unk9C = 0;
            }
        }
        break;
    case 12:
        if (sceMcSeek(D_01E4F334.file_fd, D_01E4F240.total_size - 0x20, 0) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.unk8C = 0x64;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 13:
    case 18:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x3D;
                    D_01E4F278.unk8C = 0x64;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 14:
        D_01E4F278.unk9C = 0;
        memset(D_01E4F338, 0, 0x20);
        D_01E4F278.unk8C++;
    case 15:
        if (sceMcWrite(D_01E4F334.file_fd, D_01E4F338, 0x20) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.unk8C = 0x64;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 16:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2 || result == -8) {
                    D_01E4F278.timeout = 0x3D;
                    D_01E4F278.unk8C = 0x64;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk8C++;
                D_01E4F278.unk9C = 0;
            }
        }
        break;
    case 17:
        if (sceMcSeek(D_01E4F334.file_fd, 0, 0) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.unk8C = 0x64;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 19:
        if ((u32)D_01E4F278.unkA8 >= D_01E4F244) {
            memset(D_01E4F338, 0, 0x420);
            chunk = D_01E4F278.unkA8 - D_01E4F244;
            if (D_01E4F244 == 0) {
                if (chunk >= 0x3FB) {
                    chunk = 0x3FB;
                }
                *(int *)D_01E4F338 = D_01E4F278.unkA8;
                header_size = 4;
            } else {
                if (chunk >= 0x3FF) {
                    chunk = 0x3FF;
                }
                header_size = 0;
            }
            position = chunk + header_size;
            checksum = 0;
            if (chunk != 0) {
                memcpy(&D_01E4F338[header_size], (u8 *)D_01E4F278.unkB8 + D_01E4F244, chunk);
                for (index = 0; index < chunk; index++) {
                    checksum += ~D_01E4F338[header_size + index];
                }
                D_01E4F338[position] = checksum;
                position++;
            }
            if (D_01E4F244 == 0) {
                if (chunk < 0x3FB) {
                    D_01E4F338[position] = 1;
                    position++;
                    D_01E4F244++;
                }
            } else if (chunk < 0x3FF) {
                D_01E4F338[position] = 1;
                position++;
                D_01E4F244++;
            }
            D_01E4F244 += chunk;
            if (position & 0x1F) {
                position = (position & ~0x1F) + 0x20;
            }
            D_01E4F278.unk8C++;
            D_01E4F248 = position;
            D_01E4F278.unk9C = 0;
        } else {
            D_01E4F278.unk8C = 0x16;
        }
        break;
    case 20:
        if (sceMcWrite(D_01E4F334.file_fd, D_01E4F338, D_01E4F248) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.unk8C = 0x64;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 21:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2 || result == -8) {
                    D_01E4F278.timeout = 0x3D;
                    D_01E4F278.unk8C = 0x64;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk8C = 0x13;
                D_01E4F278.unk9C = 0;
            }
        }
        break;
    case 22:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.unk8C = 0x64;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 23:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.state = 0; // match: cleared before the result is tested, so the retry below is asked of an idle request
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x3D;
                    return -8;
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
        if (sceMcSync(1, NULL, &result) != 0) {
            if (D_01E4F334.file_fd < 0) { // match: never true, the descriptor is 0 or a sceMcSync result that was not negative
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.state = 0;
                return D_01E4F278.unk90 > -1 ? -999 : D_01E4F278.unk90;
            }
            D_01E4F278.unk8C++;
            D_01E4F278.unk9C = 0;
        }
        break;
    case 101:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x3D;
                D_01E4F278.state = 0;
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
            return D_01E4F278.unk90 > -1 ? -999 : D_01E4F278.unk90;
        }
        break;
    }
    return 0;
}
