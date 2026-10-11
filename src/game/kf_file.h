/* The game's file layer: up to two open files, read from the disc either
   whole (\DATA\<name>;1 through the ISO 9660 directory) or as members of
   KF4.DAT, whose table of contents is loaded once at start-up. */
#ifndef KF_FILE_H
#define KF_FILE_H

typedef struct {
    unsigned char trycount;
    unsigned char spindlctrl;
    unsigned char datapattern;
    unsigned char pad;
} sceCdRMode;

typedef struct {
    unsigned int lsn;
    unsigned int size;
    char name[16];
    unsigned char date[8];
    unsigned int flag;
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
    unsigned int size;
    int unk38;
    unsigned int offset;    /* bytes from the start of KF4.DAT */
} KFDirEntry;

typedef struct {
    int unk0;
    unsigned int count;
    int lsn;                /* first sector of KF4.DAT */
    char pad[0x34];
} KFDirHeader;

extern int D_00446710;          /* file layer locked */
extern int D_00447000[2];       /* [1] the KF4.DAT header, [2] its entries */
extern KFFile D_00447600[2];
extern char D_00447680[];       /* the KF4.DAT table of contents */

int KF_File_Open(char *name, int nowait, int pack);
int func_001024B0(int fd);
int func_001025C0(int fd, void *buf, int size);
int func_00102790(int fd, int mode);
int func_00102238(char *name, int nowait, int pack);

int printf(const char *, ...);
int puts(const char *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
int strcasecmp(const char *, const char *);
void *memset(void *, int, unsigned int);
int sceCdSearchFile(sceCdlFILE *, const char *);
int sceCdRead(int lsn, int sectors, void *buf, sceCdRMode *mode);
int sceCdSeek(int lsn);
int sceCdSync(int mode);
int sceCdGetError(void);

#endif
