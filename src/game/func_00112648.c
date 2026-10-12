#include "kf_game.h"
#include "kf_file.h"

extern char D_0038DA30[];
extern GameDefinition D_004147F8[380];
extern u8 D_0041A6F8[];
extern u8 D_0041B9B8[];
extern char D_0041E118[];
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
    func_001025C0(handle, D_0038DA30, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E4F8, 0, 1);
    func_001025C0(handle, D_004467C0, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E508, 0, 1);
    func_001025C0(handle, D_004147F8, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E520, 0, 1);
    func_001025C0(handle, D_0041A6F8, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E538, 0, 1);
    func_001025C0(handle, D_0041B9B8, -1);
    func_001024B0(handle);
    handle = func_00102238(D_0043E550, 0, 1);
    func_001025C0(handle, D_0041E118, -1);
    func_001024B0(handle);
    definition = D_004147F8;
    for (i = 379; i >= 0; i--) {
        if (definition->unk33 != 0) {
            definition->unk3E = (f32)definition->unk33 * 2.2046; /* kilograms to pounds; the double is D_0043E568 */
        } else {
            definition->unk3E = 0;
        }
        definition++;
    }
}
