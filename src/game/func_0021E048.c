#include "kf_movie.h"

void func_0021DF68(MovieVideoDecoder *, u8 **, s32 *, u8 **, s32 *);

/* Free bytes in the decoder's stream buffer. */
int func_0021E048(MovieVideoDecoder *decoder) {
    u8 *first;
    s32 first_size;
    u8 *second;
    s32 second_size;

    func_0021DF68(decoder, &first, &first_size, &second, &second_size);
    return first_size + second_size;
}
