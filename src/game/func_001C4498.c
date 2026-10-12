#include "kf_collision.h"

int puts(char *);
u32 func_001124E0(int, u32, u32);
int func_001C2D08(void *);
void func_0020DF38(f32 *, f32 *, f32 *, f32 *, f32);
extern char D_0043F5E8[];

/* Tests a point against a map object of kind 18. With flag 0x20000 clear the object's collision
   strips are tested, as func_001C3D00 does. With it set, model points 0 to 0x1F are taken four
   at a time as quads, up to the first quad with a corner at the model's own position (an unused
   point). */
void func_001C4498(MapObject *object, f32 *point, f32 reach) {
    ModelStripList *list;
    Strip *strip;
    StripVertex *vertex;
    f32 *corner0;
    f32 *corner1;
    f32 *corner2;
    f32 *corner3;
    int strips_left;
    int triangles_left;
    int i;

    if (!(object->unk10 & 0x10000)) {
        return;
    }
    if (!(object->unk10 & 0x20000)) {
        list = object->model->strips;
        if (list != NULL && list->unk0C >= 4U) {
            func_001C2D08(object->model);
            strip = list->strips;
            strips_left = list->strip_count;
            do {
                vertex = strip->vertices;
                triangles_left = strip->vertex_count - 2;
                do {
                    func_0020DF38(vertex[0].unk00, vertex[1].unk00, vertex[2].unk00, (int)point, reach);
                    if (COLLISION_RESULT->hit != 0) {
                        COLLISION_RESULT->unk18 = func_001124E0(vertex->unk04, vertex->unk08, vertex->unk0C);
                        return;
                    }
                    vertex++;
                } while (--triangles_left != 0);
                strip = (Strip *)(vertex + 2);
            } while (--strips_left != 0);
        }
        return;
    }
    for (i = 0; i < 0x20; i += 4) {
        corner0 = (f32 *)object->model->get_point(object->model, i);
        corner1 = (f32 *)object->model->get_point(object->model, i + 1);
        corner2 = (f32 *)object->model->get_point(object->model, i + 2);
        corner3 = (f32 *)object->model->get_point(object->model, i + 3);
        if (corner0[0] == object->model->position.x && corner0[1] == object->model->position.y && corner0[2] == object->model->position.z) {
            return;
        }
        if (corner1[0] == object->model->position.x && corner1[1] == object->model->position.y && corner1[2] == object->model->position.z) {
            return;
        }
        if (corner2[0] == object->model->position.x && corner2[1] == object->model->position.y && corner2[2] == object->model->position.z) {
            return;
        }
        if (corner3[0] == object->model->position.x && corner3[1] == object->model->position.y && corner3[2] == object->model->position.z) {
            return;
        }
        func_0020DF38(corner0, corner1, corner2, (int)point, reach);
        if (COLLISION_RESULT->hit != 0) {
            puts(D_0043F5E8);
            return;
        }
        func_0020DF38(corner0, corner2, corner3, (int)point, reach);
        if (COLLISION_RESULT->hit != 0) {
            puts(D_0043F5E8);
            return;
        }
    }
}
