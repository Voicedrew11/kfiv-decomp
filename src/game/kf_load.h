/* Load jobs: the step-by-step loaders that open a file, read it into a
   buffer and close it, advancing the step counter D_00446858. */
#ifndef KF_LOAD_H
#define KF_LOAD_H

#include "common.h"

/* a file load job (func_0019EBB8, func_0019EDA8, func_0019EF68, func_0019F6B8) */
typedef struct {
    int *buffers;                   /* 0x00 [0]/[2]: where the data goes */
    char *name;                     /* 0x04 file name, or a sprintf format of the index */
    char pad08[0x04];
    char *name_format;              /* 0x0C sprintf format of the index */
    int unk10;                      /* 0x10 */
    int unk14;                      /* 0x14 */
    char pad18[0x0C];
    int file_handle;                /* 0x24 -1 when closed */
} LoadJob;

/* a staged chunk upload (func_001D4360, func_001D44A0) */
typedef struct {
    char pad00[0x08];
    int unk08;                      /* 0x08 set to -1 when done */
    char *source;                   /* 0x0C word 1 points at the chunk list */
    char pad10[0x0C];
    int stage;                      /* 0x1C index into the stage handlers */
    int done;                       /* 0x20 */
} ChunkUpload;

/* D_0043F730: the three stage handlers, copied to the stack before the call */
typedef struct {
    void (*stage[3])(void *upload);
} ChunkUploadStages;

#endif
