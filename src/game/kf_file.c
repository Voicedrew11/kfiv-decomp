/* The KF4.DAT file layer: one packed archive read through two file slots, or loose files under \DATA\. */
#include "kf_file.h"

int sceCdInit(int);
int sceCdMmode(int);

// match: kf_file.h declares these, so a plain definition would go to COMMON
static u8 D_004475C8[0x38] __attribute__((section(".bss"))) = { 0 }; /* 0x4475C8 */
KFFile file_slots[2] __attribute__((section(".bss"))) = { 0 };       /* 0x447600 the two file slots */
static u8 D_00447658[0x28] __attribute__((section(".bss"))) = { 0 }; /* 0x447658 */
char archive_toc[0x20000] __attribute__((section(".bss"))) = { 0 };   /* 0x447680 the KF4.DAT table of contents, read whole */

/* Starts the disc drive in DVD mode. */
void File_InitDrive(void) {
    while (sceCdInit(0) == 0) {
    }
    sceCdMmode(2);
}

/* Loads the KF4.DAT table of contents, read whole into archive_toc. */
void File_LoadArchiveToc(void) {
    int file;

    memset(file_slots, 0, sizeof(file_slots));
    D_00447000[1] = (int)archive_toc;
    D_00447000[2] = (int)(archive_toc + 0x40);
    file = File_Open("KF4.DAT", 0, 0);
    File_Read(file, archive_toc, 0x20000);
    ((KFDirHeader *)D_00447000[1])->lsn = file_slots[file].start;
    File_Close(file);
}

/* Locks the file layer while a background read is in flight: KF_File_Open refuses to open anything. */
void File_Lock(void) {
    file_layer_locked = 1;
}

/* Unlocks the file layer. */
void File_Unlock(void) {
    file_layer_locked = 0;
}

/* Whether the file layer is locked (File_Lock). */
int File_IsLocked(void) {
    return file_layer_locked;
}

/* Opens a file (KF_File_Open with 1 as its ignored fourth argument); returns the slot or -1. */
int File_Open(char *name, int nowait, int pack) {
    return KF_File_Open(name, nowait, pack, 1);
}

/* Opens a file like File_Open, with 0 as KF_File_Open's ignored fourth argument; returns the slot or -1. */
int File_OpenUnflagged(char *name, int nowait, int pack) {
    return KF_File_Open(name, nowait, pack, 0);
}

int KF_File_Open(char *name, int nowait, int pack, int unused) {
    sceCdlFILE file;
    char path[256];
    KFFile *opened;
    KFDirEntry *entry;
    u32 i;
    int slot;
    char *cursor;

    if (file_layer_locked != 0) {
        printf("KF_File_Open() File open error!! [lock]\n");
        return -1;
    }
    for (slot = 0; slot < 2; slot++) {
        if (file_slots[slot].active == 0) {
            break;
        }
    }
    if (slot >= 2) {
        return -1;
    }
    opened = &file_slots[slot];
    if (pack == 1) {
        strcpy(path, name);
        for (cursor = path; *cursor != 0; cursor++) {
            if (*cursor == '\\') {
                *cursor = '/';
            }
        }
        entry = (KFDirEntry *)D_00447000[2];
        for (i = 0; i < ((KFDirHeader *)D_00447000[1])->count; i++, entry++) {
            if (strcasecmp(path, entry->name) == 0) {
                break;
            }
        }
        if (i >= ((KFDirHeader *)D_00447000[1])->count) {
            return -1;
        }
        opened->nowait = nowait;
        opened->active = 1;
        opened->start = opened->cur = ((KFDirHeader *)D_00447000[1])->lsn + (entry->offset >> 11);
        opened->size = opened->remain = entry->size;
    } else {
        strcpy(path, "\\DATA\\");
        strcat(path, name);
        strcat(path, ";1");
        cursor = path;
        do {
            if (*cursor == '/') {
                *cursor = '\\';
            }
        } while (*cursor++ != 0);
        memset(&file, 0, sizeof(file));
        while (sceCdSearchFile(&file, path) == 0) {
        }
        opened->active = 1;
        opened->start = opened->cur = file.lsn;
        opened->size = opened->remain = 0;
    }
    return slot;
}

/* Closes a file after waiting for its read; returns 1, or -1 for a slot that is not open. */
int File_Close(int file) {
    if (file == -1) {
        puts("-1");
    }
    if ((u32)file >= 2 || file_slots[file].active == 0) {
        return -1;
    }
    File_Sync(file, 0);
    file_slots[file].active = 0;
    file_slots[file].start = 0;
    file_slots[file].cur = 0;
    file_slots[file].size = 0;
    file_slots[file].remain = 0;
    return 1;
}

/* closes every open file and unlocks the file layer */
void File_CloseAll(void) {
    int i;
    KFFile *opened = file_slots;

    for (i = 0; i < 2; i++, opened++) {
        if (opened->active) {
            File_Sync(i, 0);
            File_Close(i);
        }
    }
    file_layer_locked = 0;
}

/* Reads size bytes (all of a negative size: up to 16 MB) at the file's position into buf, waiting for them
   unless the file was opened with nowait; returns the bytes read, 0 at the end, -1 for a slot that is not
   open. */
int File_Read(int file, void *buf, int size) {
    sceCdRMode mode;
    KFFile *opened;
    int sectors;

    if ((u32)file >= 2 || file_slots[file].active == 0) {
        return -1;
    }
    if (size < 0) {
        size = 0x1000000;
    }
    File_Sync(file, 0);
    opened = &file_slots[file];
    if (opened->size != 0) {
        if (opened->remain == 0) {
            return 0;
        }
        if (opened->remain < size) {
            size = opened->remain;
        }
    }
    sectors = (size + 0x7FF) / 0x800;
    memset(&mode, 0, sizeof(mode));
    mode.trycount = 0;
    mode.spindlctrl = 1;
    mode.datapattern = 0;
    mode.pad = 0;
    opened->pending = 1;
    opened->pend_lsn = opened->cur;
    opened->pend_sectors = sectors;
    opened->pend_buf = buf;
    opened->pend_mode = mode;
    while (sceCdRead(opened->cur, sectors, buf, &mode) == 0) {
    }
    opened->cur += sectors;
    if (opened->nowait == 0) {
        File_Sync(file, 0);
    }
    if (opened->size != 0) {
        opened->remain -= size;
    }
    return size;
}

/* Seeks the drive to a file's next sector; returns sceCdSeek's result, -1 for a slot that is not open. */
int File_Seek(int file) {
    if ((u32)file >= 2 || file_slots[file].active == 0) {
        return -1;
    }
    File_Sync(file, 0);
    return sceCdSeek(file_slots[file].cur);
}

/* Waits for the read in flight (mode as sceCdSync), reissuing it while the drive reports an error; returns
   sceCdSync's result. */
int File_Sync(int file, int mode) {
    sceCdRMode retry_mode;
    u32 error;
    int result;

    if ((u32)file >= 2 || file_slots[file].active == 0) {
        return -1;
    }
    result = sceCdSync(mode);
    if (result == 0) {
        error = sceCdGetError();
        if (file_slots[file].pending != 0) {
            while (error >= 2) {
                retry_mode = file_slots[file].pend_mode;
                while (sceCdRead(file_slots[file].pend_lsn, file_slots[file].pend_sectors,
                                 file_slots[file].pend_buf, &retry_mode) == 0) {
                }
                result = sceCdSync(0);
                error = sceCdGetError();
            }
            file_slots[file].pending = 0;
            file_slots[file].pend_lsn = 0;
            file_slots[file].pend_sectors = 0;
            file_slots[file].pend_buf = 0;
            memset(&file_slots[file].pend_mode, 0, sizeof(sceCdRMode));
        }
    }
    return result;
}

/* The size of a file (0 for a loose file), -1 for a bad slot. */
int File_GetSize(int file) {
    if ((u32)file >= 2) {
        return -1;
    }
    return file_slots[file].size;
}
