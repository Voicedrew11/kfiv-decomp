#include "kf_model.h"

void func_0011FC18(void *);
void func_0011EFC8(MapObject *);

/* Update handler: func_0011FC18, then the fading-out update. */
void func_0011FBE8(MapObject *object) {
    func_0011FC18(object);
    func_0011EFC8(object);
}
