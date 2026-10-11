#include "kf_texture.h"

/* Adds the deltas to two 14-bit fields packed in header->tex0; returns the data base. */
void *func_0020DB48(TextureHeader *header, int delta_a, int delta_b) {
    u64 bits = header->tex0;
    int field_a;
    int field_b;

    field_a = ((int) (bits & 0x3FFF) + delta_a) & 0x3FFF;
    bits &= ~0x3FFFUL;
    bits |= field_a;
    field_b = (int) ((bits >> 37) & 0x3FFF);
    bits &= 0xFFF8001FFFFFFFFFUL;
    bits |= (u64) ((field_b + delta_b) & 0x3FFF) << 37;
    header->tex0 = bits;
    return (char *) header + header->data_offset;
}
