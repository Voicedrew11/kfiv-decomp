#include "kf_map.h"
#include "kf_game.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
MapRecordInfo *func_00115CD0(CameraState *, f32, int);
void KF_Vector_Scale(float *, float *, float);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0019AB60(ModelInstance *);
void func_0010AEB8(void *);
void func_0010DB40(ModelInstance *);
extern MapDrawState D_0038A1B0;
extern MapSlot D_0038CD20[];
extern ItemNode *D_004469D4;
extern ItemNode *D_004469D8;

/* Draws the items lying in the maps, lit like the map record each lies in. The one being picked up is left
   out unless with_picked is set. */
void func_001C9650(int with_picked) {
    f32 scale[4];
    f32 unused[8];
    ItemNode *node;
    ModelPart *part;
    s32 count;
    s32 i;

    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
    for (node = D_004469D4; node != NULL; node = node->next) {
        node->map_record = func_00115CD0((CameraState *)&node->model->position, 0.0f, node->flags & 1);
        if (node->map_record == NULL) {
            continue;
        }
        if (node == D_004469D8 && with_picked == 0) {
            continue;
        }
        KF_Vector_Scale(scale, &node->model->unkE0, 1.0f);
        func_0020C300(node->model, &node->model->position, &node->model->rotation, scale);
        part = node->model->parts;
        count = node->model->part_count;
        for (i = 0; i < count; i++, part++) {
            func_0020B8B0(node->model, part, part);
        }
        node->model->unk148 = (s32)&D_0038A1B0.unkA90[node->flags & 1][D_0038CD20[node->flags & 1].header->unk71];
        node->model->unk14C = &D_0038A1B0.unk290[node->flags & 1][((MapRecordInfo *)node->map_record)->unk34];
        func_0019AB60(node->model);
        func_0010AEB8(node->model);
        func_0010DB40(node->model);
    }
}
