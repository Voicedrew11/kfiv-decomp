/* Memory card requests (kf_mc.c): one request at a time (check a card, read a directory, write, read or copy
   a file, format, change or make a directory, look for a file), started by an Mc_Start function and stepped
   once a frame by Mc_Update; the save screens (kf_save_card.c) are built on them. */
#ifndef KF_MC_H
#define KF_MC_H

#include "common.h"
#include <libmc.h>

/* McState.request: the request in progress */
#define MC_REQUEST_NONE 0
#define MC_REQUEST_CHECK 3          /* Mc_StartCheck */
#define MC_REQUEST_GET_DIR 4        /* Mc_StartGetDir */
#define MC_REQUEST_WRITE_CHECKED 5  /* Mc_StartWriteChecked */
#define MC_REQUEST_READ_CHECKED 6   /* Mc_StartReadChecked */
#define MC_REQUEST_WRITE 7          /* Mc_StartWrite */
#define MC_REQUEST_COPY_FILE 8      /* Mc_StartCopyFile */
#define MC_REQUEST_FORMAT 9         /* Mc_StartFormat */
#define MC_REQUEST_CHDIR 10         /* Mc_StartChdir */
#define MC_REQUEST_MKDIR 11         /* Mc_StartMkdir */
#define MC_REQUEST_READ 12          /* Mc_StartRead */
#define MC_REQUEST_FIND_FILE 13     /* Mc_StartFindFile */

/* McState.status (Mc_GetStatus): how the last request went, or what it is doing */
#define MC_STATUS_OK 0
#define MC_STATUS_NO_CARD 1         /* nothing usable in the port: no card, a PS1 card, or the card stopped answering */
#define MC_STATUS_UNFORMATTED 3
#define MC_STATUS_CARD_FULL 0x1E
#define MC_STATUS_NO_ROOM 0x1F      /* not enough free clusters for the file */
#define MC_STATUS_NOT_FOUND 0x2D    /* Mc_StartFindFile: no such file */
#define MC_STATUS_FOUND 0x2E        /* Mc_StartFindFile: the file is there */
#define MC_STATUS_FORMAT_FAILED 0x32
#define MC_STATUS_FORMATTING 0x33
#define MC_STATUS_FORMATTED 0x34
#define MC_STATUS_WRITING 0x3C
#define MC_STATUS_WRITE_FAILED 0x3D
#define MC_STATUS_WRITTEN 0x3E
#define MC_STATUS_READING 0x46
#define MC_STATUS_READ_FAILED 0x47
#define MC_STATUS_READ_DONE 0x48
#define MC_STATUS_ERROR 0x64

/* what sceMcGetInfo reported per port and slot */
typedef struct {
    int type[2][4];                 /* 0x00 card type: 2 a PS2 card, 3 a PS1 card */
    int free_clusters[2][4];        /* 0x20 */
    int format[2][4];               /* 0x40 nonzero when formatted */
    int result[2][4];               /* 0x60 result of the sceMcGetInfo call */
} McCardInfo;

/* the request in progress and what it works on (mc_request) */
typedef struct {
    McCardInfo cards;               /* 0x00 */
    int port;                       /* 0x80 0..2 */
    int slot;                       /* 0x84 0..4 */
    int request;                    /* 0x88 MC_REQUEST_ */
    int step;                       /* 0x8C step of the request's state machine */
    int info_step;                  /* 0x90 step of Mc_StepGetInfo within a request, then its answer; also the error a request ends with */
    int dir_count;                  /* 0x94 entries the last directory request found (Mc_GetDirCount) */
    int status;                     /* 0x98 MC_STATUS_ */
    int retries;                    /* 0x9C failed calls of the current step; a request gives up after the third */
    char *path;                     /* 0xA0 the file or directory on the card */
    char *second_path;              /* 0xA4 Mc_StartChdir: where the old directory goes; Mc_StartCopyFile: the file copied from */
    int size;                       /* 0xA8 bytes to write or read; Mc_StartGetDir: sceMcGetDir's mode */
    int count;                      /* 0xAC Mc_StartGetDir: the most entries to read; Mc_StartCopyFile: bytes to copy, 0 the whole file */
    sceMcTblGetDir *dir_entries;    /* 0xB0 Mc_StartGetDir */
    u8 *read_buffer;                /* 0xB4 where a read puts the data */
    u8 *write_data;                 /* 0xB8 what a write sends */
} McState;

/* the PS2 memory card icon.sys header, built by SaveCard_BuildIconSys */
typedef struct {
    char head[4];                   /* 0x00 "PS2D" */
    char pad04[0x02];
    u16 line_break;                 /* 0x06 */
    char pad08[0x04];
    s32 transparency;               /* 0x0C */
    s32 bg_color[4][4];             /* 0x10 */
    f32 light_dir[3][4];            /* 0x50 */
    f32 light_color[3][4];          /* 0x80 */
    f32 ambient[4];                 /* 0xB0 */
    char title[68];                 /* 0xC0 */
    char view_name[64];             /* 0x104 */
    char copy_name[64];             /* 0x144 */
    char delete_name[64];           /* 0x184 */
    char pad1C4[0x200];
} McIconSys;

/* progress of the file being written in 0x400-byte blocks */
typedef struct {
    int total_size;                 /* 0x00 size of the whole file, a multiple of 0x20 */
    u32 offset;                     /* 0x04 bytes of the data done so far */
    int chunk_size;                 /* 0x08 bytes to hand to sceMcWrite this step */
} McFileProgress;

/* the open file and the block being built for it */
typedef struct {
    int file_fd;                    /* 0x00 */
    u8 buffer[0x420];               /* 0x04 */
} McFileWork;

/* Starts checking the card in a port and slot (port 0..2, slot 0..4). All Mc_Start functions return 0 when
   the request started, 1 while another one runs and -1 for a port or slot out of range. */
int Mc_StartCheck(u32 port, u32 slot);

/* Starts reading the entries of a directory that match path into entries (sceMcGetDir). */
int Mc_StartGetDir(u32 port, u32 slot, char *path, int mode, int max_entries, sceMcTblGetDir *entries);

/* Starts writing size bytes of data to the file in the game's checked form: the length first, a check byte
   after every block, an end mark. */
int Mc_StartWriteChecked(u32 port, u32 slot, char *path, void *data, int size);

/* Starts reading a file written by Mc_StartWriteChecked into buffer, at most size bytes, checking each block. */
int Mc_StartReadChecked(u32 port, u32 slot, char *path, void *buffer, int size);

/* Starts writing size bytes of data to the file as they are. */
int Mc_StartWrite(u32 port, u32 slot, char *path, void *data, int size);

/* Starts copying a file (sceOpen) to the card: size bytes of it, or all of it when size is 0. */
int Mc_StartCopyFile(u32 port, u32 slot, char *path, char *source_path, int size);

/* Starts reading the file into buffer as it is, at most size bytes. */
int Mc_StartRead(u32 port, u32 slot, char *path, void *buffer, int size);

/* Starts formatting the card. */
int Mc_StartFormat(u32 port, u32 slot);

/* Starts changing the card's current directory to path; the old one goes to pwd. */
int Mc_StartChdir(u32 port, u32 slot, char *path, char *pwd);

/* Starts making the directory path. */
int Mc_StartMkdir(u32 port, u32 slot, char *path);

/* Starts looking for the file path: the status becomes MC_STATUS_FOUND or MC_STATUS_NOT_FOUND. */
int Mc_StartFindFile(u32 port, u32 slot, char *path);

/* Once a frame: steps the request in progress. Returns 0 when nothing runs or the request has just finished,
   the request number while it runs, and minus the request number when it failed. */
int Mc_Update(void);

/* Sets the status from the last check of a card. Returns -1, -2, -5 or -6 when the card cannot be used, else
   minus the check's result (0 the same card as before). */
int Mc_GetCardStatus(int port, int slot);

/* Starts the memory card library and forgets every card. Returns sceMcInit's result. */
int Mc_Init(void);

/* The result of the last check of a card. */
int Mc_GetCheckResult(int port, int slot);

/* The free clusters of the card less two, or -1 when no card was seen. */
int Mc_GetFreeClusters(int port, int slot);

/* The type of the card in the port and slot. */
int Mc_GetCardType(int port, int slot);

/* How many entries the last directory request found. */
s32 Mc_GetDirCount(void);

/* The status of the last request (MC_STATUS_). */
s32 Mc_GetStatus(void);

/* Returns clusters when the card has that many free (less two), -1 when it is full and -2 when it has too few. */
int Mc_CheckRoom(int port, int slot, int clusters);

#endif
