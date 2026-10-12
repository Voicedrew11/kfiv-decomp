#include "common.h"

void *memcpy(void *, const void *, u32);

/* Copies two source stretches into two destination stretches, as one stream; returns the bytes
   copied, or 0 when they do not fit (the SDK sample's copy2area). */
int func_0021BFD8(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size, u8 *first_src,
                  int first_src_size, u8 *second_src, int second_src_size) {
    if (first_dest_size + second_dest_size < first_src_size + second_src_size) {
        return 0;
    }
    if (first_src_size >= first_dest_size) {
        memcpy(first_dest, first_src, first_dest_size);
        memcpy(second_dest, first_src + first_dest_size, first_src_size - first_dest_size);
        memcpy(second_dest + first_src_size - first_dest_size, second_src, second_src_size);
    } else if (second_src_size >= first_dest_size - first_src_size) {
        memcpy(first_dest, first_src, first_src_size);
        memcpy(first_dest + first_src_size, second_src, first_dest_size - first_src_size);
        memcpy(second_dest, second_src + first_dest_size - first_src_size,
               second_src_size - (first_dest_size - first_src_size));
    } else {
        memcpy(first_dest, first_src, first_src_size);
        memcpy(first_dest + first_src_size, second_src, second_src_size);
    }
    return first_src_size + second_src_size;
}
