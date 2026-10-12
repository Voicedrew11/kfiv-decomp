#include "kf_packet.h"
#include "kf_texture.h"

int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
void func_0010B510(char *, int, int);
void func_001D1378(ScreenSprite *, s64, u64, u64, u8, u8, u8, u8, s32, s32, u32, s32, s32, s32, s32, s32, s32);

extern char D_0043F6B8[];
extern char D_0043F6C8[];
extern char *D_00446F6C;
extern TextureTableHeader *D_004471F0;
extern ScreenSprite D_005CDE18[10];
extern ScreenSprite D_005CE098;

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads two files after the data loaded so far (the second is a texture table, uploaded at once) and sets up
   the eleven screen sprites drawn from it. */
void func_001D1450(void) {
    char *address = D_00446F6C;
    int file;
    int size;

    file = func_00102238((int)D_0043F6B8, 0, 1);
    size = func_001025C0(file, address, -1);
    func_001024B0(file);
    address += size;
    address = align16(address);
    D_004471F0 = (TextureTableHeader *)address;
    file = func_00102238((int)D_0043F6C8, 0, 1);
    func_001025C0(file, address, -1);
    func_001024B0(file);
    func_0010B510((char *)D_004471F0, 0x2BC0, 0x3FE0);
    func_001D1378(&D_005CDE18[0], 0x156, 0x4007DA85E1313640, 0x60, 0x5A, 0x5A, 0x5A, 0x80,
                  0, 0, 0, 0xDA, 0x5A, 0, 0, 0x100, 0x80);
    func_001D1378(&D_005CDE18[1], 0x156, 0x8007DC8599307780, 0x60, 0x6C, 0x2A, 0xD, 0x80,
                  0x56, 0x15, 0, 0x7C, 0xA, 0, 0, 0x40, 0x14);
    func_001D1378(&D_005CDE18[2], 0x156, 0x8007DC8599307780, 0x60, 0x39, 0x51, 0x6D, 0x80,
                  0x56, 0x20, 0, 0x7C, 0xB, 0, 0x14, 0x40, 0x28);
    func_001D1378(&D_005CDE18[3], 0x156, 0x8007DB85DD30B700, 0x60, 0x6D, 0x26, 0, 0x80,
                  0x17, 0x14, 0, 0x33, 0x31, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CDE18[4], 0x156, 0x8007DD05DD30B540, 0x60, 0x77, 0x7D, 0x52, 0x80,
                  0x17, 0x14, 0, 0x33, 0x31, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CDE18[5], 0x156, 0x8007DA05DD30B740, 0, 0x80, 0x80, 0x80, 0x80,
                  5, 4, 0, 0x50, 0x52, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CDE18[6], 0x156, 0x8007DB05DD30B6C0, 0x60, 0x44, 0x2E, 0x4A, 0x80,
                  5, 4, 0, 0x50, 0x52, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CDE18[7], 0x156, 0x8007DC8599307780, 0x60, 0, 0xB4, 0xD0, 0x80,
                  0x6E, 0x33, 0, 0x33, 4, 0, 0x14, 0x40, 0x28);
    func_001D1378(&D_005CDE18[8], 0x156, 0x8007DB85DD30B700, 0x60, 0, 0, 0, 0x80,
                  0x5A, 0x33, 0, 0xB, 0xC, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CDE18[9], 0x156, 0x8007DD05DD30B540, 0x60, 0x80, 0x80, 0x80, 0x50,
                  0x5A, 0x33, 0, 0xB, 0xC, 0, 0, 0x80, 0x80);
    func_001D1378(&D_005CE098, 0x156, 0x4007FC055D30ABC0, 0x60, 0x80, 0x80, 0x80, 0x80,
                  0x100, 0xD0, 0, 0x80, 0x20, 0, 0, 0x80, 0x20);
}
