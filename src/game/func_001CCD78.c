#include "kf_load.h"

int sceSifAllocIopHeap(int);

/* Rounds the request size up to 16, then allocates the largest IOP heap block it can (halving from 1 MB). */
int func_001CCD78(IopLoadRequest *request, IopLoadContext *context) {
    u32 heap_size = 0x100000;
    IopLoadBuffer *buffer;
    s32 unk18;
    s32 aligned;

    buffer = context->buffer;
    aligned = ((u32)(request->size + 0xF) >> 4) << 4;
    request->size = aligned;
    context->size = aligned;
    buffer->size = aligned;
    unk18 = request->unk18;
    buffer->unk0C = unk18;
    context->unk0C = unk18;
    do {
        heap_size >>= 1;
    } while ((context->heap = sceSifAllocIopHeap(heap_size)) == 0);
    context->heap_size = heap_size;
    return 1;
}
