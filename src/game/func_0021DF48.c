#include "kf_movie.h"
#include <libmpeg.h>

/* Registers the callback that receives one elementary stream of the movie. */
int func_0021DF48(MovieVideoDecoder *decoder, int type, int channel, sceMpegCallback callback, void *data) {
    sceMpegAddStrCallback((sceMpeg *)decoder, type, channel, callback, data);
    return 1;
}
