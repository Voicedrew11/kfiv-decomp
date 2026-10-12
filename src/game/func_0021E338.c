#include "kf_movie.h"
#include <libmpeg.h>

extern MovieStreamBuffer D_00437980;

int func_0021D4F8(MovieStreamBuffer *);

/* libmpeg callback 3: restart the stream DMA. */
int func_0021E338(sceMpeg *mpeg, void *callback_data, void *user_data) {
    func_0021D4F8(&D_00437980);
    return 1;
}
