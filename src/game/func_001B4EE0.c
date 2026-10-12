#include "kf_effect.h"

extern char D_0043F248[];
extern FxTableEntry D_00402DA8[];

int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);

/* Loads the effect table D_00402DA8 from its file. */
void func_001B4EE0(void) {
    int file_handle = func_00102238((int)D_0043F248, 0, 1);

    func_001025C0(file_handle, D_00402DA8, -1);
    func_001024B0(file_handle);
}
