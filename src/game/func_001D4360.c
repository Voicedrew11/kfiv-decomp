#include "kf_load.h"

extern char D_0043F730[];

/* Call the stage handler selected by upload->stage unless upload->done is set. */
void func_001D4360(ChunkUpload *upload) {
    ChunkUploadStages handlers = *(ChunkUploadStages *)D_0043F730;

    if (upload->done == 0) {
        handlers.stage[upload->stage](upload);
    }
}
