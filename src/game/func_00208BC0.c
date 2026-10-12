#include "kf_model.h"

extern char D_004410C8[];
extern char D_004410E0[];
extern char D_004410F8[];
extern char D_00441110[];
extern char D_00441128[];
extern char *D_005F9B40[7];
extern int D_00446F28;
extern char *D_00446F70;
extern ModelInstance *D_00447394;
extern ModelInstance *D_00447398;
extern ModelInstance *D_0044739C;
extern int D_004473A0;

void *memset(void *, int, u32);
int sprintf(char *, char *, ...);
int func_00102238(char *, int, int);
int func_001024B0(int);
int func_001025C0(int, void *, int);
void func_00108940(int, int *, ModelInstance *, int);
void func_0010B510(char *, int, int);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010DB40(ModelInstance *);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E770(ModelInstance *, u64, s64);
void func_0010E820(ModelInstance *, s64);
void func_0010E8E0(void *, int, int, int, int);

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads what the item effect of func_00209018 needs into the memory at D_00446F70: two models with a
   motion each, and seven textures. */
void func_00208BC0(void) {
    char name[0x100];
    char *model_data;
    char *buffer;
    int file;
    int size;
    int i;

    memset(D_00446F70, 0, D_00446F28);
    buffer = D_00446F70;
    D_00447394 = NULL;
    D_00447398 = NULL;
    memset(D_005F9B40, 0, sizeof(D_005F9B40));
    D_004473A0 = 0;
    D_0044739C = NULL;

    buffer = align16(buffer);
    D_00447394 = (ModelInstance *)buffer;
    buffer += sizeof(ModelInstance);
    buffer = align16(buffer);
    file = func_00102238((int)D_004410C8, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_0010D018(D_00447394, &model_data, &buffer);
    func_0010E8E0(D_00447394, 0x80, 0x80, 0x80, 0x80);
    func_0010E820(D_00447394, 0x44);
    func_0010E6A8(D_00447394, 0x60, 0x40);
    func_0010E770(D_00447394, 0x4007D805E1313B40, 0x60);
    D_00447394->unk1A8 = func_0010DB40;
    buffer = align16(buffer);
    file = func_00102238((int)D_004410E0, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_00108940((int)model_data, (int *)&buffer, D_00447394, 1);

    buffer = align16(buffer);
    D_00447398 = (ModelInstance *)buffer;
    buffer += sizeof(ModelInstance);
    buffer = align16(buffer);
    file = func_00102238((int)D_004410F8, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_0010D018(D_00447398, &model_data, &buffer);
    func_0010E8E0(D_00447398, 0x80, 0x80, 0x80, 0x80);
    func_0010E820(D_00447398, 0x44);
    func_0010E6A8(D_00447398, 0x60, 0x40);
    func_0010E770(D_00447398, 0x4007D805E1313B40, 0x60);
    D_00447394->unk1A8 = func_0010DB40; // match: the first model again, not the second
    buffer = align16(buffer);
    file = func_00102238((int)D_00441110, 0, 1);
    size = func_001025C0(file, buffer, -1);
    func_001024B0(file);
    model_data = buffer;
    buffer += size;
    buffer = align16(buffer);
    func_00108940((int)model_data, (int *)&buffer, D_00447398, 1);

    for (i = 0; i < 7; i++) {
        sprintf(name, D_00441128, i + 40);
        file = func_00102238((int)name, 0, 1);
        if (file >= 0) {
            buffer = align16(buffer);
            size = func_001025C0(file, buffer, -1);
            func_001024B0(file);
            D_005F9B40[i] = buffer;
            buffer += size;
            func_0010B510(D_005F9B40[i], 0x3B40, 0x3EC0);
        }
    }
}
