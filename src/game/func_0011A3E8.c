#include "kf_model.h"

void *func_001C8AF0(int);
int func_001C8C80(int, int);

/* Lowers the model of node 0x37 by 400 after func_001C8C80 has set it up. */
void func_0011A3E8(int map) {
    ModelNode *node;
    Vector4 unused;

    func_001C8C80(map, 0x37);
    node = func_001C8AF0(0x37);
    if (node != NULL) {
        node->model->position.y -= 400.0f;
    }
}
