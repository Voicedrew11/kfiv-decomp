#include "common.h"

extern int sceOpen(const char *, int);
extern int sceRead(int, void *, int);
extern int sceClose(int);
extern int FlushCache(int);

s32 func_002040C0(const char *name, void *buffer) {
    s32 file_handle;

    file_handle = sceOpen(name, 0x8001);
    if (file_handle < 0) {
        return 0;
    }
    sceRead(file_handle, buffer, 0x19000);
    sceClose(file_handle);
    FlushCache(0);
    return 1;
}
