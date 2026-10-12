#include "common.h"

int sceOpen(const char *, int);
int sceLseek(int, int, int);
int sceRead(int, void *, int);
int sceClose(int);
int FlushCache(int);

/* Reads a whole file into buffer; returns buffer, or NULL when the file does not open. */
void *func_00204030(const char *name, void *buffer) {
    s32 file_handle;
    s32 end_offset;

    file_handle = sceOpen(name, 1);
    if (file_handle < 0) {
        return NULL;
    }
    end_offset = sceLseek(file_handle, 0, 2);
    sceRead(file_handle, buffer, end_offset - sceLseek(file_handle, 0, 0));
    sceClose(file_handle);
    FlushCache(0);
    return buffer;
}
