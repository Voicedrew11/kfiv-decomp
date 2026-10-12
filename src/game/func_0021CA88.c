#include "common.h"

int func_0021CBE8(void *, void *, int);

/* Sends two source stretches to two stretches of IOP memory, as one stream, dropping what does
   not fit; returns the bytes sent (the SDK sample's sendToIOP2area). */
int func_0021CA88(u8 *first_dest, int first_dest_size, u8 *second_dest, int second_dest_size, u8 *first_src,
                  int first_src_size, u8 *second_src, int second_src_size) {
    if (first_dest_size + second_dest_size < first_src_size + second_src_size) {
        int excess = (first_src_size + second_src_size) - (first_dest_size + second_dest_size);

        if (excess >= second_src_size) {
            first_src_size -= excess - second_src_size;
            second_src_size = 0;
        } else {
            second_src_size -= excess;
        }
    }
    if (first_src_size >= first_dest_size) {
        func_0021CBE8(first_dest, first_src, first_dest_size);
        func_0021CBE8(second_dest, first_src + first_dest_size, first_src_size - first_dest_size);
        func_0021CBE8(second_dest + first_src_size - first_dest_size, second_src, second_src_size);
    } else if (second_src_size >= first_dest_size - first_src_size) {
        func_0021CBE8(first_dest, first_src, first_src_size);
        func_0021CBE8(first_dest + first_src_size, second_src, first_dest_size - first_src_size);
        func_0021CBE8(second_dest, second_src + first_dest_size - first_src_size,
                      second_src_size - (first_dest_size - first_src_size));
    } else {
        func_0021CBE8(first_dest, first_src, first_src_size);
        func_0021CBE8(first_dest + first_src_size, second_src, second_src_size);
    }
    return first_src_size + second_src_size;
}
