#include "common.h"
#include "kf_sound.h"

int func_00102238(int, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
int func_001CD0E0(int, int, int, int, int, int, int);

/* Loads two files into the spec's buffers and starts the effect; 0 if either file will not open. */
int func_001A1360(char *name_a, char *name_b, SoundJobSpec *spec, int first_arg, int second_arg) {
    int handle;

    handle = func_00102238((int)name_a, 0, 1);
    if (handle < 0) {
        func_001024B0(handle);
        return 0;
    }
    func_001025C0(handle, spec->buffer_a, spec->size_a);
    func_001024B0(handle);
    handle = func_00102238((int)name_b, 0, 1);
    if (handle < 0) {
        func_001024B0(handle);
        return 0;
    }
    func_001025C0(handle, spec->buffer_b, spec->size_b);
    func_001024B0(handle);
    return func_001CD0E0(first_arg, (int)spec->buffer_b, spec->size_b, (int)spec->buffer_a, spec->size_a, 0, second_arg);
}
