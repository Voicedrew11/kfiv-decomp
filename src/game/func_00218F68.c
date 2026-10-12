#include "kf_mc.h"
#include <libmc.h>

int func_00218188(int, int, int *, int *, int *, int);
int func_00218270(int);
void func_00218340(int);
void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
extern u8 D_01E4F338[0x400];
extern int D_01E4F24C[]; /* bytes of the data read so far */
extern int D_01E4F250[]; /* the data length the file starts with */
extern int D_01E4F254[]; /* where the end mark is in the last 0x20 bytes of the file */
extern int D_01E4F258[]; /* size of the whole file, a multiple of 0x20 */

/* Steps the "read a file" request for a file func_002186E0 wrote: 0 while it runs, 1 when the data is
   read, negative on an error. Checks the end mark in the last 0x20 bytes first, then reads the data a
   0x400-byte block at a time and checks the check byte of each block. */
int func_00218F68(void) {
    int result;
    int port = D_01E4F278.unk80;
    int slot = D_01E4F278.unk84;
    int message;
    int remaining;
    int chunk;
    int chunk_max;
    int index;
    u8 checksum;
    int padded_size;
    int read_size;
    int total_size;

    switch (D_01E4F278.unk8C) {
    case 0:
        D_01E4F334.file_fd = -1;
        D_01E4F278.unk8C++;
        D_01E4F278.unk9C = 0;
        D_01E4F24C[0] = 0;
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
            if (MC_CARD_INFO->type[port][slot] == 3) {
                D_01E4F278.timeout = 1;
            } else {
                D_01E4F278.timeout = 1;
            }
            D_01E4F278.state = 0;
            return -10;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 2:
        if (sceMcOpen(port, slot, (char *)D_01E4F278.unkA0, 1) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x47;
                return -10;
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
                    D_01E4F278.state = 0;
                    return result;
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
    case 4:
        if (sceMcRead(D_01E4F334.file_fd, D_01E4F250, 4) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 5:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x47;
                    D_01E4F278.unk8C = 100;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 6:
        remaining = D_01E4F250[0];
        D_01E4F258[0] = remaining + 6;
        remaining -= 0x3FB;
        if (remaining > 0) {
            D_01E4F258[0] += (remaining + 0x3FE) / 0x3FF;
        }
        padded_size = D_01E4F258[0] + 0x1F;
        total_size = padded_size & 0xFFFFFFE0;
        D_01E4F254[0] = padded_size - total_size;
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        D_01E4F258[0] = total_size;
        break;
    case 7:
        if (sceMcSeek(D_01E4F334.file_fd, D_01E4F258[0] - 0x20, 0) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 8:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x47;
                    D_01E4F278.unk8C = 100;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk9C = 0;
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 9:
        if (sceMcRead(D_01E4F334.file_fd, D_01E4F338, 0x20) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C++;
            }
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 10:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x47;
                    D_01E4F278.unk8C = 100;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else if (D_01E4F338[D_01E4F254[0]] != 1) {
                D_01E4F278.timeout = 0x47;
                D_01E4F278.unk8C = 100;
            } else {
                D_01E4F278.unk9C = 0;
                D_01E4F278.unk8C++;
            }
        }
        break;
    case 11:
        if (sceMcSeek(D_01E4F334.file_fd, 4, 0) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
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
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.timeout = 0x47;
                    D_01E4F278.unk8C = 100;
                } else {
                    D_01E4F278.unk9C++;
                    D_01E4F278.unk8C--;
                }
            } else {
                D_01E4F278.unk8C++;
                memset(D_01E4F338, 0, 0x400);
                D_01E4F278.unk9C = 0;
            }
        }
        break;
    case 13:
        read_size = 0x400;
        if (D_01E4F24C[0] == 0) {
            read_size = 0x3FC;
        }
        if (sceMcRead(D_01E4F334.file_fd, D_01E4F338, read_size) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
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
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    if (result == -2) {
                        D_01E4F278.timeout = 3;
                    } else if (result < -10) {
                        D_01E4F278.timeout = 1;
                    } else {
                        D_01E4F278.timeout = 0x47;
                    }
                    D_01E4F278.unk90 = result;
                    D_01E4F278.unk8C = 100;
                    break;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                chunk_max = 0x3FF;
                if (D_01E4F24C[0] == 0) {
                    chunk_max = 0x3FB;
                }
                chunk = D_01E4F250[0] - D_01E4F24C[0];
                if (chunk_max < chunk) {
                    chunk = chunk_max;
                }
                if (chunk != 0) { // match: an empty block is not taken as the end; it steps back below and is read again
                    checksum = 0;
                    for (index = 0; index < chunk; index++) {
                        checksum += ~D_01E4F338[index];
                    }
                    if (checksum == D_01E4F338[chunk]) {
                        if ((u32)D_01E4F278.unkA8 < (u32)D_01E4F250[0]) {
                            chunk = D_01E4F278.unkA8 - D_01E4F24C[0];
                        } else {
                            chunk = D_01E4F250[0] - D_01E4F24C[0];
                        }
                        if (chunk_max < chunk) {
                            chunk = chunk_max;
                        }
                        memcpy((u8 *)D_01E4F278.unkB4 + D_01E4F24C[0], D_01E4F338, chunk);
                        if (chunk < chunk_max) {
                            D_01E4F278.unk8C++;
                            break;
                        }
                    } else {
                        D_01E4F278.unk90 = -399;
                        D_01E4F278.timeout = 0x47;
                        D_01E4F278.unk8C = 100;
                        break;
                    }
                }
                D_01E4F24C[0] += chunk;
                D_01E4F278.unk9C = 0;
                D_01E4F278.unk8C--;
            }
        }
        break;
    case 15:
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 16:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.state = 0;
                D_01E4F278.timeout = 0x47;
                return -100;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 17:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.unk9C == 2) {
                    D_01E4F278.state = 0;
                    D_01E4F278.timeout = 0x47;
                    return result;
                }
                D_01E4F278.unk9C++;
                D_01E4F278.unk8C--;
            } else {
                D_01E4F278.timeout = 0x48;
                D_01E4F278.state = 0;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334.file_fd < 0) {
            D_01E4F278.state = 0;
            return -100;
        }
        D_01E4F278.unk9C = 0;
        D_01E4F278.unk8C++;
        break;
    case 101:
        if (sceMcClose(D_01E4F334.file_fd) != 0) {
            if (D_01E4F278.unk9C == 2) {
                D_01E4F278.timeout = 0x47;
                return -1;
            }
            D_01E4F278.unk9C++;
        } else {
            D_01E4F278.unk8C++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.state = 0;
            return -200;
        }
        break;
    }
    return 0;
}
