#include "common.h"
#include "kf_load.h"
#include "kf_file.h"

extern s32 func_001CBD20(void);

s32 func_0019F788(LoadRequest *request) {
    s32 file_handle;

    if (func_001CBD20() == 0) {
        return -1;
    }
    file_handle = func_00102238(request->name, 0, 1);
    func_001025C0(file_handle, request->target->buffer, -1);
    func_001024B0(file_handle);
    return 1;
}
