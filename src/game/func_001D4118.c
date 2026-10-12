#include "kf_file.h"
#include "kf_load.h"

extern int D_00446AD0;
extern int D_004472F0;
int func_00102258(int, int, int);

void func_001D4118(StreamSlot *slot) {
    s32 file_handle;

    file_handle = func_00102258((s32)slot->chain->data, 1, 1);
    slot->file_handle = file_handle;
    if (file_handle < 0) {
        SectorChain *next;

        slot->file_handle = -1;
        slot->sector_count = 0;
        next = slot->chain->next;
        D_004472F0 = 0;
        if (next != NULL) {
            slot->chain = next;
            return;
        }
        slot->done = 1;
        return;
    }
    func_001025C0(file_handle, slot->stream->buf, 0x25800);
    D_00446AD0 = slot->file_handle;
    slot->sector_count += 1;
}
