#include "kf_model.h"

/* Rewrite the 11-bit fields of one word in every record of every part's data. */
void func_0010E6A8(ModelInstance *model, s64 clear_mask, s64 set_value) {
    ModelPart *part;
    char *record;
    char *base;
    int i;
    int j;
    int count;
    int part_count;
    u16 length;
    int old_bits;
    int set_bits;
    int clear_bits;

    set_bits = set_value;
    set_bits &= 0x7FF;
    set_bits <<= 15;
    clear_bits = clear_mask;
    clear_bits &= 0x7FF;
    clear_bits <<= 15;
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        record = (char *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            base = record;
            length = *(u16 *)base;
            record += 0x70;
            old_bits = ((int *)record)[1] & 0x03FF8000;
            ((int *)record)[1] = (old_bits & ~clear_bits) | set_bits | (((int *)record)[1] & 0xFC007FFF);
            record = base + (length + 1) * 0x10;
        }
    }
}
