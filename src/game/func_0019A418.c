#include "kf_model.h"

u16 func_0019A418(ModelInstance *model, int index) {
    ModelPartLevels *levels;

    /* match: indexing level_tables as an array swaps the addu operands */
    levels = *(ModelPartLevels **)((char *)model->level_tables + (index << 2));
    return *(u16 *)((char *)levels - 4);
}
