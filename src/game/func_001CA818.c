#include "kf_map.h"

void *memset(void *, int, u32);
int func_00102238(char *, int, int);
int func_001025C0(int, void *, int);
int func_001024B0(int);
extern char D_0043F620[];
extern char D_0043F630[];
extern MapActor *D_004469E0;
extern s32 D_004471B8[2];
extern MapActor D_005B6100[2][MAP_ACTOR_COUNT];
extern ModelInstance D_005B6680[2][MAP_ACTOR_COUNT];
extern char D_005B9E80[0x2000];
extern char D_005BBE80[0x2000];
extern char D_005BDE80[2][MAP_ACTOR_COUNT][0x80];

/* Clears the map actors and loads their model (item/i0000.om2) and texture (item/i0000.tx2). */
void func_001CA818(void) {
    int file;

    D_004469E0 = NULL;
    memset(D_004471B8, 0, sizeof(D_004471B8));
    memset(D_005B6100, 0, sizeof(D_005B6100));
    memset(D_005B6680, 0, sizeof(D_005B6680));
    memset(D_005B9E80, 0, sizeof(D_005B9E80));
    memset(D_005BBE80, 0, sizeof(D_005BBE80));
    memset(D_005BDE80, 0, sizeof(D_005BDE80));
    file = func_00102238((int)D_0043F620, 0, 1);
    func_001025C0(file, D_005BBE80, -1);
    func_001024B0(file);
    file = func_00102238((int)D_0043F630, 0, 1);
    func_001025C0(file, D_005B9E80, -1);
    func_001024B0(file);
}
