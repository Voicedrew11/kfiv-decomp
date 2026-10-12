#include "kf_script.h"
#include "kf_map.h"

extern MapSlot D_0038CD20[];
extern int D_00446770;

/* Set the direction and colour of one of the three lights of a light block of the current map. */
void func_00214358(ScriptLightCommand *command) {
    MapSlot *slot = &D_0038CD20[D_00446770];
    VuVector4 direction = (VuVector4){ command->dir_x.value, command->dir_y.value, command->dir_z.value };
    MapSlotUnk10Lights *lights;

    if ((u32)command->block.value < 0x20 && command->light.value >= 0 && command->light.value < 3) {
        // match: the block's offset is added to the address as a number; indexing the blocks swaps two registers
        lights = (MapSlotUnk10Lights *)(command->block.value * sizeof(MapSlotUnk10Lights) + (u32)slot->unk10);
        lights->directions[0][command->light.value] = command->dir_x.value;
        lights->directions[1][command->light.value] = command->dir_y.value;
        lights->directions[2][command->light.value] = command->dir_z.value;
        lights->colors[command->light.value].x = (f32)command->red.value / 255.0f;
        lights->colors[command->light.value].y = (f32)command->green.value / 255.0f;
        lights->colors[command->light.value].z = (f32)command->blue.value / 255.0f;
    }
    command->active = 0;
}
