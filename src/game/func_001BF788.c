#include "common.h"

extern u32 D_00446F64;
extern u32 D_00447164;
extern u32 D_00447168;
extern u32 D_00447170;
extern int D_00446970;
extern int D_00446974;
extern char D_0043F588[];
extern char D_00414780[];
extern char D_004147A0[];
extern char D_0057B6A0[];
extern char D_0057B860[];
extern char D_0057BAB0[];
extern char D_0057CB40[];

void *memset(void *, int, u32);
int func_00102238(char *, int, int);
int func_001025C0(int, int, int);
int func_001024B0(int);
void func_001BF898(void);

void func_001BF788(void) {
    int fd;
    int t;
    t = D_00446F64;
    D_00447164 = t;
    t += 0x20000;
    D_00447170 = t;
    t += 0x50000;
    D_00447168 = t;
    memset(D_0057B860, 0, 0x1C0);
    memset(D_0057B6A0, 0, 0x1C0);
    memset((void *)D_00447170, 0, 0x50000);
    memset((void *)D_00447168, 0, 0x30000);
    memset(D_0057CB40, 0, 0x1000);
    memset(D_0057BAB0, 0, 0x1000);
    memset(D_00414780, 0, 0x20);
    memset(D_004147A0, 0, 0x20);
    D_00446970 = -1;
    D_00446974 = -1;
    fd = func_00102238(D_0043F588, 0, 1);
    func_001025C0(fd, (int)D_00447164, -1);
    func_001024B0(fd);
    func_001BF898();
}
