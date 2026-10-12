#include "kf_script.h"
#include "kf_map.h"

extern MapDrawStateOverlays D_0038A1B0;

/* Set the colour, alpha and blend mode of the screen overlay. */
void func_00214B88(ScriptScreenColorCommand *command) {
    MapDrawStateOverlays *overlays = &D_0038A1B0;

    overlays->unk6C = command->red.value;
    overlays->unk70 = command->green.value;
    overlays->unk74 = command->blue.value;
    overlays->unk78 = command->alpha.value;
    switch (command->blend.value) {
    case 0:
        overlays->unk80 = 0x44;
    case 1:  // match: no break in any case, so the blend mode always ends as 0x44
        D_0038A1B0.unk80 = 0x48;
    case 2:
        D_0038A1B0.unk80 = 0x42;
    default:
        D_0038A1B0.unk80 = 0x44;
    }
    command->active = 0;
}
