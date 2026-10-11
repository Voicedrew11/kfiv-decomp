#include "kf_load.h"

int FlushCache(int);
void func_002007C0(char *);

/* Walk every chunk of the loaded package, hand each payload to func_002007C0, then flush. */
void func_001D44A0(ChunkUpload *upload) {
    u32 i;
    u32 *header;
    char *next;
    char *chunk;

    header = ((u32 **)upload->source)[1];
    chunk = (char *)header + 0x10;
    for (i = 0; i < *header; i++) {
        next = chunk + ((int *)chunk)[3];
        func_002007C0(chunk + ((int *)chunk)[1]);
        chunk = next + ((int *)chunk)[2];
    }
    FlushCache(0);
    upload->done = 1;
    upload->stage = 0;
    upload->unk08 = -1;
}
