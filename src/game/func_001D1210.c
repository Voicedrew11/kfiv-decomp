#include "kf_game.h"
#include "kf_vu0.h"

extern EventRunner D_00420D00;

/* Moves the event runner's position and its copy from the frame before by offset (the other map slot became
   the current one). */
void func_001D1210(f32 *offset) {
    Vector_Add(D_00420D00.unk00, D_00420D00.unk00, offset);
    Vector_Add(D_00420D00.unk20, D_00420D00.unk20, offset);
}
