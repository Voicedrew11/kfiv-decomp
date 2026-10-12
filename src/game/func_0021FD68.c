#include "kf_file.h"
#include "kf_movie.h"

extern char D_004422B0[];
extern char D_004422D8[];
extern char D_004422E8[];
extern char D_00442300[];
extern char D_00442318[];
extern char D_00442328[];

int sceCdStRead(u32, u32 *, u32, u32 *);
int sceCdStStop(void);
int sceCdStStart(u32, sceCdRMode *);
int sceCdStSeek(u32);
int sceCdDiskReady(int);
int sceRead(int, void *, int);

/* Reads the next stretch of the movie file; a disc stream that reports an error is restarted or sought back to lsn. */
s32 func_0021FD68(MovieFile *file, u32 *buffer, s32 size, s32 lsn) {
    sceCdRMode mode;
    u32 error;
    s32 count;

    if (file->is_cd_stream) {
        do {
            count = sceCdStRead(size >> 11, buffer, 1, &error);
            if (error == 0) {
                break;
            }
            printf(D_004422B0, count, error);
            if (error == 1) {
                printf(D_004422D8);
                while (!sceCdStStop()) {
                }
                mode.trycount = 0;
                mode.spindlctrl = 0;
                mode.datapattern = 0;
                while (!sceCdStStart(lsn, &mode)) {
                }
            } else {
                printf(D_004422E8);
                sceCdDiskReady(0);
                printf(D_00442300, lsn);
                while (!sceCdStSeek(lsn)) {
                }
                printf(D_00442318);
            }
        } while (error != 0);
        count <<= 11;
    } else {
        do {
            count = sceRead(file->file_fd, buffer, size);
            if (count < 0) {
                printf(D_00442328, count);
            }
        } while (count < 0);
    }
    return count;
}
