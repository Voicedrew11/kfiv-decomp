#include "kf_movie.h"

int func_0021D8B0(int *);

/* Bytes of stream data the decoder has not consumed yet. */
int func_0021E028(MovieVideoDecoder *decoder) {
    return func_0021D8B0((int *)&decoder->stream);
}
