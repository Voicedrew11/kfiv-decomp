#include "kf_movie.h"

/* Sets up the ring of decoded pictures over `size` picture and tag slots, all empty. */
int func_0021E528(MovieOutBuffer *buffer, MovieOutPicture *data, MovieOutTag *tag, s32 size) {
    s32 index;

    buffer->data = data;
    buffer->tag = tag;
    buffer->size = size;
    buffer->count = 0;
    buffer->write = 0;
    for (index = 0; index < size; index++) {
        buffer->tag[index].status = 0;
    }
    return 0;
}
