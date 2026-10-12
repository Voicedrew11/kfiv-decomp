#include "common.h"
#include "kf_model.h"

/* Finds the record with the given id in a model's parts and copies it to model->found_record. */
// match: the caller passes a second argument that this function never reads
void func_001A2E58(ModelInstance *model, int unused_arg1, int record_id, int id_bits) {
    int part_total = model->part_count;
    ModelPart *part;
    char *record;
    int part_index;
    int record_index;

    model->found_record_id = 0;
    *(Uint128 *)model->found_record = 0;
    part = model->parts;
    for (part_index = 0; part_index < part_total; part_index++, part++) {
        int record_count = part->data_count;

        record = (char *)(int)part->data_addr;
        for (record_index = 0; record_index < record_count; record_index++, record += 0x10) {
            if ((((ModelPartRecord *)record)->id_word & 0x3FFF) == record_id) {
                *(UnalignedQword *)model->found_record = *(UnalignedQword *)&((ModelPartRecord *)record)->id_word;
                model->found_record_id = record_id;
                model->found_record[0] = (model->found_record[0] & 0xFFFFC000) | id_bits;
                return;
            }
            // match: the record is walked as bytes, its payload size read mid-step
            record += 0x60;
            {
                int payload_size = *(int *)record;

                record += 0x10;
                record += (payload_size & 0x7FFF) << 6;
            }
        }
    }
}
