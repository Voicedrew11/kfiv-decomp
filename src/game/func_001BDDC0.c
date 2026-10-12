#include "kf_map.h"
#include "kf_collision.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerState D_00414280;      /* the player as of the frame before */
extern int D_00446770;
extern MapSlot D_0038CD20[2];
extern MapSlotMemory D_004321F0[2];
extern f32 D_00446968[];

void func_00216778(void *, void *);
void func_00216D20(void *, void *);
void KF_Vector_Scale(void *, void *, float);
int func_001C2D08(void *);
void func_001C0A88(void *, void *, void *, void *);
void func_001C0C50(void *, void *, void *, void *, f32);
void func_0020E1F8(f32 *, f32 *, f32 *, f32 *, f32, f32);
u32 func_001124E0(int, u32, u32);
u32 func_001125A8(u32, u32, int);
void func_001152B8(int);

/* Tests the player against the collision meshes of the records around him: what he touches, the walls that
   push him back, the water above him and the floor he stands on. Returns TRUE when a floor was found. */
int func_001BDDC0(void) {
    // match: the scratchpad points through pointers (retail forms each address anew), and the order of these
    // declarations decides which stack slot each value that does not fit a register gets
    Vector4 *wall_point = &COLLISION_SCRATCH->unk90;
    Vector4 *floor_point = &COLLISION_SCRATCH->point;
    Vector4 raised_point;
    s32 step_bit;
    s32 map;
    s32 record_count;
    s32 found;
    u8 stepping;
    s32 floor_map;
    f32 floor_height;
    f32 reach;
    MapSlot *slot;
    MapRecordBounds *record;
    s32 i;
    ModelInstance *model;
    CollisionMesh *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    s32 strips_left;
    s32 points_left;
    u32 surface;
    f32 drop;

    step_bit = (s32)(D_00413F80.unk1C8 >> 33) & 1;
    stepping = step_bit; // match: tested as the bit, kept as a byte
    if (step_bit) {
        if (D_00413F80.unk1D0 <= D_00413F80.unk1D4) {
            D_00413F80.position.y -= D_00413F80.unk1D0;
            D_00413F80.unk1C8 &= ~(0x8000L << 18);
            D_00413F80.unk1D4 = 0.0f;
            D_00413F80.unk1D0 = 0.0f;
        } else {
            D_00413F80.unk1D0 -= D_00413F80.unk1D4;
            D_00413F80.position.y -= D_00413F80.unk1D4;
        }
    }
    floor_height = D_00446968[0];
    func_00216778(wall_point, &D_00413F80.position);
    found = FALSE;
    wall_point->y -= 1.0f;
    func_00216778(floor_point, &D_00413F80.position);
    floor_point->y -= 75.0f;
    reach = D_00413F80.unk1AC + 75.0f;
    func_00216778(&raised_point, &D_00413F80.position);
    floor_map = D_00446770;
    D_00413F80.unk211 = D_00413F80.unk210 = 6;
    D_00413F80.unk214 = 0.0f;
    raised_point.y += 3.0f; // match: the raised point is never used
    for (map = 0; map < 2; map++) {
        slot = D_00446770 == map ? &D_0038CD20[0] : &D_0038CD20[1];
        if (slot->unk00 != 0) {
            record_count = slot->header->record_count;
            model = (ModelInstance *)D_004321F0[D_00446770 == map ? 0 : 1].unk34;
            record = (MapRecordBounds *)slot->records;
            for (i = 0; i < record_count; i++, record++, model++) {
                if (record->unk32 == 0xFFFF) {
                    continue;
                }
                if (record->unk3C & 0x40000000) {
                    continue;
                }
                if (D_00413F80.position.x < record->lowest[0] - 100.0f
                    || record->highest[0] + 100.0f < D_00413F80.position.x
                    || D_00413F80.position.z < record->lowest[2] - 100.0f
                    || record->highest[2] + 100.0f < D_00413F80.position.z) {
                    continue;
                }
                mesh = (CollisionMesh *)model->strips;
                ((void (*)(void *))func_001C2D08)(model); // match: called as if it returned nothing
                if (D_00413F80.unk1E0 == 0) {
                    strips_left = mesh->unk40;
                    strip = mesh->unk28;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            func_001C0A88(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &D_00413F80.position);
                            if (COLLISION_RESULT->hit != 0 && D_00413F80.position.y - 230.0f <= COLLISION_RESULT->point[1]
                                && COLLISION_RESULT->point[1] <= D_00413F80.position.y) {
                                surface = func_001125A8(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                                if (surface != 0) {
                                    D_00413F80.unk1E4 = surface;
                                    D_00413F80.unk1E0 = 1;
                                } else {
                                    D_00413F80.unk1EC = 1;
                                }
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
                strips_left = mesh->unk38;
                strip = mesh->unk18;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_0020E1F8((int)strip_point[0].position, (int)strip_point[1].position,
                                      (int)strip_point[2].position, (int)&COLLISION_SCRATCH->unk90, 100.0f, 230.0f);
                        if (COLLISION_RESULT->hit != 0) {
                            COLLISION_SCRATCH->push.x = COLLISION_SCRATCH->unk90.x - COLLISION_RESULT->point[0];
                            COLLISION_SCRATCH->push.y = 0.0f;
                            COLLISION_SCRATCH->push.z = COLLISION_SCRATCH->unk90.z - COLLISION_RESULT->point[2];
                            func_00216D20(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push);
                            KF_Vector_Scale(&COLLISION_SCRATCH->push, &COLLISION_SCRATCH->push, 100.0f);
                            COLLISION_SCRATCH->unk90.x = COLLISION_RESULT->point[0] + COLLISION_SCRATCH->push.x;
                            COLLISION_SCRATCH->unk90.z = COLLISION_RESULT->point[2] + COLLISION_SCRATCH->push.z;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
                if (map == 0) {
                    strips_left = mesh->unk42;
                    strip = mesh->unk2C;
                    while (--strips_left != -1) {
                        strip_point = strip->points;
                        points_left = strip->point_count - 2;
                        do {
                            func_001C0A88(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                          &D_00413F80.position);
                            if (COLLISION_RESULT->hit != 0
                                && COLLISION_RESULT->point[1] <= D_00413F80.position.y + D_00413F80.unk1AC
                                && (!((s32)(D_00413F80.unk1C8 >> 38) & 1) || COLLISION_RESULT->point[1] < D_00413F80.unk1DC)) {
                                D_00413F80.unk1DC = COLLISION_RESULT->point[1];
                                D_00413F80.unk1C8 |= 0x8000L << 23;
                                if (COLLISION_RESULT->point[1] <= D_00413F80.position.y - 220.0f) {
                                    D_00413F80.unk1C8 |= 0x8000L << 24;
                                } else {
                                    D_00413F80.unk1C8 &= ~(0x8000L << 24);
                                }
                            }
                            strip_point++;
                        } while (--points_left != 0);
                        strip = (CollisionStrip *)&strip_point[2];
                    }
                }
                if (stepping) {
                    continue;
                }
                if (D_00413F80.unk208 != 0
                    && (MAP_OBJECT_DEF_FLAGS(((MapObject *)D_00413F80.unk208)->unk04) & MAP_OBJECT_DEF_CARRIES)) {
                    continue;
                }
                strips_left = mesh->floor_strip_count;
                strip = mesh->floor_strips;
                while (--strips_left != -1) {
                    strip_point = strip->points;
                    points_left = strip->point_count - 2;
                    do {
                        func_001C0C50(strip_point[0].position, strip_point[1].position, strip_point[2].position,
                                      &COLLISION_SCRATCH->point, reach);
                        if (COLLISION_RESULT->hit != 0 && COLLISION_RESULT->point[1] < floor_height) {
                            found = TRUE;
                            floor_height = COLLISION_RESULT->point[1];
                            D_00413F80.unk250 = record->unk38;
                            D_00413F80.map_index = i;
                            D_00413F80.unk210 = func_001124E0(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                            floor_map = D_00446770 != map;
                        }
                        strip_point++;
                    } while (--points_left != 0);
                    strip = (CollisionStrip *)&strip_point[2];
                }
            }
        }
    }
    D_00413F80.position.x = wall_point->x;
    D_00413F80.position.z = wall_point->z;
    if (!stepping) {
        if (found) {
            drop = D_00413F80.position.y - floor_height;
            if (drop < 16.666666f) {
                D_00413F80.position.y = floor_height;
            } else {
                D_00413F80.unk1C8 |= 0x8000L << 18;
                D_00413F80.unk1D4 = 16.666666f;
                D_00413F80.position.y -= 16.666666f;
                D_00413F80.unk1D0 = drop - 16.666666f + 5.0f;
            }
            D_00413F80.unk1AC = 0.0f;
        } else if (D_00413F80.unk1A8 == 0) {
            D_00413F80.position.y += D_00413F80.unk1AC;
        } else {
            D_00413F80.position.y += D_00413F80.unk1AC * (f32)(60 - D_00413F80.unk1A8) / 60.0f;
        }
    }
    if (((s32)(D_00413F80.unk1C8 >> 33) & 1) || found) {
        D_00413F80.unk1C8 &= ~(0x8000L << 19);
    }
    if (!((s32)(D_00414280.unk1C8 >> 38) & 1) && ((s32)(D_00413F80.unk1C8 >> 38) & 1)) {
        D_00413F80.unk1AC = 0.0f;
        D_00413F80.unk1C8 &= ~(0x8000L << 19);
    }
    if (D_00414280.unk1E4 != 4 && D_00413F80.unk1E4 == 4) {
        D_00413F80.unk1AC = 0.0f;
        D_00413F80.unk1C8 &= ~(0x8000L << 19);
    }
    if (D_00413F80.unk1E0 == 0) {
        D_00413F80.unk1E8 = 0;
    } else if (D_00414280.unk1E0 != 0 && D_00414280.unk1E4 == D_00413F80.unk1E4) {
        D_00413F80.unk1E8++;
    } else {
        D_00413F80.unk1E8 = 0;
    }
    func_001152B8(floor_map);
    return found;
}
