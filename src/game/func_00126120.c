#include "kf_entity.h"

void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void func_0010B5D0(ModelPart *);
void func_00216778(void *, void *);

/* Rebuilds the entity's model matrices with the root part's own offset and rotation left out. */
void func_00126120(Entity *entity) {
    f32 saved_offset[4];
    f32 saved_rotation[4];
    ModelPart *root;
    ModelPart *part;
    int part_count;
    int i;

    root = ENTITY_MODEL(entity)->unk130;
    part_count = ENTITY_MODEL(entity)->part_count;
    func_0020C300(ENTITY_MODEL(entity), &ENTITY_MODEL(entity)->position, &ENTITY_MODEL(entity)->rotation, &ENTITY_MODEL(entity)->unkE0);
    func_00216778(saved_offset, root->unk40);
    func_00216778(saved_rotation, &root->unk50);
    root->unk40[0] = root->unk40[1] = root->unk40[2] = 0.0f;
    root->unk50 = root->unk54 = root->unk58 = 0.0f;
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        func_0020B8B0(ENTITY_MODEL(entity), part, part);
        part++;
    }
    func_00216778(root->unk40, saved_offset);
    func_00216778(&root->unk50, saved_rotation);
    part = ENTITY_MODEL(entity)->parts;
    for (i = 0; i < part_count; i++) {
        func_0010B5D0(part);
        part++;
    }
}
