/* The KF4.DAT file layer: one packed archive read through two file slots, or loose files under \DATA\. */
#include "kf_file.h"

/* loads the KF4.DAT table of contents */
void func_00102178(void) {
    int file;

    memset(D_00447600, 0, sizeof(D_00447600));
    D_00447000[1] = (int)D_00447680;
    D_00447000[2] = (int)(D_00447680 + 0x40);
    file = func_00102238("KF4.DAT", 0, 0);
    func_001025C0(file, D_00447680, 0x20000);
    ((KFDirHeader *)D_00447000[1])->lsn = D_00447600[file].start;
    func_001024B0(file);
}

void func_00102218(void) {
    D_00446710 = 1;
}

void func_00102228(void) {
    D_00446710 = 0;
}

int func_00102230(void) {
    return D_00446710;
}

int func_00102238(char *name, int nowait, int pack) {
    return KF_File_Open(name, nowait, pack, 1);
}

int func_00102258(char *name, int nowait, int pack) {
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

    if (D_00446710 != 0) {
        printf("KF_File_Open() File open error!! [lock]\n");
        return -1;
    }
    for (slot = 0; slot < 2; slot++) {
        if (D_00447600[slot].active == 0) {
            break;
        }
    }
    if (slot >= 2) {
        return -1;
    }
    opened = &D_00447600[slot];
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

/* closes a file */
int func_001024B0(int file) {
    if (file == -1) {
        puts("-1");
    }
    if ((u32)file >= 2 || D_00447600[file].active == 0) {
        return -1;
    }
    func_00102790(file, 0);
    D_00447600[file].active = 0;
    D_00447600[file].start = 0;
    D_00447600[file].cur = 0;
    D_00447600[file].size = 0;
    D_00447600[file].remain = 0;
    return 1;
}

/* closes every open file and unlocks the file layer */
void func_00102558(void) {
    int i;
    KFFile *opened = D_00447600;

    for (i = 0; i < 2; i++, opened++) {
        if (opened->active) {
            func_00102790(i, 0);
            func_001024B0(i);
        }
    }
    D_00446710 = 0;
}

/* reads from a file */
int func_001025C0(int file, void *buf, int size) {
    sceCdRMode mode;
    KFFile *opened;
    int sectors;

    if ((u32)file >= 2 || D_00447600[file].active == 0) {
        return -1;
    }
    if (size < 0) {
        size = 0x1000000;
    }
    func_00102790(file, 0);
    opened = &D_00447600[file];
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
        func_00102790(file, 0);
    }
    if (opened->size != 0) {
        opened->remain -= size;
    }
    return size;
}

/* seeks the drive to a file's next sector */
int func_00102728(int file) {
    if ((u32)file >= 2 || D_00447600[file].active == 0) {
        return -1;
    }
    func_00102790(file, 0);
    return sceCdSeek(D_00447600[file].cur);
}

/* waits for the read in flight, reissuing it while the drive reports an error */
int func_00102790(int file, int mode) {
    sceCdRMode retry_mode;
    u32 error;
    int result;

    if ((u32)file >= 2 || D_00447600[file].active == 0) {
        return -1;
    }
    result = sceCdSync(mode);
    if (result == 0) {
        error = sceCdGetError();
        if (D_00447600[file].pending != 0) {
            while (error >= 2) {
                retry_mode = D_00447600[file].pend_mode;
                while (sceCdRead(D_00447600[file].pend_lsn, D_00447600[file].pend_sectors,
                                 D_00447600[file].pend_buf, &retry_mode) == 0) {
                }
                result = sceCdSync(0);
                error = sceCdGetError();
            }
            D_00447600[file].pending = 0;
            D_00447600[file].pend_lsn = 0;
            D_00447600[file].pend_sectors = 0;
            D_00447600[file].pend_buf = 0;
            memset(&D_00447600[file].pend_mode, 0, sizeof(sceCdRMode));
        }
    }
    return result;
}

/* the size of a file */
int func_00102900(int file) {
    if ((u32)file >= 2) {
        return -1;
    }
    return D_00447600[file].size;
}
