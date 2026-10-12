#include "kf_load.h"

extern int D_00446AD0;
int func_00102790(int, int);
int func_001024B0(int);

/* Stage 1 of a chunk upload: when the read has finished close the file and go on; on error start over. */
void func_001D4428(ChunkUpload *upload) {
    s32 status;

    status = func_00102790(upload->unk08, 1);
    if (status == 0) {
        func_001024B0(upload->unk08);
        D_00446AD0 = -1;
        upload->stage++;
    } else if (status < 0) {
        func_001024B0(upload->unk08);
        upload->done = upload->stage = 0;
        D_00446AD0 = -1;
    }
}
