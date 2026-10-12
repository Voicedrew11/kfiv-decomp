#include "kf_movie.h"

void func_0021D0D0(MovieStreamBuffer *, u8 **, s32 *, u8 **, s32 *);

/* Gives the two free stretches of the decoder's stream buffer. */
void func_0021DF68(MovieVideoDecoder *decoder, u8 **first, s32 *first_size, u8 **second, s32 *second_size) {
    func_0021D0D0(&decoder->stream, first, first_size, second, second_size);
}
