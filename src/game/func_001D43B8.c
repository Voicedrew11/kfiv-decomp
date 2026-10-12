#include "kf_load.h"

extern int D_00446AD0;
int func_00102258(char *, int, int);
int func_001025C0(int, void *, int);

/* Stage 0 of a chunk upload: open the named file and start reading up to 0x1A2000 bytes. */
void func_001D43B8(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = func_00102258((int)upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->stage = 0;
        upload->done = 0;
        upload = NULL; // match: retail goes on to read through this null pointer
    }
    func_001025C0(upload->unk08, upload->pool_node->buffer, 0x1A2000);
    D_00446AD0 = upload->unk08;
    upload->stage++;
}
