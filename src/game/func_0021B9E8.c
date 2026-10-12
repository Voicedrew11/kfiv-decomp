#include "kf_movie.h"

extern s32 D_00437820[];
extern MovieBuffers D_004379F0;

s32 func_0021C810(char *);

/* TRUE when enough sound is buffered to start, or the movie has no sound (isAudioOK). */
int func_0021B9E8(void) {
    return (D_00437820[0] != 0) ? func_0021C810((char *)&D_004379F0) : TRUE;
}
