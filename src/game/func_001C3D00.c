#include "kf_model.h"

u32 func_001124E0(int, u32, u32);
int func_001C2D08(void *);
extern void func_0020DF38(void *, void *, void *, int, float);

void func_001C3D00(CollisionOwner *owner, int mode, float factor) {
    ModelStripList *list;
    Strip *strip;
    StripVertex *vertex;
    int i, j;

    if (owner->flags & 0x10000) {
        list = owner->model->strips;
        if (list != NULL && list->unk0C >= 4U) {
            func_001C2D08(owner->model);
            strip = list->strips;
            i = list->strip_count;
            do {
                vertex = strip->vertices;
                j = strip->vertex_count - 2;
                do {
                    func_0020DF38(vertex[0].unk00, vertex[1].unk00, vertex[2].unk00, mode, factor);
                    if (*(int *)0x70000010 != 0) {
                        *(int *)0x70000018 = func_001124E0(vertex->unk04, vertex->unk08, vertex->unk0C);
                        return;
                    }
                    vertex++;
                } while (--j != 0);
                strip = (Strip *)(vertex + 2);
            } while (--i != 0);
        }
    }
}
