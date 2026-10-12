#include "kf_game.h"
#include "kf_load.h"
#include "kf_texture.h"

extern const PoolClutPages D_00440DC0;
extern u8 D_00446D81;

void FlushCache(int); // match: an int result would keep $2 busy after the call
void func_00201518(PoolPart *, s32);
void func_00204180(TextureHeader *, LoadedBlockRef *);

/* Uploads a loaded model's texture and draws its parts with func_00201518. */
void func_00201F58(ChunkUpload *upload, s32 draw_param) {
    u64 texture_pages[1] = { 0x2840 }; // match: one entry, though indexed like clut_pages (D_00446D81 is always 0)
    PoolClutPages clut_pages = D_00440DC0;
    KeyItemModelFile *file;
    TextureHeader *texture;
    PoolPartFile *part_file;
    PoolPart *part;
    u32 i;

    if (upload != NULL && upload->done != 0) {
        file = upload->pool_node->buffer;
        texture = (TextureHeader *)((char *)file + file->texture_offset);
        part_file = (PoolPartFile *)((char *)file + file->part_offset);
        TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages[D_00446D81];
        TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages.pages[D_00446D81];
        func_00204180(texture, NULL);
        FlushCache(0);
        part = part_file->parts;
        for (i = 0; i < part_file->part_count; i++) {
            part->material->tex0 = texture->tex0;
            func_00201518(part, draw_param);
            part++;
        }
    }
}
