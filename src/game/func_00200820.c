#include "kf_game.h"

/* Turns the file offsets inside a loaded file into pointers. */
s32 func_00200820(RelocFile *file) {
    RelocEntry *entry;
    u32 i;
    s32 j;
    RelocEntry *cursor = file->entries;
    RelocLayer *layer;

    for (i = 0; i < file->entry_count; i++) {
        entry = cursor;
        if (entry->unk30 != NULL) {
            entry->unk30 = (char *)file + (s32)entry->unk30;
        }
        if (entry->layers != NULL) {
            entry->layers = (RelocLayer *)((char *)file + (s32)entry->layers);
        }
        if (entry->unk38 != NULL) {
            entry->unk38 = (char *)file + (s32)entry->unk38;
        }
        if (entry->unk3C != NULL) {
            entry->unk3C = (char *)file + (s32)entry->unk3C;
        }
        for (j = 0, layer = entry->layers; j < entry->layer_count; j++, layer++) {
            if (layer->unk08 != NULL) {
                layer->unk08 = (char *)file + (s32)layer->unk08;
            }
            if (layer->unk0C != NULL) {
                layer->unk0C = (char *)file + (s32)layer->unk0C;
            }
        }
        cursor = entry + 1;
    }
    return 0;
}
