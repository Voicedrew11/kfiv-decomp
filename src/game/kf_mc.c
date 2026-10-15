/* Memory card requests: an Mc_Start function records what to do in mc_state and Mc_Update steps the request's
   state machine once a frame. Every library call is retried up to three times before the request fails; the
   outcome is left in mc_state.status. */
#include "kf_mc.h"

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);
char *strcpy(char *, const char *);
int sceOpen(const char *, int);
int sceRead(int, void *, int);
int sceLseek(int, int, int);
int sceClose(int);

/* The module's .bss: a work area and the file state, packed at four bytes. */
#define MC_4BYTE __attribute__((section(".bss"), aligned(4)))

static u8 D_01E10000[0x3F240];      /* 0x1E10000 a zeroed work area no code of this unit names */
static int D_01E4F240 MC_4BYTE;      /* 0x1E4F240 size of the file being written (McFileProgress.total_size) */
static u32 D_01E4F244 MC_4BYTE;      /* 0x1E4F244 bytes of it written so far (McFileProgress.offset) */
static int D_01E4F248 MC_4BYTE;      /* 0x1E4F248 bytes to hand to sceMcWrite each step (McFileProgress.chunk_size) */
static int D_01E4F24C MC_4BYTE;  /* 0x1E4F24C bytes of the data read so far */
static int D_01E4F250 MC_4BYTE;  /* 0x1E4F250 the data length the file starts with */
static int D_01E4F254 MC_4BYTE;  /* 0x1E4F254 where the end mark is in the last 0x20 bytes of the file */
static int D_01E4F258 MC_4BYTE;  /* 0x1E4F258 size of the whole file, a multiple of 0x20 */
static u32 D_01E4F25C MC_4BYTE;  /* 0x1E4F25C bytes written so far */
static int D_01E4F260 MC_4BYTE;  /* 0x1E4F260 bytes to hand to sceMcWrite this step */
static int D_01E4F264 MC_4BYTE;  /* 0x1E4F264 the file being copied from */
static int D_01E4F268 MC_4BYTE;  /* 0x1E4F268 bytes read from it this step */
static int D_01E4F26C MC_4BYTE;  /* 0x1E4F26C its size */
static int D_01E4F270 MC_4BYTE;  /* 0x1E4F270 bytes copied so far */
static u32 D_01E4F274 MC_4BYTE;  /* 0x1E4F274 bytes read so far */
static McState D_01E4F278;          /* 0x1E4F278 the request in progress and what it works on */
static int D_01E4F334 MC_4BYTE;         /* 0x1E4F334 the open file (McFileWork.file_fd) */
static u8 D_01E4F338[0x448] MC_4BYTE;    /* 0x1E4F338 the block being built (McFileWork.buffer) */
static sceMcTblGetDir D_01E4F780;       /* 0x1E4F780 the entry of the file a write or copy goes to */

#define MC_CARD_INFO ((McCardInfo *)&D_01E4F278)

static int Mc_GetInfoStatus(int result);
static void Mc_SetErrorStatus(int error);
static int Mc_StepCheck(void);
static int Mc_StepGetDir(void);
static int Mc_StepWriteChecked(void);
static int Mc_StepReadChecked(void);
static int Mc_StepWrite(void);
static int Mc_StepCopyFile(void);
static int Mc_StepRead(void);
static int Mc_StepFormat(void);
static int Mc_StepChdir(void);
static int Mc_StepMkdir(void);
static int Mc_StepFindFile(void);

/* One step of asking for a card's information within a request: asks three times at most (steps 0, 2, 4) and
   waits for each answer (steps 1, 3, 5). Returns the next step, the answer once step 5 has it (also kept in
   result[port][slot]), an error answer from steps 1 and 3, or -100 when the third call fails. */
static int Mc_StepGetInfo(int port, int slot, int *type, int *free_clusters, int *format, int step) {
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

/* The status for the answer of a card check of the current port and slot: MC_STATUS_OK for a formatted PS2
   card, MC_STATUS_NO_CARD for a PS1 card or no answer, MC_STATUS_UNFORMATTED, 2 or 4 for a new card (sceMcGetInfo
   -1 and -2), MC_STATUS_ERROR otherwise. */
static int Mc_GetInfoStatus(int result) {
    int port = D_01E4F278.port;
    int slot = D_01E4F278.slot;

    if (result >= 0) {
        if (MC_CARD_INFO->format[port][slot] == 0) {
            if (MC_CARD_INFO->type[port][slot] == 3) {
                return MC_STATUS_NO_CARD;
            }
            return MC_STATUS_UNFORMATTED;
        }
        return MC_CARD_INFO->type[port][slot] == 3;
    }
    if (result == -1) {
        if (MC_CARD_INFO->type[port][slot] == 3) {
            return MC_STATUS_NO_CARD;
        }
        return 2;
    }
    if (result == -2) {
        if (MC_CARD_INFO->type[port][slot] == 3) {
            return MC_STATUS_NO_CARD;
        }
        return 4;
    }
    if (result <= -11) {
        return MC_STATUS_NO_CARD;
    }
    return MC_STATUS_ERROR;
}

/* Sets the status for a failed sceMcOpen (or any negative library result). */
static void Mc_SetErrorStatus(int error) {
    if (error >= 0) {
        D_01E4F278.status = MC_STATUS_OK;
        return;
    }
    if (error == -2) {
        D_01E4F278.status = MC_STATUS_UNFORMATTED;
        return;
    }
    if (error == -3) {
        D_01E4F278.status = MC_STATUS_CARD_FULL;
        return;
    }
    if (error == -4) {
        D_01E4F278.status = MC_STATUS_ERROR;
        return;
    }
    if (error == -5) {
        D_01E4F278.status = MC_STATUS_ERROR;
        return;
    }
    if (error == -7) {
        D_01E4F278.status = MC_STATUS_ERROR;
        return;
    }
    if (error < -0xA) {
        D_01E4F278.status = MC_STATUS_NO_CARD;
        return;
    }
    D_01E4F278.status = MC_STATUS_ERROR;
}

/* Steps the "check the card" request: 0 while it runs, 1 when a usable card is there, -10 otherwise. */
static int Mc_StepCheck(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->step;
    int port = mc_state->port;
    int slot = mc_state->slot;
    int message;

    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcGetInfo(port, slot, &MC_CARD_INFO->type[port][slot], &MC_CARD_INFO->free_clusters[port][slot],
                         &MC_CARD_INFO->format[port][slot]) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_ERROR;
                return -10;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            MC_CARD_INFO->result[port][slot] = result;
            if (result < 0) {
                if (mc_state->retries == step) {
                    mc_state->status = Mc_GetInfoStatus(result);
                    return -10;
                }
            } else {
                mc_state->request = MC_REQUEST_NONE;
                message = Mc_GetInfoStatus(result);
                mc_state->status = message;
                if (message == MC_STATUS_OK) {
                    return 1;
                }
                return -10;
            }
            mc_state->retries++;
            mc_state->step--;
        }
        break;
    default:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_ERROR;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        break;
    }
    return 0;
}

/* Steps the "read a directory" request: 0 while it runs, 1 when done (the entry count in dir_count), negative
   on an error. */
static int Mc_StepGetDir(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->step;
    int port = mc_state->port;
    int slot = mc_state->slot;

    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcGetDir(port, slot, mc_state->path, mc_state->size, mc_state->count, mc_state->dir_entries) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_ERROR;
                mc_state->request = MC_REQUEST_NONE;
                return -1;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result >= 0) {
                mc_state->dir_count = result;
                mc_state->status = MC_STATUS_OK;
                mc_state->request = MC_REQUEST_NONE;
                return 1;
            }
            if (mc_state->retries == step) {
                if (result == -2) {
                    mc_state->status = MC_STATUS_UNFORMATTED;
                } else if (result == -4) {
                    mc_state->status = MC_STATUS_ERROR;
                } else {
                    mc_state->status = MC_STATUS_ERROR;
                }
                return result;
            }
            mc_state->retries++;
            mc_state->step--;
        }
        break;
    default:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_ERROR;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -1;
        }
        break;
    }
    return 0;
}

/* Steps the "write a checked file" request: 0 while it runs, 1 when the file is written, negative on an error.
   The file is the data length, the data and a check byte per 0x400-byte block, an end mark, padding to 0x20
   bytes and a 0x20-byte trailer of zeroes that is written first. */
static int Mc_StepWriteChecked(void) {
    int result;
    int step = D_01E4F278.step;
    int slot = D_01E4F278.slot;
    int port = D_01E4F278.port;
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
        D_01E4F334 = 0;
        D_01E4F244 = 0;
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
        D_01E4F248 = 0;
        break;
    case 1:
        if ((D_01E4F278.info_step = Mc_StepGetInfo(port, slot, &MC_CARD_INFO->type[port][slot],
                                                   &MC_CARD_INFO->free_clusters[port][slot],
                                                   &MC_CARD_INFO->format[port][slot], D_01E4F278.info_step)) > 0) {
            break;
        }
        if ((D_01E4F278.status = Mc_GetInfoStatus(D_01E4F278.info_step)) != MC_STATUS_OK) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -1;
        }
        D_01E4F278.step++;
        break;
    case 2:
        if (MC_CARD_INFO->type[port][slot] != 2) {
            if (MC_CARD_INFO->type[port][slot] == 3) {
                D_01E4F278.status = MC_STATUS_NO_CARD;
            } else {
                D_01E4F278.status = MC_STATUS_NO_CARD;
            }
            D_01E4F278.request = MC_REQUEST_NONE;
            return -4;
        }
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
        break;
    case 3:
        if (sceMcGetDir(port, slot, D_01E4F278.path, 0, 1, &D_01E4F780) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_ERROR;
                D_01E4F278.request = MC_REQUEST_NONE;
                return -1;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 4:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    if (result == -2) {
                        D_01E4F278.status = MC_STATUS_UNFORMATTED;
                    } else if (result == -4) {
                        D_01E4F780.FileSizeByte = 0;
                        D_01E4F278.step++;
                        break;
                    } else {
                        D_01E4F278.status = MC_STATUS_ERROR;
                    }
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                if (result == 0) {
                    D_01E4F780.FileSizeByte = 0;
                }
                D_01E4F278.step++;
            }
        }
        break;
    case 5:
        available = ((D_01E4F780.FileSizeByte + 0x3FF) >> 10) + MC_CARD_INFO->free_clusters[port][slot];
        quantity = D_01E4F278.size;
        if (D_01E4F780.FileSizeByte == 0 && available < 3) {
            D_01E4F278.status = MC_STATUS_NO_ROOM;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -6;
        }
        quantity -= 0x3FB;
        D_01E4F240 = D_01E4F278.size + 6;
        if (quantity > 0) {
            D_01E4F240 += (quantity + 0x3FE) / 0x3FF;
        }
        D_01E4F240 = (D_01E4F240 + 0x1F) & 0xFFFFFFE0;
        quantity = (u32)(D_01E4F240 + 0x3FF) >> 10;
        reserved = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserved = 0;
        }
        if (available - reserved < quantity) {
            D_01E4F278.status = MC_STATUS_NO_ROOM;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -6;
        }
        D_01E4F278.step = 9;
        break;
    case 9:
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
    case 10:
        open_mode = 3;
        if (D_01E4F780.FileSizeByte == 0) {
            open_mode = 0x203;
        }
        if (sceMcOpen(port, slot, D_01E4F278.path, open_mode) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.request = MC_REQUEST_NONE;
                return -10;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 11:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    Mc_SetErrorStatus(result);
                    if (D_01E4F278.status == MC_STATUS_ERROR) {
                        D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    }
                    D_01E4F278.request = MC_REQUEST_NONE;
                    return -5;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F334 = result;
                D_01E4F278.step++;
                D_01E4F278.retries = 0;
            }
        }
        break;
    case 12:
        if (sceMcSeek(D_01E4F334, D_01E4F240 - 0x20, 0) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.step = 0x64;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 13:
    case 18:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    D_01E4F278.step = 0x64;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.step++;
            }
        }
        break;
    case 14:
        D_01E4F278.retries = 0;
        memset(D_01E4F338, 0, 0x20);
        D_01E4F278.step++;
    case 15:
        if (sceMcWrite(D_01E4F334, D_01E4F338, 0x20) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.step = 0x64;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 16:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2 || result == -8) {
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    D_01E4F278.step = 0x64;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.step++;
                D_01E4F278.retries = 0;
            }
        }
        break;
    case 17:
        if (sceMcSeek(D_01E4F334, 0, 0) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.step = 0x64;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 19:
        if ((u32)D_01E4F278.size >= D_01E4F244) {
            memset(D_01E4F338, 0, 0x420);
            chunk = D_01E4F278.size - D_01E4F244;
            if (D_01E4F244 == 0) {
                if (chunk >= 0x3FB) {
                    chunk = 0x3FB;
                }
                *(int *)D_01E4F338 = D_01E4F278.size;
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
                memcpy(&D_01E4F338[header_size], D_01E4F278.write_data + D_01E4F244, chunk);
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
            D_01E4F278.step++;
            D_01E4F248 = position;
            D_01E4F278.retries = 0;
        } else {
            D_01E4F278.step = 0x16;
        }
        break;
    case 20:
        if (sceMcWrite(D_01E4F334, D_01E4F338, D_01E4F248) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.step = 0x64;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 21:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2 || result == -8) {
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    D_01E4F278.step = 0x64;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.step = 0x13;
                D_01E4F278.retries = 0;
            }
        }
        break;
    case 22:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.step = 0x64;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 23:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.request = MC_REQUEST_NONE; // match: cleared before the result is tested, so the retry below is asked of an idle request
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    return -8;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.status = MC_STATUS_WRITTEN;
                return 1;
            }
        }
        break;
    case 100:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (D_01E4F334 < 0) { // match: never true, the descriptor is 0 or a sceMcSync result that was not negative
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.request = MC_REQUEST_NONE;
                return D_01E4F278.info_step > -1 ? -999 : D_01E4F278.info_step;
            }
            D_01E4F278.step++;
            D_01E4F278.retries = 0;
        }
        break;
    case 101:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                D_01E4F278.request = MC_REQUEST_NONE;
                return -999;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return D_01E4F278.info_step > -1 ? -999 : D_01E4F278.info_step;
        }
        break;
    }
    return 0;
}

/* Steps the "read a checked file" request for a file Mc_StepWriteChecked wrote: 0 while it runs, 1 when the
   data is read, negative on an error. Checks the end mark in the last 0x20 bytes first, then reads the data a
   0x400-byte block at a time and checks the check byte of each block. */
static int Mc_StepReadChecked(void) {
    int result;
    int port = D_01E4F278.port;
    int slot = D_01E4F278.slot;
    int message;
    int remaining;
    int chunk;
    int chunk_max;
    int index;
    u8 checksum;
    int padded_size;
    int read_size;
    int total_size;

    switch (D_01E4F278.step) {
    case 0:
        D_01E4F334 = -1;
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
        D_01E4F24C = 0;
    case 1:
        if ((D_01E4F278.info_step = Mc_StepGetInfo(port, slot, &MC_CARD_INFO->type[port][slot],
                                                   &MC_CARD_INFO->free_clusters[port][slot],
                                                   &MC_CARD_INFO->format[port][slot], D_01E4F278.info_step)) > 0) {
            break;
        }
        message = Mc_GetInfoStatus(D_01E4F278.info_step);
        D_01E4F278.status = message;
        if (message != MC_STATUS_OK) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        if (MC_CARD_INFO->type[port][slot] != 2) {
            if (MC_CARD_INFO->type[port][slot] == 3) {
                D_01E4F278.status = MC_STATUS_NO_CARD;
            } else {
                D_01E4F278.status = MC_STATUS_NO_CARD;
            }
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 2:
        if (sceMcOpen(port, slot, D_01E4F278.path, 1) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                return -10;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 3:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    Mc_SetErrorStatus(result);
                    if (D_01E4F278.status == MC_STATUS_ERROR) {
                        D_01E4F278.status = MC_STATUS_READ_FAILED;
                    }
                    D_01E4F278.request = MC_REQUEST_NONE;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F334 = result;
                D_01E4F278.step++;
                D_01E4F278.retries = 0;
            }
        }
        break;
    case 4:
        if (sceMcRead(D_01E4F334, &D_01E4F250, 4) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 5:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.step++;
            }
        }
        break;
    case 6:
        remaining = D_01E4F250;
        D_01E4F258 = remaining + 6;
        remaining -= 0x3FB;
        if (remaining > 0) {
            D_01E4F258 += (remaining + 0x3FE) / 0x3FF;
        }
        padded_size = D_01E4F258 + 0x1F;
        total_size = padded_size & 0xFFFFFFE0;
        D_01E4F254 = padded_size - total_size;
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        D_01E4F258 = total_size;
        break;
    case 7:
        if (sceMcSeek(D_01E4F334, D_01E4F258 - 0x20, 0) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 8:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.retries = 0;
                D_01E4F278.step++;
            }
        }
        break;
    case 9:
        if (sceMcRead(D_01E4F334, D_01E4F338, 0x20) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 10:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else if (D_01E4F338[D_01E4F254] != 1) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries = 0;
                D_01E4F278.step++;
            }
        }
        break;
    case 11:
        if (sceMcSeek(D_01E4F334, 4, 0) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 12:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F278.step++;
                memset(D_01E4F338, 0, 0x400);
                D_01E4F278.retries = 0;
            }
        }
        break;
    case 13:
        read_size = 0x400;
        if (D_01E4F24C == 0) {
            read_size = 0x3FC;
        }
        if (sceMcRead(D_01E4F334, D_01E4F338, read_size) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 14:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    if (result == -2) {
                        D_01E4F278.status = MC_STATUS_UNFORMATTED;
                    } else if (result < -10) {
                        D_01E4F278.status = MC_STATUS_NO_CARD;
                    } else {
                        D_01E4F278.status = MC_STATUS_READ_FAILED;
                    }
                    D_01E4F278.info_step = result;
                    D_01E4F278.step = 100;
                    break;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                chunk_max = 0x3FF;
                if (D_01E4F24C == 0) {
                    chunk_max = 0x3FB;
                }
                chunk = D_01E4F250 - D_01E4F24C;
                if (chunk_max < chunk) {
                    chunk = chunk_max;
                }
                if (chunk != 0) { // match: an empty block is not taken as the end; it steps back below and is read again
                    checksum = 0;
                    for (index = 0; index < chunk; index++) {
                        checksum += ~D_01E4F338[index];
                    }
                    if (checksum == D_01E4F338[chunk]) {
                        if ((u32)D_01E4F278.size < (u32)D_01E4F250) {
                            chunk = D_01E4F278.size - D_01E4F24C;
                        } else {
                            chunk = D_01E4F250 - D_01E4F24C;
                        }
                        if (chunk_max < chunk) {
                            chunk = chunk_max;
                        }
                        memcpy(D_01E4F278.read_buffer + D_01E4F24C, D_01E4F338, chunk);
                        if (chunk < chunk_max) {
                            D_01E4F278.step++;
                            break;
                        }
                    } else {
                        D_01E4F278.info_step = -399;
                        D_01E4F278.status = MC_STATUS_READ_FAILED;
                        D_01E4F278.step = 100;
                        break;
                    }
                }
                D_01E4F24C += chunk;
                D_01E4F278.retries = 0;
                D_01E4F278.step--;
            }
        }
        break;
    case 15:
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 16:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                return -100;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 17:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.request = MC_REQUEST_NONE;
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.status = MC_STATUS_READ_DONE;
                D_01E4F278.request = MC_REQUEST_NONE;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334 < 0) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -100;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 101:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                return -1;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -200;
        }
        break;
    }
    return 0;
}

/* Steps the "write a file" request: 0 while it runs, 1 when the file is written, negative on an error. The data
   goes out as it is, 0x400 bytes at a time. */
static int Mc_StepWrite(void) {
    int result;
    int port = D_01E4F278.port;
    int slot = D_01E4F278.slot;
    int message;
    int available;
    int reserve;
    int flags;

    switch (D_01E4F278.step) {
    case 0:
        D_01E4F334 = 0;
        D_01E4F25C = 0;
        D_01E4F278.step++;
        break;
    case 1:
        if ((D_01E4F278.info_step = Mc_StepGetInfo(port, slot, &MC_CARD_INFO->type[port][slot],
                                                   &MC_CARD_INFO->free_clusters[port][slot],
                                                   &MC_CARD_INFO->format[port][slot], D_01E4F278.info_step)) > 0) {
            break;
        }
        message = Mc_GetInfoStatus(D_01E4F278.info_step);
        D_01E4F278.status = message;
        if (message != MC_STATUS_OK) {
            if (message == MC_STATUS_ERROR) {
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            }
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        D_01E4F278.step++;
        break;
    case 2:
        if (MC_CARD_INFO->type[port][slot] != 2) {
            D_01E4F278.request = MC_REQUEST_NONE;
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            return -4;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 3:
        if (sceMcGetDir(port, slot, D_01E4F278.path, 0, 1, &D_01E4F780) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_ERROR;
                return -1;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 4:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    if (result == -2) {
                        D_01E4F278.status = MC_STATUS_UNFORMATTED;
                    } else if (result == -4) {
                        D_01E4F780.FileSizeByte = 0;
                        D_01E4F278.step++;
                        break;
                    } else {
                        D_01E4F278.status = MC_STATUS_ERROR;
                    }
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                if (result == 0) {
                    D_01E4F780.FileSizeByte = 0;
                }
                D_01E4F278.step++;
            }
        }
        break;
    case 5:
        available = ((D_01E4F780.FileSizeByte + 0x3FF) >> 10) + MC_CARD_INFO->free_clusters[port][slot];
        if (D_01E4F780.FileSizeByte == 0 && available < 3) {
            D_01E4F278.request = MC_REQUEST_NONE;
            D_01E4F278.status = MC_STATUS_NO_ROOM;
            return -6;
        }
        reserve = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserve = 0;
        }
        if ((u32)(available - reserve) < (u32)(D_01E4F278.size + 0x3FF) >> 10) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -101;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step = 8;
        break;
    case 8:
        flags = 3;
        if (D_01E4F780.FileSizeByte == 0) {
            flags = 0x203;
        }
        if (sceMcOpen(port, slot, D_01E4F278.path, flags) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                return -11;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 9:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    Mc_SetErrorStatus(result);
                    if (D_01E4F278.status == MC_STATUS_ERROR) {
                        D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    }
                    D_01E4F278.step = 100;
                    D_01E4F278.info_step = -1;
                    break;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F334 = result;
                D_01E4F278.step++;
            }
        }
        break;
    case 10:
        D_01E4F260 = D_01E4F278.size - D_01E4F25C;
        if (D_01E4F260 > 0x400) {
            D_01E4F260 = 0x400;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 11:
        if (sceMcWrite(D_01E4F334, D_01E4F278.write_data + D_01E4F25C, D_01E4F260) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.info_step = -1;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 12:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2 || result == -8) {
                    D_01E4F278.info_step = -1;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F25C += result;
                D_01E4F278.retries = 0;
                if (D_01E4F25C >= (u32)D_01E4F278.size) {
                    D_01E4F278.step++;
                } else {
                    D_01E4F278.step = 10;
                }
            }
        }
        break;
    case 13:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.info_step = -1;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 14:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.request = MC_REQUEST_NONE;
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.status = MC_STATUS_WRITTEN;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334 < 0) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -999;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 101:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                return -999;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -999;
        }
        break;
    }
    return 0;
}

/* Steps the "copy a file to the card" request: 0 while it runs, 1 when the file is written, negative on an
   error. The file is read with sceRead 0x400 bytes at a time. */
static int Mc_StepCopyFile(void) {
    char path[0x40];
    int result;
    int port = D_01E4F278.port;
    int slot = D_01E4F278.slot;
    int message;
    int available;
    int reserve;
    int flags;
    int source;

    switch (D_01E4F278.step) {
    case 0:
        D_01E4F334 = 0;
        D_01E4F26C = 0;
        D_01E4F270 = 0;
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
        break;
    case 1:
        if ((D_01E4F278.info_step = Mc_StepGetInfo(port, slot, &MC_CARD_INFO->type[port][slot],
                                                   &MC_CARD_INFO->free_clusters[port][slot],
                                                   &MC_CARD_INFO->format[port][slot], D_01E4F278.info_step)) > 0) {
            break;
        }
        message = Mc_GetInfoStatus(D_01E4F278.info_step);
        D_01E4F278.status = message;
        if (message != MC_STATUS_OK) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        D_01E4F278.step++;
        break;
    case 2:
        if (MC_CARD_INFO->type[port][slot] != 2) {
            D_01E4F278.request = MC_REQUEST_NONE;
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            return -4;
        }
        D_01E4F278.step++;
        break;
    case 3:
        if (sceMcGetDir(port, slot, D_01E4F278.path, 0, 1, &D_01E4F780) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_ERROR;
                return -1;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 4:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    if (result == -2) {
                        D_01E4F278.status = MC_STATUS_UNFORMATTED;
                    } else if (result == -4) {
                        D_01E4F780.FileSizeByte = 0;
                        D_01E4F278.step++;
                        break;
                    } else {
                        D_01E4F278.status = MC_STATUS_ERROR;
                    }
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                if (result == 0) {
                    D_01E4F780.FileSizeByte = 0;
                }
                D_01E4F278.step++;
            }
        }
        break;
    case 5:
        available = ((D_01E4F780.FileSizeByte + 0x3FF) >> 10) + MC_CARD_INFO->free_clusters[port][slot];
        if (D_01E4F780.FileSizeByte == 0 && available < 3) {
            D_01E4F278.request = MC_REQUEST_NONE;
            D_01E4F278.status = MC_STATUS_NO_ROOM;
            return -6;
        }
        strcpy(path, D_01E4F278.second_path);
        source = sceOpen(path, 1);
        D_01E4F264 = source;
        if (source < 0) {
            D_01E4F278.request = MC_REQUEST_NONE;
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            return source;
        }
        if (D_01E4F278.count != 0) {
            D_01E4F26C = D_01E4F278.count;
        } else {
            D_01E4F26C = sceLseek(source, 0, 2);
            sceLseek(D_01E4F264, 0, 0);
        }
        reserve = 2;
        if (D_01E4F780.FileSizeByte != 0) {
            reserve = 0;
        }
        if (available - reserve < (D_01E4F26C + 0x3FF) / 0x400) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -101;
        }
        D_01E4F278.step = 8;
        D_01E4F278.retries = 0;
        break;
    case 8:
        flags = 3;
        if (D_01E4F780.FileSizeByte == 0) {
            flags = 0x203;
        }
        if (sceMcOpen(port, slot, D_01E4F278.path, flags) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                return -11;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 9:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    Mc_SetErrorStatus(result);
                    if (D_01E4F278.status == MC_STATUS_ERROR) {
                        D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    }
                    D_01E4F278.step = 100;
                    D_01E4F278.info_step = -1;
                    break;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F334 = result;
                D_01E4F278.step++;
            }
        }
        break;
    case 10:
        if ((D_01E4F268 = sceRead(D_01E4F264, D_01E4F338, 0x400)) <= 0) {
            D_01E4F278.step = 13;
        } else {
            D_01E4F278.retries = 0;
            D_01E4F278.step++;
        }
        break;
    case 11:
        if (sceMcWrite(D_01E4F334, D_01E4F338, D_01E4F268) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.info_step = -1;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 12:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2 || result == -8) {
                    D_01E4F278.info_step = -1;
                    D_01E4F278.step = 100;
                } else {
                    D_01E4F278.retries++;
                    D_01E4F278.step--;
                }
            } else {
                D_01E4F270 += D_01E4F268;
                D_01E4F278.retries = 0;
                if (D_01E4F270 < D_01E4F26C) {
                    D_01E4F278.step -= 2;
                } else {
                    D_01E4F278.step++;
                }
            }
        }
        break;
    case 13:
        sceClose(D_01E4F264);
        D_01E4F278.step++;
        break;
    case 14:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.info_step = -1;
                D_01E4F278.step = 100;
            } else {
                D_01E4F278.retries++;
            }
        } else {
            D_01E4F278.step++;
        }
        break;
    case 15:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.request = MC_REQUEST_NONE;
                    D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.status = MC_STATUS_WRITTEN;
                D_01E4F278.request = MC_REQUEST_NONE;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F264 != 0) {
            sceClose(D_01E4F264);
        }
        if (D_01E4F334 < 0) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -999;
        }
        D_01E4F278.retries = 0; // match: the step is not advanced here, so the request stays in step 100
        break;
    case 101:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_WRITE_FAILED;
                return -999;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_WRITE_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -999;
        }
        break;
    }
    return 0;
}

/* Steps the "read a file" request: 0 while it runs, 1 when the data is read, negative on an error. */
static int Mc_StepRead(void) {
    int result;
    int port = D_01E4F278.port;
    int slot = D_01E4F278.slot;
    int message;

    switch (D_01E4F278.step) {
    case 0:
        D_01E4F334 = 0;
        D_01E4F274 = 0;
        D_01E4F278.step++;
        D_01E4F278.retries = 0;
    case 1:
        if ((D_01E4F278.info_step = Mc_StepGetInfo(port, slot, &MC_CARD_INFO->type[port][slot],
                                                   &MC_CARD_INFO->free_clusters[port][slot],
                                                   &MC_CARD_INFO->format[port][slot], D_01E4F278.info_step)) > 0) {
            break;
        }
        message = Mc_GetInfoStatus(D_01E4F278.info_step);
        D_01E4F278.status = message;
        if (message != MC_STATUS_OK) {
            D_01E4F278.request = MC_REQUEST_NONE;
            return -10;
        }
        if (MC_CARD_INFO->type[port][slot] != 2) {
            D_01E4F278.step = 100;
            break;
        }
        D_01E4F278.step++;
        break;
    case 2:
        if (sceMcOpen(port, slot, D_01E4F278.path, 1) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.step = 100;
                break;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 3:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    Mc_SetErrorStatus(result);
                    if (D_01E4F278.status == MC_STATUS_ERROR) {
                        D_01E4F278.status = MC_STATUS_READ_FAILED;
                    }
                    D_01E4F278.step = 100;
                    D_01E4F278.request = MC_REQUEST_NONE;
                    D_01E4F278.info_step = -1;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F334 = result;
                D_01E4F278.step++;
            }
        }
        break;
    case 4:
        memset(D_01E4F338, 0, 0x400);
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 5:
        if (sceMcRead(D_01E4F334, D_01E4F338, 0x400) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.step = 100;
                break;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 6:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.info_step = result;
                    D_01E4F278.step = 100;
                    break;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.retries = 0;
                if ((u32)D_01E4F278.size < D_01E4F274 + result) { // match: looks inverted: the clipped copy goes back to read again, the other goes on to close
                    result = D_01E4F278.size - D_01E4F274;
                    memcpy(D_01E4F278.read_buffer + D_01E4F274, D_01E4F338, result);
                    D_01E4F278.step--;
                } else {
                    memcpy(D_01E4F278.read_buffer + D_01E4F274, D_01E4F338, result);
                    D_01E4F274 += result;
                    D_01E4F278.step++;
                }
            }
        }
        break;
    case 7:
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 8:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_READ_FAILED;
                return -1;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 9:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.request = MC_REQUEST_NONE; // match: cleared before the result is tested, as in Mc_StepWriteChecked step 23
            if (result < 0) {
                if (D_01E4F278.retries == 2) {
                    D_01E4F278.status = MC_STATUS_READ_FAILED;
                    return result;
                }
                D_01E4F278.retries++;
                D_01E4F278.step--;
            } else {
                D_01E4F278.status = MC_STATUS_READ_DONE;
                return 1;
            }
        }
        break;
    case 100:
        if (D_01E4F334 < 0) {
            D_01E4F278.status = MC_STATUS_READ_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return D_01E4F278.info_step;
        }
        D_01E4F278.retries = 0;
        D_01E4F278.step++;
        break;
    case 101:
        if (sceMcClose(D_01E4F334) != 0) {
            if (D_01E4F278.retries == 2) {
                D_01E4F278.request = MC_REQUEST_NONE;
                D_01E4F278.status = MC_STATUS_READ_FAILED;
            }
            D_01E4F278.retries++;
        } else {
            D_01E4F278.step++;
        }
        break;
    case 102:
        if (sceMcSync(1, NULL, &result) != 0) {
            D_01E4F278.status = MC_STATUS_READ_FAILED;
            D_01E4F278.request = MC_REQUEST_NONE;
            return -100;
        }
        break;
    }
    return 0;
}

/* Steps the "format the card" request: 0 while it runs, 1 when the card is formatted, negative on an error. */
static int Mc_StepFormat(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->step;
    int port = mc_state->port;
    int slot = mc_state->slot;

    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcFormat(port, slot) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_FORMAT_FAILED;
                return -10;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result == 0) {
                mc_state->request = MC_REQUEST_NONE;
                mc_state->status = MC_STATUS_FORMATTED;
                return 1;
            }
            if (mc_state->retries == step) {
                mc_state->request = MC_REQUEST_NONE;
                mc_state->status = MC_STATUS_FORMAT_FAILED;
                return result;
            }
            mc_state->retries++;
            mc_state->step--;
        }
        break;
    }
    return 0;
}

/* Steps the "change directory" request: 0 while it runs, 1 when done, negative on an error. */
static int Mc_StepChdir(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->step;
    int port = mc_state->port;
    int slot = mc_state->slot;

    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcChdir(port, slot, mc_state->path, mc_state->second_path) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_ERROR;
                return -10;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result == 0) {
                mc_state->request = MC_REQUEST_NONE;
                mc_state->status = MC_STATUS_OK;
                return 1;
            }
            if (mc_state->retries == step) {
                mc_state->request = MC_REQUEST_NONE;
                if (result == -2) {
                    mc_state->status = MC_STATUS_UNFORMATTED;
                } else {
                    mc_state->status = MC_STATUS_ERROR;
                }
                return result;
            }
            mc_state->retries++;
            mc_state->step--;
        }
        break;
    }
    return 0;
}

/* Steps the "make directory" request: 0 while it runs, 1 when done, negative on an error. */
static int Mc_StepMkdir(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int step = mc_state->step;
    int port = mc_state->port;
    int slot = mc_state->slot;

    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcMkdir(port, slot, mc_state->path) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_ERROR;
                return -10;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result == 0) {
                mc_state->status = MC_STATUS_OK;
                mc_state->request = MC_REQUEST_NONE;
                return 1;
            }
            if (mc_state->retries == step) {
                mc_state->status = MC_STATUS_ERROR;
                mc_state->request = MC_REQUEST_NONE;
                return result;
            }
            mc_state->retries++;
            mc_state->step--;
        }
        break;
    }
    return 0;
}

/* Steps the "look for a file" request: 0 while it runs, 1 when done (the status says whether the file is
   there), negative on an error. */
static int Mc_StepFindFile(void) {
    McState *mc_state = &D_01E4F278;
    int result;
    int port = mc_state->port;
    int slot = mc_state->slot;
    int step = mc_state->step;
    switch (step) {
    case 0:
        mc_state->step = 1;
        mc_state->retries = 0;
        break;
    case 1:
        if (sceMcGetDir(port, slot, mc_state->path, 0, 1, &D_01E4F780) != 0) {
            if (mc_state->retries == 2) {
                mc_state->status = MC_STATUS_ERROR;
                mc_state->request = MC_REQUEST_NONE;
                return -1;
            }
            mc_state->retries++;
        } else {
            mc_state->step++;
        }
        break;
    case 2:
        if (sceMcSync(1, NULL, &result) != 0) {
            if (result < 0) {
                if (mc_state->retries == step) {
                    if (result == -2) {
                        mc_state->status = MC_STATUS_UNFORMATTED;
                    } else if (result == -4) {
                        mc_state->status = MC_STATUS_ERROR;
                    } else {
                        mc_state->status = MC_STATUS_ERROR;
                    }
                    return result;
                }
                mc_state->retries++;
                mc_state->step--;
            } else {
                if (result > 0) {
                    mc_state->status = MC_STATUS_FOUND;
                }
                if (result == 0) {
                    mc_state->status = MC_STATUS_NOT_FOUND;
                }
                mc_state->request = MC_REQUEST_NONE;
                return 1;
            }
        }
        break;
    }
    return 0;
}

/* Starts checking the card in a port and slot. */
int Mc_StartCheck(u32 port, u32 slot) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->slot = slot;
    mc_state->request = MC_REQUEST_CHECK;
    mc_state->port = port;
    mc_state->step = 0;
    return 0;
}

/* Starts reading the entries of a directory that match path into entries. */
int Mc_StartGetDir(u32 port, u32 slot, char *path, int mode, int max_entries, sceMcTblGetDir *entries) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->dir_entries = entries;
    mc_state->request = MC_REQUEST_GET_DIR;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->size = mode;
    mc_state->count = max_entries;
    mc_state->step = 0;
    return 0;
}

/* Starts writing size bytes of data to the file in the checked form. */
int Mc_StartWriteChecked(u32 port, u32 slot, char *path, void *data, int size) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->status = MC_STATUS_WRITING;
    mc_state->request = MC_REQUEST_WRITE_CHECKED;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->write_data = data;
    mc_state->size = size;
    mc_state->info_step = 0;
    mc_state->step = 0;
    return 0;
}

/* Starts reading a checked file into buffer, at most size bytes. */
int Mc_StartReadChecked(u32 port, u32 slot, char *path, void *buffer, int size) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->status = MC_STATUS_READING;
    mc_state->request = MC_REQUEST_READ_CHECKED;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->read_buffer = buffer;
    mc_state->size = size;
    mc_state->info_step = mc_state->step = 0;
    return 0;
}

/* Starts writing size bytes of data to the file as they are. */
int Mc_StartWrite(u32 port, u32 slot, char *path, void *data, int size) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->status = MC_STATUS_WRITING;
    mc_state->request = MC_REQUEST_WRITE;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->write_data = data;
    mc_state->size = size;
    mc_state->step = 0;
    mc_state->info_step = 0;
    return 0;
}

/* Starts copying a file to the card: size bytes of it, or all of it when size is 0. */
int Mc_StartCopyFile(u32 port, u32 slot, char *path, char *source_path, int size) {
    if (D_01E4F278.request != MC_REQUEST_NONE) {
        return 1;
    }
    if ((port >= 3U) || (slot >= 5U)) {
        return -1;
    }
    D_01E4F278.status = MC_STATUS_WRITING;
    D_01E4F278.request = MC_REQUEST_COPY_FILE;
    D_01E4F278.port = port;
    D_01E4F278.slot = slot;
    D_01E4F278.path = path;
    D_01E4F278.second_path = source_path;
    D_01E4F278.count = size;
    D_01E4F278.info_step = D_01E4F278.step = 0;
    return 0;
}

/* Starts reading the file into buffer as it is, at most size bytes. */
int Mc_StartRead(u32 port, u32 slot, char *path, void *buffer, int size) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->status = MC_STATUS_READING;
    mc_state->request = MC_REQUEST_READ;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->read_buffer = buffer;
    mc_state->size = size;
    mc_state->info_step = mc_state->step = 0;
    return 0;
}

/* Starts formatting the card. */
int Mc_StartFormat(u32 port, u32 slot) {
    if (D_01E4F278.request != MC_REQUEST_NONE) {
        return 1;
    }
    if ((port >= 3U) || (slot >= 5U)) {
        return -1;
    }
    D_01E4F278.status = MC_STATUS_FORMATTING;
    D_01E4F278.request = MC_REQUEST_FORMAT;
    D_01E4F278.port = port;
    D_01E4F278.slot = slot;
    D_01E4F278.step = 0;
    return 0;
}

/* Starts changing the card's current directory to path; the old one goes to pwd. */
int Mc_StartChdir(u32 port, u32 slot, char *path, char *pwd) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->second_path = pwd;
    mc_state->request = MC_REQUEST_CHDIR;
    mc_state->port = port;
    mc_state->slot = slot;
    mc_state->path = path;
    mc_state->step = 0;
    return 0;
}

/* Starts making the directory path. */
int Mc_StartMkdir(u32 port, u32 slot, char *path) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->path = path;
    mc_state->request = MC_REQUEST_MKDIR;
    mc_state->slot = slot;
    mc_state->port = port;
    mc_state->step = 0;
    return 0;
}

/* Starts looking for the file path. */
int Mc_StartFindFile(u32 port, u32 slot, char *path) {
    McState *mc_state = &D_01E4F278;

    if (mc_state->request != MC_REQUEST_NONE) {
        return 1;
    }
    if (port >= 3U || slot >= 5U) {
        return -1;
    }
    mc_state->path = path;
    mc_state->request = MC_REQUEST_FIND_FILE;
    mc_state->slot = slot;
    mc_state->port = port;
    mc_state->step = 0;
    return 0;
}

/* Steps the request in progress. Returns 0 when idle or just finished, the request's number while it runs,
   and minus that number when it failed. */
int Mc_Update(void) {
    int result;

    switch (D_01E4F278.request) {
    case MC_REQUEST_CHECK:
        result = Mc_StepCheck();
        break;
    case MC_REQUEST_GET_DIR:
        result = Mc_StepGetDir();
        break;
    case MC_REQUEST_WRITE_CHECKED:
        result = Mc_StepWriteChecked();
        break;
    case MC_REQUEST_READ_CHECKED:
        result = Mc_StepReadChecked();
        break;
    case MC_REQUEST_WRITE:
        result = Mc_StepWrite();
        break;
    case MC_REQUEST_COPY_FILE:
        result = Mc_StepCopyFile();
        break;
    case MC_REQUEST_FORMAT:
        result = Mc_StepFormat();
        break;
    case MC_REQUEST_CHDIR:
        result = Mc_StepChdir();
        break;
    case MC_REQUEST_MKDIR:
        result = Mc_StepMkdir();
        break;
    case MC_REQUEST_READ:
        result = Mc_StepRead();
        break;
    case MC_REQUEST_FIND_FILE:
        result = Mc_StepFindFile();
        break;
    case MC_REQUEST_NONE:
        return 0;
    default:
        result = -1;
        break;
    }
    if (result < 0) {
        result = -D_01E4F278.request;
        D_01E4F278.request = MC_REQUEST_NONE;
        D_01E4F278.step = 0;
        if (result == 0) {
            return -100;
        }
        return result;
    }
    if (result > 0) {
        D_01E4F278.request = MC_REQUEST_NONE;
        D_01E4F278.step = 0;
        return 0;
    }
    return D_01E4F278.request;
}

/* Sets the status from the last check of a card. Returns -1, -2, -5 or -6 when the card cannot be used, else
   minus the check's result (0 the same card as before, 1 and 2 a new one). */
int Mc_GetCardStatus(int port, int slot) {
    int type = MC_CARD_INFO->type[port][slot];
    int result;

    if (type == 1) {
        D_01E4F278.status = MC_STATUS_NO_CARD;
        return -1;
    }
    result = MC_CARD_INFO->result[port][slot];
    if (result < -2) {
        D_01E4F278.status = MC_STATUS_NO_CARD;
        return -2;
    }
    if (type != 2 && type != 3) {
        D_01E4F278.status = MC_STATUS_NO_CARD;
        return -5;
    }
    result = -result;
    if (type == 3) {
        switch (result) {
        case 3:
            D_01E4F278.status = 0x14;
            break;
        case 4:
            D_01E4F278.status = 0x16;
            break;
        case 5:
            D_01E4F278.status = 0x15;
            break;
        default:
            D_01E4F278.status = MC_STATUS_NO_CARD;
            break;
        }
    } else {
        switch (result) {
        case 0:
            D_01E4F278.status = MC_STATUS_OK;
            break;
        case 1:
            D_01E4F278.status = 2;
            break;
        case 2:
            D_01E4F278.status = MC_STATUS_UNFORMATTED;
            break;
        default:
            D_01E4F278.status = MC_STATUS_NO_CARD;
            break;
        }
    }
    if (D_01E4F278.status == MC_STATUS_NO_CARD) {
        return -6;
    }
    return result;
}

/* Starts the memory card library, then, if it started, clears the request and the check results. */
int Mc_Init(void) {
    int result;
    int count;
    int *words;

    result = sceMcInit();
    if (result == 0) {
        count = 3;
        D_01E4F278.step = D_01E4F278.request = MC_REQUEST_NONE;
        words = &MC_CARD_INFO->result[1][0];
        do {
            words[-4] = 0;
            count -= 1;
            words[0] = 0;
            words += 1;
        } while (count >= 0);
    }
    return result;
}

/* The result of the last check of this port and slot. */
int Mc_GetCheckResult(int port, int slot) {
    return MC_CARD_INFO->result[port][slot];
}

/* The free clusters of the card less two, or -1 when no card was seen. */
int Mc_GetFreeClusters(int port, int slot) {
    int free_clusters;

    if (MC_CARD_INFO->free_clusters[port][slot] == 0 && MC_CARD_INFO->type[port][slot] == 0) {
        D_01E4F278.status = MC_STATUS_NO_CARD;
        return -1;
    }
    free_clusters = MC_CARD_INFO->free_clusters[port][slot] - 2;
    if (free_clusters <= 0) {
        D_01E4F278.status = MC_STATUS_CARD_FULL;
        free_clusters = 0;
    }
    D_01E4F278.status = MC_STATUS_OK; // match: overwrites the MC_STATUS_CARD_FULL just stored
    return free_clusters;
}

/* The type of the card in this port and slot. */
int Mc_GetCardType(int port, int slot) {
    return MC_CARD_INFO->type[port][slot];
}

/* How many entries the last directory request found. */
s32 Mc_GetDirCount(void) {
    return D_01E4F278.dir_count;
}

/* The status of the last request. */
s32 Mc_GetStatus(void) {
    return D_01E4F278.status;
}

/* Returns clusters when the card has that many free (less two), -1 when it is full and -2 when it has too few;
   nothing calls it. */
int Mc_CheckRoom(int port, int slot, int clusters) {
    McState *mc_state = &D_01E4F278;
    int free_clusters = MC_CARD_INFO->free_clusters[port][slot] - 2;

    if (free_clusters <= 0) {
        mc_state->status = MC_STATUS_CARD_FULL;
        return -1;
    }
    if ((u32)free_clusters < (u32)clusters) {
        mc_state->status = MC_STATUS_NO_ROOM;
        return -2;
    }
    return clusters;
}
