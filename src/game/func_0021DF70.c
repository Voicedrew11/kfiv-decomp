#include "kf_movie.h"

void func_0021D1B8(void *, int);

/* Tells the decoder's stream buffer how many bytes were written to it. */
void func_0021DF70(MovieVideoDecoder *decoder, s32 size) {
    func_0021D1B8(&decoder->stream, size);
}
