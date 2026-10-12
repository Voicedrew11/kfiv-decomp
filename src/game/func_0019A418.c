#include "kf_model.h"

/* Returns the first value of the header in front of one of the model's tables. */
int func_0019A418(ModelInstance *model, int table_index) {
    ModelPartLevels **table = model->level_tables + table_index;

    return ((ModelTableHeader *)*table - 1)->unk00;
}
