#include "kf_load.h"

extern int D_00446AD0;
extern char D_0043F6F8[];
int func_00102238(char *, int, int);
int func_00102258(char *, int, int);
int func_001025C0(int, void *, int);

/* Stage 0 of a chunk upload: open the named file (or the fallback file) and start reading it. */
void func_001D3F78(ChunkUpload *upload) {
    s32 file_handle;

    file_handle = func_00102258((int)upload->name, 1, 1);
    upload->unk08 = file_handle;
    if (file_handle < 0) {
        upload->unk08 = func_00102238((int)D_0043F6F8, 1, 1);
    }
    func_001025C0(upload->unk08, upload->pool_node->buffer, 0x25800);
    D_00446AD0 = upload->unk08;
    upload->stage++;
}
