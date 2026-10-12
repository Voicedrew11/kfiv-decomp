#include "kf_file.h"
#include "kf_movie.h"

extern char D_004420F8[];
extern char D_00442118[];
extern char D_004421D0[];
extern char D_004421D8[];
extern char D_004421E0[];
extern char D_004421E8[];
extern char D_004421F0[];
extern char D_004421F8[] __attribute__((aligned(8))); // match: "host0", copied a word and a half at a time
extern char D_00442200[];
extern char D_00442218[];
extern char D_00442230[];
extern char D_00442258[];
extern char D_00442278[];
extern char D_00442298[];

char *index(const char *, int);
char *strncpy(char *, const char *, u32);
int strcmp(const char *, const char *);
u32 strlen(const char *);
int toupper(int);
void *memcpy(void *, const void *, u32);
int sprintf(char *, char *, ...);
int sceCdDiskReady(int);
int sceSifAllocIopHeap(int);
int sceSifFreeIopHeap(int);
int sceCdStInit(u32, u32, u32);
int sceCdStStop(void);
int sceCdStStart(u32, sceCdRMode *);
int sceOpen(const char *, int);
int sceLseek(int, int, int);
int sceClose(int);

/* Opens the movie file: "cdrom0:" paths are streamed from the disc, anything else is read from its device (host0 by default). */
s32 func_0021F998(MovieFile *file, char *path) {
    char full_path[0x100];
    char device[0x40];
    sceCdRMode mode;
    char *name;
    s32 device_length;
    s32 name_length;
    s32 i;
    char *suffix;

    name = index(path, ':');
    if (name != NULL) {
        device_length = name - path;
        strncpy(device, path, device_length);
        name++;
        device[device_length] = '\0';
        if (strcmp(device, D_004421D8) == 0) {
            name_length = strlen(name);
            file->is_cd_stream = TRUE;
            for (i = 0; i < name_length; i++) {
                if (name[i] == '/') {
                    name[i] = 0x5C; /* backslash */
                }
                name[i] = toupper(name[i]);
            }
            suffix = index(path, ';') ? D_004421D0 : D_004421E0;
            sprintf(full_path, D_004421E8, name, suffix);
        } else {
            file->is_cd_stream = FALSE;
            sprintf(full_path, D_004421F0, device, name);
        }
    } else {
        memcpy(device, D_004421F8, 6);
        file->is_cd_stream = FALSE;
        sprintf(full_path, D_004421F0, device, path);
    }
    if (file->is_cd_stream) {
        sceCdDiskReady(0);
        file->iop_buffer = sceSifAllocIopHeap(0x28010);
        if (file->iop_buffer == 0) {
            printf(D_00442200);
            return 0;
        }
        if (sceCdStInit(0x50, 5, ((u32)(file->iop_buffer + 0xF) >> 4) << 4) != 1) {
            printf(D_00442218);
            if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
                printf(D_004420F8, D_00442118, 0xE5B);
            }
            return 0;
        }
        if (!sceCdSearchFile((sceCdlFILE *)&file->lsn, full_path)) {
            printf(D_00442230, full_path);
            while (!sceCdStStop()) {
            }
            if (sceSifFreeIopHeap(file->iop_buffer) < 0) {
                printf(D_004420F8, D_00442118, 0xE63);
            }
            return 0;
        }
        file->size = file->cd_size;
        mode.trycount = 0;
        mode.spindlctrl = 0;
        mode.datapattern = 0;
        while (!sceCdStStart(file->lsn, &mode)) {
        }
    } else {
        file->file_fd = sceOpen(full_path, 1);
        if (file->file_fd < 0) {
            printf(D_00442258, full_path);
            return 0;
        }
        file->size = sceLseek(file->file_fd, 0, 2);
        if (file->size < 0) {
            printf(D_00442278, full_path, file->size);
            sceClose(file->file_fd);
            return 0;
        }
        if (sceLseek(file->file_fd, 0, 0) < 0) {
            printf(D_00442298, full_path);
            sceClose(file->file_fd);
            return 0;
        }
    }
    return 1;
}
