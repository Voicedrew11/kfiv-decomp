#include "kf_model.h"

/* Stores in each part's 7-bit level field how many thresholds the value passed. */
int func_00192DA8(ModelInstance *model, int table_index, int value) {
    ModelPartLevels *levels;
    ModelPart *part;
    int i, passed;
    u16 *threshold;

    /* match: indexing level_tables as an array swaps the addu operands */
    levels = *(ModelPartLevels **)((char *)model->level_tables + (table_index << 2));
    for (i = 0; i < model->part_count; i++, levels++) {
        part = &model->parts[i];
        if (levels->thresholds == NULL || levels->count == 0) {
            continue;
        }
        threshold = levels->thresholds;
        for (passed = 0; passed < levels->count; passed++, threshold += 9) {
            if (value < *threshold) {
                break;
            }
        }
        if (passed == levels->count) {
            part->level = 0;
        } else if (passed != 0) {
            part->level = passed - 1;
        }
    }
    return 1;
}
