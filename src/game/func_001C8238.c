#include "kf_model.h"
#include "kf_player.h"

int func_001CA338(int);
extern LevelDef D_004147F8[];
extern s32 D_004469D0;
extern ModelNode *D_004469D4;
extern ModelNode *D_004469D8;
extern s32 D_00447198;
extern f32 D_0044719C;
extern s32 D_004471A4;

/* Resets the model node lists and marks every unset item record state as 1. */
void func_001C8238(void) {
    LevelDef *def;
    s32 i;

    D_004469D4 = NULL;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_00447198 = 0;
    D_004471A4 = 0;
    D_004469D0 = 0;
    func_001CA338(0);
    func_001CA338(1);
    def = D_004147F8;
    for (i = 0; i < 0x17C; i++, def++) {
        if (def->state == 0) {
            def->state = 1;
        }
    }
}
