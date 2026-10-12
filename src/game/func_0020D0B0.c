#include "kf_texture.h"

void func_0020D0E8(TextureHeader *);

/* Uploads a texture chunk when asked to; returns the address after it. */
char *func_0020D0B0(char *chunk, int upload) {
    TextureHeader *header = (TextureHeader *)chunk;

    if (upload) {
        func_0020D0E8(header);
    }
    return chunk + header->data_offset;
}
