#include "kf_movie.h"
#include <libmpeg.h>

extern MovieStreamBuffer D_00437980;

int func_0021D3F8(int *);

/* libmpeg callback 2: stop the stream DMA. */
int func_0021E310(sceMpeg *mpeg, void *callback_data, void *user_data) {
    func_0021D3F8((int *)&D_00437980);
    return 1;
}
