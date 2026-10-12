#include "kf_map.h"
#include "kf_screen.h"

void *memset(void *, int, u32);
void func_0010B758(ModelInstance *, int *, int *);
void func_001CB710(OverlaySprite *);
int func_001CB0D0(Vector4 *, Vector4 *, int, int, int, int, int);
extern MapDrawState D_0038A1B0;
extern MapSlot D_0038CD20[];
extern ObjectMarkRow D_00362928[];
extern f32 D_00431AF0[16];
extern s32 D_004471B8[2];
extern MapActor D_005B6100[2][MAP_ACTOR_COUNT];
extern ModelInstance D_005B6680[2][MAP_ACTOR_COUNT];
extern char D_005B9E80[0x2000];
extern char D_005BBE80[0x2000];
extern char D_005BDE80[2][MAP_ACTOR_COUNT][0x80];

/* copies a 4x4 matrix a quadword at a time */
static inline void copy_matrix(ModelMatrixQwords *target, ModelMatrixQwords *origin) {
    Uint128 row0 = origin->rows[0];
    Uint128 row1 = origin->rows[1];
    Uint128 row2 = origin->rows[2];
    Uint128 row3 = origin->rows[3];

    target->rows[0] = row0;
    target->rows[1] = row1;
    target->rows[2] = row2;
    target->rows[3] = row3;
}

/* Sets up a map slot's gold: gives each of the 16 map actors its model, then drops again what the map's marks
   say was lying there when it was left, and clears the marks. */
void func_001CA920(int map) {
    f32 position[4];
    f32 rotation[4];
    ObjectMarkRow marks;
    int source; // match: two variables, an array of two would be padded to 16 bytes
    int work;
    MapActor *actor;
    ModelInstance *model;
    char *matrix;
    s32 i;
    s32 tag;

    memset(position, 0, sizeof(position));
    position[3] = 1.0f;
    memset(rotation, 0, sizeof(rotation));
    rotation[3] = 1.0f;
    D_004471B8[map] = 0;
    memset(D_005B6100[map], 0, sizeof(D_005B6100[0]));
    memset(D_005B6680[map], 0, sizeof(D_005B6680[0]));
    memset(D_005BDE80[map], 0, sizeof(D_005BDE80[0]));
    actor = D_005B6100[map];
    model = D_005B6680[map];
    matrix = D_005BDE80[map][0];
    for (i = 0; i < MAP_ACTOR_COUNT; i++, actor++, model++, matrix += 0x80) {
        actor->body = model;
        work = (int)matrix;
        source = (int)D_005BBE80;
        func_0010B758(model, &source, &work);
        copy_matrix((ModelMatrixQwords *)matrix, (ModelMatrixQwords *)D_00431AF0);
        model->unk148 = (s32)&D_0038A1B0.unkC0;
        model->unk14C = &D_0038A1B0.unkA0;
        model->unk194 = (int)D_005B9E80;
        model->unk190 = 2;
        model->draw = (void (*)(ModelInstance *))func_001CB710;
    }
    tag = D_0038CD20[map].tag; // match: kept in a variable, retail does not read it again after the copy
    marks = D_00362928[tag];
    memset(&D_00362928[tag], 0, sizeof(ObjectMarkRow));
    for (i = 0; i < MAP_ACTOR_COUNT; i++) {
        if (marks.marks[i].unk08 != 0) {
            position[0] = marks.marks[i].x;
            position[1] = marks.marks[i].y;
            position[2] = marks.marks[i].z;
            rotation[1] = marks.marks[i].radius * 0.0625f;
            func_001CB0D0((Vector4 *)position, (Vector4 *)rotation, map, marks.marks[i].unk08, marks.marks[i].unk07, FALSE, FALSE);
        }
    }
}
