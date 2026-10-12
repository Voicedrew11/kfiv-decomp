#include "kf_movie.h"
#include <libmpeg.h>

extern MovieStreamBuffer D_00437980;

int func_0021E500(void);
void func_0021B9E0(void);
int func_0021D208(MovieStreamBuffer *);

/* libmpeg callback 1 (the decoder ran out of data): read more, yield, queue what the stream buffer holds for DMA. */
int func_0021E2D8(sceMpeg *mpeg, void *callback_data, void *user_data) {
    func_0021E500();
    func_0021B9E0();
    func_0021D208(&D_00437980);
    return 1;
}
