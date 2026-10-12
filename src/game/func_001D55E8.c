#include "kf_load.h"

extern s32 D_00446F58;
extern ChunkPoolNode D_005CE900[];
extern ChunkUpload D_005CEEE0;
extern ChunkPoolNode *D_004472B0;
extern char D_0043F858[];

int func_001FBDE8(int);
void *func_001FBDF8(int, int);
void func_001D4540(ChunkUpload *upload);
int sprintf(char *, const char *, ...);

/* Builds the free list of 16 pool nodes with 0x25800-byte buffers and sets up their owner. */
void func_001D55E8(void) {
    u32 i;

    func_001FBDE8(D_00446F58);
    D_004472B0 = D_005CE900;
    for (i = 0; i < 15; i++) {
        D_005CE900[i].next = &D_005CE900[i + 1];
        D_005CE900[i].buffer = func_001FBDF8(0x25800, 0x10);
    }
    D_005CE900[15].next = NULL;
    D_005CE900[15].buffer = func_001FBDF8(0x25800, 0x10);
    D_005CEEE0.pool_node = D_005CE900;
    D_005CEEE0.callback = func_001D4540;
    D_005CEEE0.stage = 0;
    D_005CEEE0.done = 0;
    sprintf(D_005CEEE0.name, D_0043F858);
}
