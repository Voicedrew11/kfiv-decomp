#include "kf_movie.h"

int WaitSema(int);
int SignalSema(int);

/* Whether the stream DMA is running. */
int func_0021D870(MovieStreamBuffer *stream) {
    s32 is_active;

    WaitSema(stream->sema);
    is_active = stream->is_active;
    SignalSema(stream->sema);
    return is_active;
}
