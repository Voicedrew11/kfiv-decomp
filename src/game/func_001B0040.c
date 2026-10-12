#include "kf_entity.h"

/* Nonzero if the entity cannot be targeted: not live, dying, or flagged out. */
int func_001B0040(Entity *entity) {
    int excluded;

    if (!ENTITY_FLAG_BITS(entity)->bit1 || ENTITY_FLAG_BITS(entity)->bits2 > 0) {
        return TRUE;
    }
    if (ENTITY_FLAG_BITS(entity)->bit16 == 1) {
        return TRUE;
    }
    excluded = FALSE;
    if (ENTITY_FLAG_BITS(entity)->bit21) {
        excluded = TRUE;
    }
    return excluded;
}
