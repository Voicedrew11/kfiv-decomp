#include "kf_collision.h"

void KF_Vu0_LoadMatrix(void *);

/* Places the points of a model's collision mesh in the world: each is multiplied by the model's
   matrix and written to the scratchpad (COLLISION_WORLD_POINTS), three at a time and then the
   one or two left over. */
// match: nothing is returned, but the callers that read $2 after it need the int
int func_001C2D08(void *target) {
    ModelInstance *model = target;
    CollisionMesh *mesh;
    Vector4 *world = COLLISION_WORLD_POINTS;
    Vector4 *local;
    u32 count;
    s32 i;

    mesh = (CollisionMesh *)model->strips;
    KF_Vu0_LoadMatrix(model->unk80);
    count = mesh->unk0C;
    local = (Vector4 *)mesh->unk08;
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
}
