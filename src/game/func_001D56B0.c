#include "common.h"

void *func_001FBD90(int);
int func_00203FC8(char *, void *);

/* Loads a file of 0x60-byte records behind a 0x10-byte header; gives back how many and where. */
void func_001D56B0(char *name, u32 *record_count, void **records) {
    u32 *file_data;

    file_data = func_001FBD90(0x2800);
    if (func_00203FC8(name, file_data) != 0) {
        *record_count = (*file_data - 0x10) / 0x60;
        if (*record_count == 0) {
            *records = NULL;
        } else {
            file_data += 4;
            *records = file_data;
        }
    }
}
