#include "kf_movie.h"
#include <libmpeg.h>

int func_0021D818(int *);

/* Shuts the video decoder down: its stream buffer, then the libmpeg handle. */
int func_0021DF88(MovieVideoDecoder *decoder) {
    func_0021D818((int *)&decoder->stream);
    sceMpegDelete((sceMpeg *)decoder);
    return 1;
}
