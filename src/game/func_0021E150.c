#include "kf_movie.h"
#include <libmpeg.h>

int func_0021E028(MovieVideoDecoder *);

/* Whether the decoder has used up everything it was given. */
int func_0021E150(MovieVideoDecoder *decoder) {
    return func_0021E028(decoder) == 0 && sceMpegIsRefBuffEmpty((sceMpeg *)decoder);
}
