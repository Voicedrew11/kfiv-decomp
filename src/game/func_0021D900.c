#include "kf_movie.h"

int WaitSema(int);
int SignalSema(int);

/* Pads the bytes not queued yet to a whole block, so the last of the stream gets sent. */
void func_0021D900(MovieStreamBuffer *stream) {
    WaitSema(stream->sema);
    stream->read_bytes = (stream->read_bytes + 0x7FF) / 0x800 * 0x800;
    SignalSema(stream->sema);
}
