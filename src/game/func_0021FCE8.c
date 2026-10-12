#include "kf_movie.h"

extern char D_004420F8[];
extern char D_00442118[];

int sceCdStStop(void);
int sceSifFreeIopHeap(int);
int sceClose(int);
int printf(char *, ...);

/* Closes the movie file: stops the disc stream and frees its buffer, or closes the host file. */
s32 func_0021FCE8(void *movie_file) {
    MovieFile *file = movie_file;

    if (file->is_cd_stream) {
        while (!sceCdStStop()) {
        }
        if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
            printf(D_004420F8, D_00442118, 0xEB0);
        }
    } else {
        sceClose(file->file_fd);
    }
    return 1;
}
