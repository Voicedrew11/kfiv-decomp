#include "kf_model.h"

int func_001CA5B0(int, int);
extern int D_00446770;
extern ModelNode D_0057E240[][64];

/* Returns the node at the current map slot for slot, or NULL when the index is invalid or the node is unused. */
void *func_001C8AF0(int slot) {
    int index = func_001CA5B0(D_00446770, slot);

    if (index >= 0 && D_0057E240[D_00446770][index].model != NULL) {
        return &D_0057E240[D_00446770][index];
    }
    return NULL;
}
