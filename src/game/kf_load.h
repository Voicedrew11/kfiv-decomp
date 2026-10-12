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

/* the job func_0019EC90 and func_0019F498 step: control words, the IOP handle it got and its arguments */
typedef struct {
    int *ctl;                       /* 0x00 */
    char pad04[0x0C];
    s32 result;                     /* 0x10 handle, negative until loaded */
    s32 unk14;                      /* 0x14 */
    s32 unk18;                      /* 0x18 */
    s32 unk1C;                      /* 0x1C */
} IopLoadJob;

/* LoadRequest.target (func_0019F788) */
typedef struct {
    s32 unk00;                      /* 0x00 */
    void *buffer;                   /* 0x04 where the file is read to */
} LoadBuffer;

/* a request to read a named file into a buffer (func_0019F788) */
typedef struct {
    LoadBuffer *target;             /* 0x00 */
    char pad04[0x04];
    char *name;                     /* 0x08 */
} LoadRequest;

/* a node of the free list of upload buffers (D_005CE900[16], func_001D55E8) */
typedef struct ChunkPoolNode {
    struct ChunkPoolNode *next;     /* 0x00 */
    void *buffer;                   /* 0x04 the loaded package: word 0 is its chunk count */
} ChunkPoolNode;

/* a staged chunk upload (func_001D4360, func_001D44A0) */
typedef struct ChunkUpload {
    char pad00[0x08];
    int unk08;                      /* 0x08 set to -1 when done */
    ChunkPoolNode *pool_node;       /* 0x0C the node whose buffer holds the chunk list */
    char pad10[0x08];
    void (*callback)(struct ChunkUpload *upload); /* 0x18 runs the next stage */
    int stage;                      /* 0x1C index into the stage handlers */
    int done;                       /* 0x20 */
    char pad24[0x04];
    char name[0x28];                /* 0x28 */
} ChunkUpload;

/* D_0043F730: the three stage handlers, copied to the stack before the call */
typedef struct {
    void (*stage[3])(void *upload);
} ChunkUploadStages;

/* IopLoadContext.buffer */
typedef struct {
    char pad00[0x0C];
    s32 unk0C;                      /* 0x0C */
    char pad10[0x04];
    s32 size;                       /* 0x14 */
} IopLoadBuffer;

/* the request func_001CCD78 reads: the data size and a second value */
typedef struct {
    char pad00[0x10];
    s32 size;                       /* 0x10 rounded up to 16 */
    char pad14[0x04];
    s32 unk18;                      /* 0x18 */
} IopLoadRequest;

/* an IOP heap block load: the sizes it settled on and the heap it got */
typedef struct {
    IopLoadBuffer *buffer;          /* 0x00 */
    char pad04[0x04];
    s32 size;                       /* 0x08 */
    s32 unk0C;                      /* 0x0C */
    s32 heap;                       /* 0x10 */
    char pad14[0x04];
    s32 heap_size;                  /* 0x18 */
} IopLoadContext;

/* StreamSlot.stream */
typedef struct {
    char pad00[0x04];
    void *buf;                      /* 0x04 read destination */
} StreamBuf;

/* a chain of sectors, each followed by its file data */
typedef struct {
    char pad00[0x14];
    void *next;                     /* 0x14 */
    char pad18[0x10];
    u8 data[0];                     /* 0x28 size unknown */
} SectorChain;

/* a streaming read slot (func_001D4118) */
typedef struct {
    char pad00[0x08];
    s32 file_handle;                /* 0x08 */
    StreamBuf *stream;              /* 0x0C */
    SectorChain *chain;             /* 0x10 */
    char pad14[0x08];
    s32 sector_count;               /* 0x1C */
    s32 done;                       /* 0x20 */
} StreamSlot;

#endif
