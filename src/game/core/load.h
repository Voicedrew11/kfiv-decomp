/* Load jobs: the step-by-step loaders that open a file, read it into a
   buffer and close it, advancing the step counter bgm_load_step. */
#ifndef KF_LOAD_H
#define KF_LOAD_H

#include "common.h"

/* a file load job (Sound_BgmReadBody, Sound_BgmCloseBody, Sound_BgmReadSequence, Sound_ReadJobFile) */
typedef struct {
    int *buffers;                   /* 0x00 [0]/[2]: where the data goes */
    char *name;                     /* 0x04 file name, or a sprintf format of the index */
    char *header_format;            /* 0x08 sprintf format of the index (Sound_BgmReadHeader) */
    char *name_format;              /* 0x0C sprintf format of the index */
    int voice_index;                /* 0x10 the voice the load fills */
    int sequence_index;             /* 0x14 the sequence the load starts */
    char file_size_area[0x0C]; /* 12 bytes; holds a file size at +4 for BGM */
    int file_handle;                /* 0x24 -1 when closed */
} LoadJob;

/* the job Sound_BgmFadeOut and Sound_BgmLoadVoice step: control words, the IOP handle it got and its arguments */
typedef struct {
    int *ctl;                       /* 0x00 */
    char pad04[0x0C];
    s32 result;                     /* 0x10 handle, negative until loaded */
    s32 sequence_index;             /* 0x14 the sequence bank the load started */
    s32 load_index;                 /* 0x18 the voice or sequence bank the load fills */
    s32 spu_dest;                   /* 0x1C where the samples go in sound memory */
} IopLoadJob;

/* LoadRequest.target (Sound_ReadRequestFile) */
typedef struct {
    s32 pad00;                      /* 0x00 */
    void *buffer;                   /* 0x04 where the file is read to */
} LoadBuffer;

/* a request to read a named file into a buffer (Sound_ReadRequestFile) */
typedef struct {
    LoadBuffer *target;             /* 0x00 */
    char pad04[0x04];
    char *name;                     /* 0x08 */
} LoadRequest;

/* a node of the free list of upload buffers (load_buffer_nodes[16], Menu_InitLoadBuffers) */
typedef struct ChunkPoolNode {
    struct ChunkPoolNode *next;     /* 0x00 */
    void *buffer;                   /* 0x04 the loaded package: word 0 is its chunk count */
} ChunkPoolNode;

/* a staged chunk upload (Menu_StepWornModelLoad, Menu_FinishPackageLoad) */
typedef struct ChunkUpload {
    char pad00[0x08];
    int file_handle;                /* 0x08 the file being read; -1 when done */
    ChunkPoolNode *pool_node;       /* 0x0C the node whose buffer holds the chunk list */
    char pad10[0x08];
    void (*callback)(struct ChunkUpload *upload); /* 0x18 runs the next stage */
    int stage;                      /* 0x1C index into the stage handlers */
    int done;                       /* 0x20 */
    char pad24[0x04];
    char name[0x28];                /* 0x28 */
} ChunkUpload;

/* the three stage handlers of a ChunkUpload (model_load_stages, stream_load_stages, package_load_stages in
   ui/menu_screens.c), copied to the stack before the call */
typedef struct {
    void (*stage[3])(void *upload);
} ChunkUploadStages;

/* IopLoadContext.buffer */
typedef struct {
    char pad00[0x0C];
    s32 spu_dest;                   /* 0x0C Where the samples go in sound memory */
    char pad10[0x04];
    s32 size;                       /* 0x14 */
} IopLoadBuffer;

/* the request SoundDriver_VoiceLoadAllocBuffer reads: the data size and a second value */
typedef struct {
    char pad00[0x10];
    s32 size;                       /* 0x10 rounded up to 16 */
    char pad14[0x04];
    s32 spu_dest;                   /* 0x18 Where the samples go in sound memory */
} IopLoadRequest;

/* an IOP heap block load: the sizes it settled on and the heap it got */
typedef struct {
    IopLoadBuffer *buffer;          /* 0x00 */
    char pad04[0x04];
    s32 size;                       /* 0x08 */
    s32 spu_dest;                   /* 0x0C Where the samples go in sound memory */
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

/* a streaming read slot (Menu_OpenWornModelFile) */
typedef struct {
    char pad00[0x08];
    s32 file_handle;                /* 0x08 */
    StreamBuf *stream;              /* 0x0C */
    SectorChain *chain;             /* 0x10 */
    char pad14[0x08];
    s32 sector_count;               /* 0x1C */
    s32 done;                       /* 0x20 */
} StreamSlot;

/* menu_file_blocks[menu_file_block_count]: one per block of the loaded menu file (MenuFile_IndexBlocks fills them in) */
typedef struct {
    char pad00[0x10];
    void *data;                     /* 0x10 the block, handed to MenuTexture_Upload */
    char pad14[0x04];
} LoadedBlockRef;

/* the header of one chunk in a loaded package (ChunkPoolNode.buffer + 0x10 is the first; see Menu_FinishPackageLoad) */
typedef struct {
    char pad00[0x04];
    s32 payload_offset;             /* 0x04 from the header to the chunk's data */
    s32 chunk_data_size;            /* 0x08 Bytes after the header, added to the stride */
    s32 chunk_padding_size;         /* 0x0C Bytes after the header, added to the stride */
} PackageChunk;

/* a loaded key item model (the buffer of a key_item_model_loads[] upload, Menu_UpdateKeyItems) */
typedef struct {
    char pad00[0x04];
    s32 part_offset;                /* 0x04 from 0x10 bytes into the file to its PoolPart */
    char pad08[0x04];
    s32 texture_offset;             /* 0x0C from the file's start to its TextureHeader */
} KeyItemModelFile;

/* core/load_buffer.c */
extern char sound_heap_base[];

#endif
