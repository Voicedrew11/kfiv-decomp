#include "kf_file.h"

int KF_File_Open(char *name, int nowait, int pack) {
    sceCdlFILE file;
    char path[256];
    KFFile *f;
    KFDirEntry *e;
    unsigned int i;
    int slot;
    char *p;

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
    f = &D_00447600[slot];
    if (pack == 1) {
        strcpy(path, name);
        for (p = path; *p != 0; p++) {
            if (*p == '\\') {
                *p = '/';
            }
        }
        e = (KFDirEntry *)D_00447000[2];
        for (i = 0; i < ((KFDirHeader *)D_00447000[1])->count; i++, e++) {
            if (strcasecmp(path, e->name) == 0) {
                break;
            }
        }
        if (i >= ((KFDirHeader *)D_00447000[1])->count) {
            return -1;
        }
        f->nowait = nowait;
        f->active = 1;
        f->start = f->cur = ((KFDirHeader *)D_00447000[1])->lsn + (e->offset >> 11);
        f->size = f->remain = e->size;
    } else {
        strcpy(path, "\\DATA\\");
        strcat(path, name);
        strcat(path, ";1");
        p = path;
        do {
            if (*p == '/') {
                *p = '\\';
            }
        } while (*p++ != 0);
        memset(&file, 0, sizeof(file));
        while (sceCdSearchFile(&file, path) == 0) {
        }
        f->active = 1;
        f->start = f->cur = file.lsn;
        f->size = f->remain = 0;
    }
    return slot;
}
