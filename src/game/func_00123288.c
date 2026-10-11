#include "kf_entity.h"

void func_001232C0(void);
void func_00123690(void);
void func_001234F0(void);
void func_00124570(void);

/* Per-frame entity update: runs the pool, list and despawn steps in order. */
void func_00123288(void) {
    func_001232C0();
    func_001234F0();
    KF_Entity_DespawnOutOfRange();
    func_00123690();
    func_00124570();
}
