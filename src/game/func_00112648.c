#include "kf_game.h"
#include "kf_file.h"

extern char map_object_defs[];
extern GameDefinition item_name_defs[380];
extern u8 D_0041A6F8[];
extern u8 weapon_defs[];
extern char worn_defs[];
extern char D_0043E4E8[];
extern char D_0043E4F8[];
extern char D_0043E508[];
extern char D_0043E520[];
extern char D_0043E538[];
extern char D_0043E550[];
extern char *D_004467C0;

/* Reads the six game data files, then works out each definition's scaled value. */
void func_00112648(void) {
    GameDefinition *definition;
    int handle;
    int i;

    handle = func_00102238(D_0043E4E8, 0, 1);
    func_001025C0(handle, map_object_defs, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E4F8, 0, 1);
    func_001025C0(handle, D_004467C0, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E508, 0, 1);
    func_001025C0(handle, item_name_defs, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E520, 0, 1);
    func_001025C0(handle, D_0041A6F8, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E538, 0, 1);
    func_001025C0(handle, weapon_defs, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E550, 0, 1);
    func_001025C0(handle, worn_defs, -1);
    func_001024B0(handle);
    definition = item_name_defs;
    for (i = 379; i >= 0; i--) {
        if (definition->unk33 != 0) {
            definition->unk3E = (f32)definition->unk33 * 2.2046; /* kilograms to pounds; the double is D_0043E568 */
        } else {
            definition->unk3E = 0;
        }
        definition++;
    }
}
