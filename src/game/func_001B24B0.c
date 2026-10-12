#include "kf_collision.h"

// match: declared without a size, so it is not addressed through $gp
extern CollisionListPick2 D_00446918[];
extern const VuVector4 D_0043F200;

void KF_Vu0_LoadMatrix(void *);
u32 func_001124E0(int, u32, u32);
void func_0020DF38(f32 *, f32 *, f32 *, f32 *, f32);

/* func_001B21F8 for strip lists 1 and 5 only: returns 1 when the sphere touches a triangle
   of them (the result is left in the scratchpad), 0 when it touches none. */
int func_001B24B0(ModelInstance *model, void *request, f32 reach) {
    CollisionListPick2 lists = D_00446918[0];
    f32 unused_work[8]; // match: never used, keeps the retail stack frame size
    VuVector4 unused_axis = D_0043F200; // match: copied and never read
    CollisionMeshLists *mesh;
    CollisionStrip *strip;
    CollisionStripPoint *strip_point;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;
    s32 list;
    s32 points_left;

    mesh = (CollisionMeshLists *)model->strips;
    COLLISION_RESULT->hit = 0;
    if (mesh == NULL) {
        return;
    }
    if (((CollisionMesh *)mesh)->unk0C < 4) {
        return;
    }
    KF_Vu0_LoadMatrix(model->unk40);
    count = ((CollisionMesh *)mesh)->unk0C;
    local = (Vector4 *)((CollisionMesh *)mesh)->unk08;
    i = count / 3;
    do {
        __asm__ volatile("pref 0, 0x30(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%3)\n"
            "lqc2 $vf17, 0x0(%4)\n"
            "lqc2 $vf18, 0x0(%5)\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vmulax.xyzw ACC, $vf1, $vf17x\n"
            "vmadday.xyzw ACC, $vf2, $vf17y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf17z\n"
            "vmaddw.xyzw $vf17, $vf4, $vf17w\n"
            "vmulax.xyzw ACC, $vf1, $vf18x\n"
            "vmadday.xyzw ACC, $vf2, $vf18y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf18z\n"
            "vmaddw.xyzw $vf18, $vf4, $vf18w\n"
            "sqc2 $vf16, 0x0(%0)\n"
            "sqc2 $vf17, 0x0(%1)\n"
            "vnop\n"
            "sqc2 $vf18, 0x0(%2)\n"
            : : "r"(world), "r"(world + 1), "r"(world + 2), "r"(local), "r"(local + 1), "r"(local + 2));
        local += 3;
        world += 3;
    } while (--i != 0);
    for (i = count % 3 - 1; i != -1; i--) {
        __asm__ volatile("pref 0, 0x10(%0)\n" : : "r"(local));
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "vmulax.xyzw ACC, $vf1, $vf16x\n"
            "vmadday.xyzw ACC, $vf2, $vf16y\n"
            "vmaddaz.xyzw ACC, $vf3, $vf16z\n"
            "vmaddw.xyzw $vf16, $vf4, $vf16w\n"
            "vnop\n"
            "vnop\n"
            "vnop\n"
            "sqc2 $vf16, 0x0(%1)\n"
            : : "r"(local), "r"(world));
        local++;
        world++;
    }
    for (list = 0; list < 2; list++) {
        strip = mesh->strip_lists[lists.lists[list]];
        i = mesh->strip_counts[lists.lists[list]];
        while (--i != -1) {
            strip_point = strip->points;
            points_left = strip->point_count - 2;
            do {
                func_0020DF38(strip_point[0].position, strip_point[1].position, strip_point[2].position, (int)request, reach);
                if (COLLISION_RESULT->hit != 0) {
                    COLLISION_RESULT->unk18 = func_001124E0(strip_point->unk04, strip_point->unk08, strip_point->unk0C);
                    return 1;
                }
                strip_point++;
            } while (--points_left != 0);
            strip = (CollisionStrip *)&strip_point[2];
        }
    }
    return 0;
}
