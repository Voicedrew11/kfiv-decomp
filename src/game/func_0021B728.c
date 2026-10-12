#include "kf_movie.h"

extern MovieThreadState D_00437850;
extern MovieVideoDecoder D_00437938;

void func_0021BA20(char *);
int func_0021B778(MovieVideoDecoder *, MovieReadBuffer *, MovieFile *, int (*)(void));
void func_0021BD18(void);

/* Plays one movie file from start to end and frees what playing it needed. */
int func_0021B728(char *path, int (*skip_requested)(void)) {
    int result;

    func_0021BA20(path);
    result = func_0021B778(&D_00437938, MOVIE_READ_BUFFER(&D_00437850), (MovieFile *)&D_00437850.unk4C, skip_requested);
    func_0021BD18();
    return result;
}
