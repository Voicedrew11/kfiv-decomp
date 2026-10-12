#include "kf_entity.h"

void *func_001B1390();
char *func_0010CAB8(char *, u32);
void func_001B4F30(u8, void *, void *, void *, void *);

/* launches the shots of a record whose frame lies in (frame_from, frame_to] */
int func_0018D4E8(Entity *entity, EntityParamsUnk3D8 *unk3D8, s32 frame_from, s32 frame_to, f32 *direction) {
    EntityParamsUnk3D8Full *record = (EntityParamsUnk3D8Full *)unk3D8;
    EntityUnk1B1390 *shot;
    char *origin;
    s32 i;

    if (entity->unk2A != 0 && record->unk0C == 0) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (record->unk10[i].unk02 != 0 && frame_from < record->unk10[i].unk02 && record->unk10[i].unk02 <= frame_to) {
            shot = func_001B1390();
            shot->unk00 = 1;
            shot->unk20 = 0;
            shot->unk22 = 0;
            shot->unk28 = entity->params->unk238;
            origin = func_0010CAB8((char *)entity->model, record->unk10[i].unk01);
            if (origin != NULL) {
                func_001B4F30(record->unk10[i].unk00, shot, origin, direction, entity);
            }
        }
    }
    return 0;
}
