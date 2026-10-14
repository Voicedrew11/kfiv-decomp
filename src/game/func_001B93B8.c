#include "kf_player.h"
#include "kf_vu0.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;

/* Moves both player records by offset (the other map slot became the current one): their positions and the
   heights kept with them. */
void func_001B93B8(f32 *offset) {
    Vector_Add(&D_00413F80.position, &D_00413F80.position, offset);
    Vector_Add(&D_00414280.position, &D_00414280.position, offset);
    D_00413F80.unk1D8 += offset[1];
    D_00413F80.unk1DC += offset[1];
    D_00413F80.unk214 += offset[1];
    D_00414280.unk1D8 += offset[1];
    D_00414280.unk1DC += offset[1];
    D_00414280.unk214 += offset[1];
}
