#include "kf_movie.h"

s32 func_0021E588(char *);

/* Returns the slot the next picture is decoded into, or NULL when the ring is full. */
MovieOutPicture *func_0021E610(MovieOutBuffer *buffer) {
    return func_0021E588((char *)buffer) ? NULL : buffer->data + buffer->write;
}
