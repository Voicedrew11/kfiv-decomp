/* The game's file layer: up to two open files, read from the disc either
   whole (\DATA\<name>;1 through the ISO 9660 directory) or as members of
   KF4.DAT, whose table of contents is loaded once at start-up. */
#ifndef KF_FILE_H
#define KF_FILE_H

#include "common.h"
#include <libcdvd.h>

/* the file record sceCdSearchFile fills in (libcdvd.h leaves its body to the caller) */
typedef struct sceCdlFILE {
    u32 lsn;
    u32 size;
    char name[16];
    u8 date[8];
    u32 flag;
} sceCdlFILE;

/* one open file */
typedef struct {
    int active;
    int nowait;     /* nonzero: reads return before the data is in */
    int start;      /* first sector */
    int cur;        /* next sector to read */
    int size;       /* bytes, 0 if unknown (opened from the ISO directory) */
    int remain;     /* bytes not yet read */
    /* the read in flight, retried by the sync if the drive reports an error */
    int pending;
    int pend_lsn;
    int pend_sectors;
    void *pend_buf;
    sceCdRMode pend_mode;
} KFFile;

/* a KF4.DAT member */
typedef struct {
    char name[0x34];
    u32 size;
    int pad38;
    u32 offset;    /* bytes from the start of KF4.DAT */
} KFDirEntry;

typedef struct {
    int pad0;
    u32 count;
    int lsn;                /* first sector of KF4.DAT */
    char pad[0x34];
} KFDirHeader;

extern int file_layer_locked;   /* file layer locked */
extern int kf4_directory_words[2];       /* [1] the KF4.DAT header, [2] its entries, [3] the fade direction (core/game.c); 8 bytes keeps it $gp-relative */
extern KFFile file_slots[2];
extern char archive_toc[];
extern char file_load_buffer[0x100000];  /* where whole files are read to (core/load_buffer.c) */       /* the KF4.DAT table of contents */

int KF_File_Open(char *name, int nowait, int pack, int unused);
int File_Close(int fd);
int File_Read(int fd, void *buf, int size);
int File_Sync(int fd, int mode);
int File_Open(char *name, int nowait, int pack);

int printf(const char *, ...);
int puts(const char *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
int strcasecmp(const char *, const char *);
void *memset(void *, int, u32);

/* core/file.c */
void File_InitDrive(void);
void File_LoadArchiveToc(void);
void File_Lock(void);
void File_Unlock(void);
int File_IsLocked(void);
int File_OpenUnflagged(char *name, int nowait, int pack);
void File_CloseAll(void);
int File_GetSize(int file);

#endif
