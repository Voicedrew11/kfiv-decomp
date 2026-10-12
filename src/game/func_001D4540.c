#include "kf_load.h"

extern ChunkUploadStages D_0043F740;

/* Runs the next stage of a chunk upload until it is done. */
void func_001D4540(ChunkUpload *upload) {
    ChunkUploadStages stages = D_0043F740;

    if (upload->done == 0) {
        stages.stage[upload->stage](upload);
    }
}
