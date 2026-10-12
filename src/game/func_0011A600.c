#include "kf_map.h"
#include "kf_collision.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern char D_0043E730[];       /* "map_view/%3.3d.tx2" */
extern char D_0043E748[];       /* "map_view/pointer.omd" */
extern f32 D_00446798[];        /* the largest float */
extern f32 D_0044679C[];        /* the lowest float */
extern char *D_0044705C;
extern int D_00447060;
extern int D_00447064;
extern Vector4 D_00472060;
extern Vector4 D_00472070;
extern ModelInstance D_00472080;

void *memset(void *, int, u32);
int sprintf(char *, char *, ...);
int func_00102238(int, int, int);
int func_001024B0(int);
int func_001025C0(int, void *, int);
void func_00109800(void);
void func_00109878(int);
void func_00109A38(int *, int *);
void func_0010B510(char *, int, int);
void func_0010B5D0(ModelPart *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010E6A8(ModelInstance *, s64, s64);
int func_0011C678(int);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_00216778(void *, void *);

static inline char *align16(char *address) {
    if ((int)address & 0xF) {
        address -= ((int)address & 0xF) - 0x10;
    }
    return address;
}

/* Loads the picked viewer's textures; for viewer 4 also the pointer model, put where the player is, and
   the box that holds every ordinary map record. FALSE when a file is missing. */
int func_0011A600(void) {
    char name[0x100];
    char *model_data;
    char *buffer;
    int file;
    int size;
    int i;
    int j;
    MapRecordBounds *record;
    int count;

    sprintf(name, D_0043E730, D_00447064);
    file = func_00102238((int)name, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    func_00109800();
    func_00109878(2);
    D_0044705C = NULL;
    D_00447060 = 0;
    func_00109A38((int *)&D_0044705C, &D_00447060);
    memset(D_0044705C, 0, D_00447060);
    size = func_001025C0(file, D_0044705C, -1);
    func_001024B0(file);
    func_0010B510(D_0044705C, 0x2BC0, 0x3FE0);
    if (D_00447064 != 4) {
        return TRUE;
    }
    model_data = D_0044705C + size;
    model_data = align16(model_data);
    file = func_00102238((int)D_0043E748, 0, 1);
    if (file < 0) {
        return FALSE;
    }
    size = func_001025C0(file, model_data, -1);
    func_001024B0(file);
    buffer = model_data + size;
    buffer = align16(buffer);
    func_0010D018(&D_00472080, &model_data, &buffer);
    func_0010E6A8(&D_00472080, 0x260, 0x240);
    func_00216778(&D_00472080.position, &D_00413F80);
    D_00472080.position.y -= 100.0f;
    D_00472080.rotation.x = 0.0f;
    D_00472080.rotation.y = D_00413F80.rotation.y;
    D_00472080.rotation.z = 0.0f;
    D_00472080.unkE0 = D_00472080.unkE4 = D_00472080.unkE8 = D_00472080.rotation.w = 4.0f;
    D_00472080.unkEC = 1.0f;
    // match: it returns a value
    ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))func_0020C300)(&D_00472080, &D_00472080.position, &D_00472080.rotation, &D_00472080.unkE0);
    for (i = 0; i < D_00472080.part_count; i++) {
        // match: the callee returns a value here, which keeps $2 busy
        ((int (*)(ModelInstance *, ModelPart *, ModelPart *))func_0020B8B0)(&D_00472080, &D_00472080.parts[i], &D_00472080.parts[i]);
    }
    for (i = 0; i < D_00472080.part_count; i++) {
        func_0010B5D0(&D_00472080.parts[i]);
    }
    D_00472060.z = D_00472060.y = D_00472060.x = D_00446798[0];
    D_00472070.z = D_00472070.y = D_00472070.x = D_0044679C[0];
    for (i = 0; i < 2; i++) {
        if (D_0038CD20[i].unk00 != 0 && func_0011C678(D_0038CD20[i].tag) != 0) {
            count = ((MapSlotUnk08 *)D_0038CD20[i].unk08)->record_count;
            record = (MapRecordBounds *)D_0038CD20[i].records;
            for (j = 0; j < count; j++, record++) {
                if (D_00472060.x > record->unk40[0]) {
                    D_00472060.x = record->unk40[0];
                }
                if (D_00472060.y > record->unk40[1]) {
                    D_00472060.y = record->unk40[1];
                }
                if (D_00472060.z > record->unk40[2]) {
                    D_00472060.z = record->unk40[2];
                }
                if (D_00472070.x < record->unk50[0]) {
                    D_00472070.x = record->unk50[0];
                }
                if (D_00472070.y < record->unk50[1]) {
                    D_00472070.y = record->unk50[1];
                }
                if (D_00472070.z < record->unk50[2]) {
                    D_00472070.z = record->unk50[2];
                }
            }
        }
    }
    return TRUE;
}
