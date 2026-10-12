#include "kf_model.h"

extern ModelNode *D_004469D8;
extern int D_004471A4;
extern void (*D_004147E0[])(void);

/* Runs the current step of the pickup in progress, if there is one. */
void func_001C9C00(void) {
    if (D_004469D8 != NULL) {
        D_004147E0[D_004471A4]();
    }
}
