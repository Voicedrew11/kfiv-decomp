#include "kf_map.h"
#include "kf_linked_point.h"
#include "kf_gs.h"
#include "kf_map_object.h"
#include "kf_map_scroll.h"
#include "kf_entity.h"
#include "kf_collision.h"
#include "kf_screen.h"
#include "kf_game.h"
#include "kf_gold.h"
#include "kf_entity_world.h"
#include "kf_sound.h"
#include "kf_effect.h"
#include "kf_model.h"
#include "kf_player.h"
#include "kf_message.h"
#include "kf_packet.h"
#include "kf_texture.h"

#define FOG_COLOR() \
    ((u64)map_draw_state.unk280.red | ((u64)map_draw_state.unk280.green << 8) | ((u64)map_draw_state.unk280.blue << 16) | \
     ((u64)0x80 << 24))
#define USABLE(node) (!((node)->flags & 0x40000000))
#define IN_X(node) ((node)->left - margin <= point[0] && point[0] <= (node)->right + margin)
#define IN_Y(node) ((node)->unk44 - margin <= point[1] && point[1] <= (node)->unk54 + margin)
#define IN_Z(node) ((node)->top - margin <= point[2] && point[2] <= (node)->bottom + margin)
/* how far the point is outside the box along one axis */
#define DISTANCE(low, high, value)                                                                 \
    if ((value) < (low) - margin) {                                                                \
        distance = ((low) - margin) - (value);                                                     \
    } else {                                                                                       \
        distance = (value) - ((high) + margin);                                                    \
    }                                                                                              \
    if (distance < best_distance) {                                                                \
        best_distance = distance;                                                                  \
        best = node;                                                                               \
    }
#define USABLE_IN_MAP(node) (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map && USABLE(node))
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A008(void);
void func_0010EE98(ModelInstance *);
void func_00111368(ModelInstance *, int, int, int, int);
void Map_SetOffscreenTarget(void);
void Map_QueueSlotPacket(int, int);
int Map_ClassifyRecordOnScreen(MapDrawRecord *, ModelInstance *);
void *memset(void *, int, u32);
void Map_InsertRecordBySize(AreaNode *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010B758(ModelInstance *, int *, int *);
MotionData *func_00108940(MotionFile *, ModelChannel **, ModelInstance *, int);
u32 *Map_GetSlotPacket(int, int);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E820(ModelInstance *, s64);
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *, f32, int);
void func_00111E28(ModelInstance *, u32 *);
void Vector_Copy(void *, void *);
char *Collision_RelocateMesh(char *, ModelInstance *, int);
void Player_MoveByOffset(f32 *);
void Interact_OffsetViewRay(f32 *);
void Map_OffsetRecords(int, f32 *);
void Map_OffsetRecord(MapRecordBounds *, f32 *);
void Map_PlaceRecordModels(int);
void MapItem_MoveMap(int, f32 *);
void Script_Init(int);
int Script_Start(int, int);
void Player_StopFootstepSound(void);
void Sound_StopAll(int);
void Vector_Add(void *, void *, void *);
void Matrix_Multiply(void *, void *, void *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0MulMatrix(void *, void *, void *);
void Vector_Scale(float *, float *, float);
void Vector_Subtract(void *, void *, void *);
void GameLoop_RunSoundLoads(void);
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010A7E0(void *);
void func_0010B000(void);
void Map_DrawFreeCameraView(void);
float Angle_Wrap(float);
void Input_Update(void);
void Display_SwapBuffers(int);
void Vector_Normalize(void *, void *);
void Game_BlendScreenOffsetDown(void);
void func_0010A100(void *);
void func_0010B238(TextureTableHeader *);
void func_0010D308(ModelInstance *);
void func_00109F80(s64, s64, s64);
void func_0010A4E8(int, int, int, int, int);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);
int Map_TestFlagBlockBit(int, int, int, int);
void Vu0_ProjectToScreen3(int *, int *, int *, f32 *, f32 *, f32 *);
extern MapSlot map_slots[];
extern MapDrawState map_draw_state;
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern MapDrawRecord *D_00446778[2];
extern int D_00446770;
extern MapDrawRecord *D_00446780[2];
extern MapObject map_slot_objects[][0x200];
extern Entity entity_pool[][0x80];
extern MapSlotMemory D_004321F0[2];
extern int D_004467B0[2];
extern EntitySpawnInfo *D_004467D0[2];
extern MapItemDef *D_004469B8[2];
extern s16 *D_004469C0[2];
extern u8 *D_004469C8[2];
extern MapProp *map_slot_props[2][0xFF];
extern char map_object_defs[];
extern s32 D_004467B8[2];
extern MapLoadState map_load_state;
extern SoundLoadQueue sound_load_queue;
extern f32 D_00431AF0[16];
extern f32 D_004323E0[16];
extern AreaNode *D_00446788;
extern AreaNode *D_0044678C;
extern f32 D_00446768[]; /* the largest float */
extern f32 D_0044676C[]; /* the largest float */
extern GameStatus game_status;
extern u16 pad_state[];
extern PlayerState player_state;
extern MapSlotUnk10Words map_object_default_lights;
extern TextureTableHeader *D_00446790;
extern u8 D_004141D0[];

/* Draws the map records of both map slots whose flag bit 0 is clear (the list func_00112BD0 builds each
   frame), then each slot's map objects. mode 0 draws every record, the others only some; below 2 the
   drawing goes to the screen, at 2 to the off-screen buffer (Map_SetOffscreenTarget). */
void Map_DrawRecords(int mode) {
    MapSlot *slot;
    MapDrawRecord *record;
    MapSlotUnk08 *header;
    int map;
    int active;
    int flags;

    D_00432E20[0] = FOG_COLOR();
    D_00432E20[30] = FOG_COLOR();
    D_00433050[0] = FOG_COLOR();
    D_00433050[30] = FOG_COLOR();
    active = 0;
    if (map_slots[0].unk00 != 0) {
        active = map_slots[0].unk02 != 0;
    }
    if (map_slots[1].unk00 != 0) {
        if (map_slots[1].unk02 != 0) {
            active++;
        }
    }
    for (map = 0; map < 2; map++) {
        record = D_00446778[map];
        slot = &map_slots[map];
        if (slot->unk00 != 0) {
            Map_QueueSlotPacket(map, 0);
            Map_QueueSlotPacket(map, 1);
            Map_QueueSlotPacket(map, 5);
            func_00109E88();
            MapScroll_UpdateDraw(map, 0);
            MapScroll_UpdateCopy(map, 0);
            if (mode < 2) {
                func_0010A008();
            } else {
                Map_SetOffscreenTarget();
            }
            func_00109A50();
            func_00109A80(GS_REG_TEST_1, 0x5000D);
            func_00109A80(8, 0);
            func_00109AD0();
            func_00109E18();
            for (; record != NULL; record = record->next) {
                flags = record->flags;
                if ((flags & 2) && mode != 0) {
                    continue;
                }
                if ((flags & 0x200) && mode != 0) {
                    continue;
                }
                if (!(flags & 0x20) && mode != 0) {
                    continue;
                }
                if (active >= 2 && flags < 0 && map != D_00446770) {
                    continue;
                }
                if (flags & 0x10) {
                    record->model->unk148 = (s32)&map_draw_state.unkC0;
                    record->model->unk14C = &map_draw_state.unkA0;
                } else {
                    record->model->unk148 = (s32)&map_draw_state.unkA90[map][record->unk35];
                    record->model->unk14C = &map_draw_state.unk290[map][record->unk34];
                }
                LinkedPoint_FindHitsForMapDraw(record, record->model);
                if (record->flags & 0x200) {
                    header = slot->header;
                    func_00111368(record->model, header->unk78, header->unk79, header->unk7A, header->unk7B);
                }
                record->model->unk1A8(record->model);
                if (Map_ClassifyRecordOnScreen(record, record->model) == 0) {
                    func_0010EE98(record->model);
                }
                func_00109E18();
            }
            MapObject_DrawAll(mode, 1, map);
        }
    }
}

/* Draws the map records of both map slots whose flag bit 0 is set, after the entities and items, switching
   z writes on and off as the records ask (flag 0x80). mode as for Map_DrawRecords. */
void Map_DrawLateRecords(int mode) {
    MapSlot *slot;
    MapDrawRecord *record;
    MapSlotUnk08 *header;
    int map;
    int active;
    int flags;
    int z_mask;
    int old_z_mask;

    active = 0;
    if (map_slots[0].unk00 != 0) {
        active = map_slots[0].unk02 != 0;
    }
    if (map_slots[1].unk00 != 0) {
        if (map_slots[1].unk02 != 0) {
            active++;
        }
    }
    for (map = 0; map < 2; map++) {
        record = D_00446780[map];
        slot = &map_slots[map];
        if (slot->unk00 != 0) {
            Map_QueueSlotPacket(map, 2);
            Map_QueueSlotPacket(map, 5);
            func_00109E88();
            MapScroll_UpdateDraw(map, 1);
            MapScroll_UpdateCopy(map, 1);
            if (mode < 2) {
                func_0010A008();
            } else {
                Map_SetOffscreenTarget();
            }
            func_00109A50();
            z_mask = 1;
            func_00109A80(GS_REG_TEST_1, 0x5000D);
            func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
            func_00109AD0();
            for (; record != NULL; record = record->next) {
                flags = record->flags;
                if ((flags & 2) && mode != 0) {
                    continue;
                }
                if ((flags & 0x200) && mode != 0) {
                    continue;
                }
                if (!(flags & 0x20) && mode != 0) {
                    continue;
                }
                if (active >= 2 && flags < 0 && map != D_00446770) {
                    continue;
                }
                old_z_mask = z_mask;
                z_mask = !((flags >> 7) & 1);
                if (z_mask != old_z_mask) {
                    func_00109A50();
                    func_00109A80(GS_REG_ZBUF_1, ((s64)z_mask << 32) | 0xA000118);
                    func_00109AD0();
                }
                if (record->flags & 0x10) {
                    record->model->unk148 = (s32)&map_draw_state.unkC0;
                    record->model->unk14C = &map_draw_state.unkA0;
                } else {
                    record->model->unk148 = (s32)&map_draw_state.unkA90[map][record->unk35];
                    record->model->unk14C = &map_draw_state.unk290[map][record->unk34];
                }
                LinkedPoint_FindHitsForMapDraw(record, record->model);
                if (record->flags & 0x200) {
                    header = slot->header;
                    func_00111368(record->model, header->unk78, header->unk79, header->unk7A, header->unk7B);
                }
                record->model->unk1A8(record->model);
                if (Map_ClassifyRecordOnScreen(record, record->model) == 0) {
                    func_0010EE98(record->model);
                }
                func_00109E18();
            }
        }
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
}

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;
    u32 aligned = address;

    if (remainder != 0) {
        aligned = remainder - 0x10;
        aligned = address - aligned;
    }
    return aligned;
}

/* Finds the parts of a map file that was just read into a slot's memory, and shares out the
   slot's working memory. */
void Map_ParseFile(int map) {
    MapSlotMemory *memory = &D_004321F0[map];
    // match: set twice, the compiler no longer knows the slot is not the memory block, so the store
    // in the loop below makes it read memory->unk1C again afterwards
    MapSlot *slot = NULL;
    MapSlotUnk08 *header;
    char *cursor;
    char *work;
    u32 *packet;
    int i;
    int offset;
    int size;
    u32 address;

    slot = &map_slots[map];
    memset(map_slot_objects[map], 0, sizeof(map_slot_objects[map]));
    memset(entity_pool[map], 0, sizeof(entity_pool[map]));
    header = (MapSlotUnk08 *)memory->unk1C;
    work = memory->unk18;

    /* the working memory: the models, then the two tables of the slot */
    // match: stored as a model pointer (it is room for models); as a char * it is scheduled earlier
    *(ModelInstance **)&memory->unk34 = (ModelInstance *)work;
    work += header->record_count * 0x1C0;
    memory->unk3C = work;
    work += header->unk40 * 0x1C0;
    memory->unk44 = (ModelInstance *)work;
    work += header->unk42 * 0x1C0;
    memory->unk48 = work;
    work += header->unk3A * 0x1C0;
    slot->unk24 = work;
    work += header->unk2C * 4;
    slot->unk28 = work;
    work += header->unk2C;
    // match: stored as a u8 *; as a char * the read of header->unk4C below comes one instruction late
    *(u8 **)&memory->unk18 = (u8 *)work;

    /* the file: its fixed parts, then the tables whose sizes the header gives */
    cursor = (char *)header;
    slot->header = (MapHeader *)cursor;
    cursor = (char *)header + 0x80;
    slot->unk0C = (MapSlotUnk0C *)cursor;
    cursor = (char *)header + 0x480;
    slot->unk10 = cursor;
    cursor = (char *)header + 0x1480;
    slot->unk38 = cursor;
    cursor = (char *)header + 0x1540;
    slot->unk3C = cursor;
    cursor = (char *)header + 0x17C0;
    slot->records = cursor;
    cursor += header->record_count * 0x80;
    slot->flag_blocks = (MapFlagBlock *)cursor;
    cursor += header->unk3E * 0x40;
    slot->origins = (Vector4 *)cursor;
    cursor += 0x200;
    D_004467B0[map] = (int)cursor;
    cursor += header->unk40 * 0x80;
    D_004467D0[map] = (EntitySpawnInfo *)cursor;
    cursor += header->unk42 * 0x60;
    D_004469B8[map] = (MapItemDef *)cursor;
    cursor += header->unk44 * 0x40;
    D_004469C0[map] = (s16 *)cursor;
    cursor += 0x40;
    D_004469C8[map] = (u8 *)cursor;

    /* the parts the header gives offsets for */
    cursor = (char *)header;
    cursor += header->unk4C;
    slot->unk5C = (s32)cursor;
    offset = header->unk28;
    cursor = (char *)header;
    cursor += header->unk24;
    slot->unk30 = cursor;
    size = header->unk00;
    packet = (u32 *)cursor;
    for (i = 0; i < 6; i++) {
        // match: stored as a char *, the type memory->unk1C has (see the note on slot)
        ((char **)slot->unk40.items)[i] = (char *)packet;
        packet += (*packet + 1) * 4;
    }
    cursor = (char *)header;
    cursor += offset;
    slot->unk34 = (s32 *)cursor;
    /* two offsets in that part become addresses */
    ((char **)cursor)[1] += (s32)memory->unk1C;
    ((char **)cursor)[3] += (s32)memory->unk1C;
    address = (u32)memory->unk1C;
    address += size;
    memory->unk14 = (char *)align_16(address);
}


/* align_16 written another way; Map_BuildModels only matches with this one */
static inline u32 round_up_16(u32 address) {
    u32 remainder = address & 0xF;

    if (remainder != 0) {
        remainder -= 0x10;
        address -= remainder;
    }
    return address;
}

/* Builds the models of a map that was just read: one per record, then one per map object, with
   their collision meshes and motions. */
void Map_BuildModels(int map) {
    int cursor;
    char *work;
    int next;
    MapSlotMemory *memory = &D_004321F0[map];
    MapSlot *slot = &map_slots[map];
    MapSlotUnk08 *header = (MapSlotUnk08 *)memory->unk1C;
    int record_count;
    MapRecordBounds *record;
    MapRecordBounds *other;
    ModelInstance *model;
    // match: the three unused initial values below leave no code, but without them two saved values swap stack slots
    ModelInstance *first = NULL;
    MapObjectUnk08 *info = NULL;
    MapObject *object = NULL;
    int i;
    int j;
    int k;

    record_count = header->record_count;
    record = (MapRecordBounds *)slot->records;
    model = (ModelInstance *)memory->unk34;
    work = memory->unk18;
    work = (char *)round_up_16((u32)work);
    for (i = 0; i < record_count; i++, model++, record++) {
        Map_InsertRecordBySize((AreaNode *)record);
        MAP_RECORD_BITS(record)->map = (u8)map;
        record->model = model;
        if (record->unk30 != 0xFFFF) {
            cursor = (int)header + header->unk04;
            k = record->unk30;
            while (k-- > 0) {
                cursor += *(int *)cursor;
            }
            func_0010D018(model, (char **)&cursor, &work);
            if (record->unk3C & 0x100) {
                Vector_Copy(&model->position, record);
                model->position.w = 1.0f;
                ((void (*)(ModelInstance *, u32 *, int))func_00111E28)(model, (u32 *)&work, 0); // match: a third argument the callee does not take
            }
            if (((ModelInstance **)slot->unk24)[record->unk30] == NULL) {
                ((ModelInstance **)slot->unk24)[record->unk30] = model;
            }
            func_0010E6A8(model, 0x20, 0x20);
            if (record->unk3C & 0x202) {
                func_0010E6A8(model, 0x60, 0x60);
                func_0010E820(model, 0x48);
            }
        }
        if (record->unk32 != 0xFFFF) {
            other = (MapRecordBounds *)slot->records;
            for (j = 0; j < i; j++, other++) {
                if (record->unk32 == other->unk32) {
                    break;
                }
            }
            if (j >= i) {
                cursor = (int)header + header->unk08;
                k = record->unk32;
                while (k-- > 0) {
                    cursor += (int)((CollisionMesh *)cursor)->unk14;
                }
                cursor = (int)Collision_RelocateMesh((char *)cursor, model, 1);
            } else {
                model->strips = ((ModelInstance *)memory->unk34)[j].strips;
            }
        }
    }
    memory->unk18 = work;
    slot->unk02 = 0;
    memset(map_slot_props[map], 0, sizeof(map_slot_props[map]));
    record = (MapRecordBounds *)slot->records;
    for (i = 0; i < record_count; i++, record++) {
        if (((MapRecord *)record)->unk4E != 0) {
            if (map_slot_props[map][((MapRecord *)record)->unk4E] == NULL) {
                map_slot_props[map][((MapRecord *)record)->unk4E] = (MapProp *)record;
            }
        }
        if ((s32)record->unk3C < 0) {
            slot->unk02 = 1;
        }
    }
    D_004467B8[map] = header->unk40;
    object = map_slot_objects[map];
    work = memory->unk18;
    model = (ModelInstance *)memory->unk3C;
    info = *(MapObjectUnk08 **)&D_004467B0[map]; // match: read as a pointer; read as an int it waits for the stores above
    for (i = 0; i < header->unk40; i++, model++, info++, object++) {
        object->model = model;
        object->unk08 = info;
        object->unk04 = (MapObjectUnk04 *)&map_object_defs[info->unk30 * 0x80];
        object->unk0C = (s32)Map_FindRecordAtInSlot((CameraState *)info, 0.0f, map);
        MAP_OBJECT_BITS(object)->map = (u8)map;
    }
    cursor = (int)header + header->unk0C;
    for (i = 0; i < header->unk30; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && !(MAP_OBJECT_DEF_WORD(object) & 0x100) && object->unk08->unk32 == i) {
                next = cursor; // match: retail bug, only set when an object uses this model (see cursor = next)
                func_0010D018(object->model, (char **)&next, &work);
                object->model->unk190 = 1;
                object->model->unk194 = next;
                next += *(int *)next;
                object->model->unk19C = 0;
                object->model->unk1A0 = 0;
            }
        }
        cursor = next; // match: retail bug, next is stale (or never set) when no object uses this model
    }
    cursor = (int)header + header->unk10;
    for (i = 0; i < header->unk32; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x100) && object->unk08->unk32 == i) {
                next = cursor; // match: retail bug, as in the loop above
                func_0010B758(object->model, &next, (int *)&work);
                object->model->unk190 = 1;
                object->model->unk194 = next;
                next += *(int *)next;
                object->model->unk19C = (int)Map_GetSlotPacket(map, 3);
                object->model->unk1A0 = (int)Map_GetSlotPacket(map, 5);
            }
        }
        cursor = next; // match: retail bug, as in the loop above
    }
    cursor = (int)header + header->unk14;
    for (i = 0; i < header->unk34; i++) {
        next = cursor;
        object = map_slot_objects[map];
        first = NULL;
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk06 != 0xFFFF && object->unk08->unk34 == i) {
                if (first == NULL) {
                    cursor = (int)Collision_RelocateMesh((char *)next, object->model, 1);
                    first = object->model;
                } else {
                    object->model->strips = first->strips;
                }
            }
        }
    }
    cursor = (int)header + header->unk18;
    for (i = 0; i < header->unk36; i++) {
        object = map_slot_objects[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x200) && object->unk08->unk36 == i) {
                next = ((int (*)(int, int *, ModelInstance *, int))func_00108940)(cursor, (int *)&work, object->model, 1); // match: the callee returns where the motion ends
            }
        }
        cursor = next; // match: retail bug, as above: next is only set when an object uses this motion
    }
    memory->unk18 = work;
}

/* Makes the other map slot the current one: moves everything by the load's offset and switches
   the sound over. */
void Map_MakeCurrent(int map) {
    MapSlot *slot;
    SoundLoadQueue *queue;
    f32 *offset;
    MapSlot *loading;

    if (D_00446770 != map) {
        offset = &map_load_state.position.x;
        slot = &map_slots[map];
        queue = &sound_load_queue;
        D_00446770 = map;
        Player_MoveByOffset(offset);
        Interact_OffsetViewRay(offset);
        Map_OffsetRecords(0, offset);
        Map_OffsetRecords(1, offset);
        Map_PlaceRecordModels(0);
        Map_PlaceRecordModels(1);
        MapObject_OffsetAll(0, offset);
        MapObject_OffsetAll(1, offset);
        Entity_OffsetAllInSlot(0, offset);
        Entity_OffsetAllInSlot(1, offset);
        Effect_OffsetAll(offset);
        LinkedPoint_TransformFollows(offset);
        MapItem_MoveMap(0, offset);
        MapItem_MoveMap(1, offset);
        Gold_MoveMap(0, offset);
        Gold_MoveMap(1, offset);
        map_load_state.position.x = -map_load_state.position.x;
        map_load_state.position.y = -map_load_state.position.y;
        map_load_state.position.z = -map_load_state.position.z;
        Script_Init(map_slots[map].unk5C);
        Script_Start(0x7FFF, 0);
        loading = queue->unk10;
        if (loading != slot && queue->unk14 != slot) {
            Player_StopFootstepSound();
            Sound_StopAll(1);
            queue->unk14 = slot;
        } else if (loading == slot && sound_load_queue.unk14 != slot) {
            sound_load_queue.unk14 = NULL;
        }
        sound_load_queue.unk00 = ((MapSlotUnk08 *)map_slots[map].header)->unk70;
        sound_load_queue.unk04 = 1;
        MapScroll_CarryPairs(map == 0, map);
    }
}

/* Moves every record of a map and its model by an offset. */
void Map_OffsetRecords(int map, f32 *offset) {
    MapRecordBounds *record;
    u16 count;
    int remaining;
    MapSlot *slot;

    slot = &map_slots[map];
    record = (MapRecordBounds *)slot->records;
    if (record != NULL) {
        count = ((MapSlotUnk08 *)slot->header)->record_count;
        if (count != 0) {
            remaining = count;
            do {
                Map_OffsetRecord(record, offset);
                remaining--;
                record++;
            } while (remaining != 0);
        }
    }
}

/* Moves a map record and its model by an offset. */
void Map_OffsetRecord(MapRecordBounds *record, f32 *offset) {
    if (record != NULL) {
        Vector_Add(&record->model->position, &record->model->position, offset);
        Vector_Add(record, record, offset);
        Vector_Add(record->unk40, record->unk40, offset);
        Vector_Add(record->unk50, record->unk50, offset);
        Vector_Add(record->lowest, record->lowest, offset);
        Vector_Add(record->highest, record->highest, offset);
    }
}

/* Puts the model of every record of a map where the record says, and builds its matrices. */
void Map_PlaceRecordModels(int map) {
    MapSlot *slot = &map_slots[map];
    MapRecordBounds *record;
    ModelInstance *model;
    f32 (*matrix)[4];
    int i;
    int j;
    int count;

    record = (MapRecordBounds *)slot->records;
    if (record != NULL) {
        count = ((MapSlotUnk08 *)slot->header)->record_count;
        model = record->model;
        for (i = 0; i < count; i++, record++, model++) {
            Vector_Copy(&model->position, record->position);
            model->position.w = 1.0f;
            Vector_Copy(&model->rotation, record->rotation);
            model->rotation.w = 1.0f;
            Vector_Copy(&model->unkE0, record->scale);
            model->unkEC = 1.0f;
            matrix = (f32 (*)[4])model->unk40;
            sceVu0CopyMatrix(matrix, D_00431AF0);
            MODEL_MATRIX(model)[0][0] *= model->unkE0;
            MODEL_MATRIX(model)[1][1] *= model->unkE4;
            MODEL_MATRIX(model)[2][2] *= model->unkE8;
            sceVu0RotMatrixX((f32 *)matrix, (f32 *)matrix, model->rotation.x);
            sceVu0RotMatrixY((f32 *)matrix, (f32 *)matrix, model->rotation.y);
            sceVu0RotMatrixZ((f32 *)matrix, (f32 *)matrix, model->rotation.z);
            sceVu0TransMatrix((f32 *)matrix, (f32 *)matrix, &model->position.x);
            Matrix_Multiply(model, D_004323E0, matrix);
            for (j = 0; j < model->part_count; j++) {
                // match: the callee returns a value here, which keeps $2 busy
                ((int (*)(ModelInstance *, ModelPart *, ModelPart *))ModelMatrix_BuildPartLocal)(model, &model->parts[j], &model->parts[j]);
            }
            sceVu0CopyMatrix(model->unk80, matrix);
        }
    }
}

/* Links a map record into one of the two lists of record boxes (by its flag 0x80000000), keeping each
   sorted from the smallest box (x by z) up, so that a search finds the smallest box first. Records
   with flag 0x02000000 are left out. */
void Map_InsertRecordBySize(AreaNode *node) {
    AreaNode **head;
    AreaNode *prev;
    AreaNode *cur;
    f32 area;

    if (!(node->flags & 0x02000000)) {
        head = (node->flags & 0x80000000) ? &D_0044678C : &D_00446788;
        area = (node->right - node->left) * (node->bottom - node->top);
        prev = NULL;
        cur = *head;
        while (cur != NULL && !(area < (cur->right - cur->left) * (cur->bottom - cur->top))) {
            prev = cur;
            cur = cur->next;
        }
        if (prev != NULL) {
            prev->next = node;
        } else {
            *head = node;
        }
        node->next = cur;
    }
}

/* Takes the records of a map slot out of both lists of record boxes. */
void Map_UnlinkSlotRecords(int map) {
    AreaNode *prev;
    AreaNode *node;

    prev = NULL;
    node = D_00446788;
    while (node != NULL) {
        if (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                D_00446788 = node->next;
            }
        } else {
            prev = node;
        }
        node = node->next;
    }
    if (prev == NULL) {
        D_00446788 = NULL;
    }

    prev = NULL;
    node = D_0044678C;
    while (node != NULL) {
        if (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map) {
            if (prev != NULL) {
                prev->next = node->next;
            } else {
                D_0044678C = node->next;
            }
        } else {
            prev = node;
        }
        node = node->next;
    }
    if (prev == NULL) {
        D_0044678C = NULL;
    }
}

/* Returns the map record of the first list whose box holds the point (each side widened by margin).
   Failing that, the box nearest above or below it, then the nearest in z, then the nearest in x; NULL
   when none lines up with it. */
AreaNode *Map_FindRecordAt(f32 *point, f32 margin) {
    AreaNode *head = D_00446788;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return node;
        }
    }
    best_distance = D_00446768[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->unk44, node->unk54, point[1]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return best;
}

/* Map_FindRecordAt among the records of one map slot. */
MapRecordInfo *Map_FindRecordAtInSlot(CameraState *position, f32 margin, int map) {
    f32 *point = (f32 *)position;
    AreaNode *head = D_00446788;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return (MapRecordInfo *)node;
        }
    }
    best_distance = D_0044676C[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->unk44, node->unk54, point[1]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE_IN_MAP(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return (MapRecordInfo *)best;
}

/* Tells whether the point lies inside the node's box grown by margin on every side. */
int Map_IsPointInRecord(f32 *point, AreaNode *node, f32 margin) {
    int inside;

    if (node != NULL) {
        inside = FALSE;
        if (!(node->flags & 0x40000000) && node->left - margin <= point[0] && point[0] <= node->right + margin &&
            node->unk44 - margin <= point[1] && point[1] <= node->unk54 + margin &&
            node->top - margin <= point[2] && point[2] <= node->bottom + margin) {
            inside = TRUE;
        }
        return inside;
    }
    return FALSE;
}

/* Returns the first map record of the slot with id prop_id (MapRecord.unk4E), NULL when there is none or
   either is out of range. */
void *Map_GetRecordById(int slot, int prop_id) {
    if ((u32)slot < 2 && prop_id >= 0 && prop_id < 0x100) {
        return map_slot_props[slot][prop_id];
    }
    return NULL;
}

/* The free-camera map viewer: while GameStatus.bit7 is set it runs its own frame loop, in which the pad
   turns the camera about the player (with 0x2000 held) or moves it, and both 0x400 and 0x800 put it back. */
void Map_RunFreeCamera(void) {
    VuVector4 position;
    VuVector4 rotation;
    int reset_held;

    reset_held = FALSE;
    memset(&position, 0, sizeof(position));
    position.z = 10000.0f;
    rotation = (VuVector4){ -1.5707964f, player_state.rotation.y, 0.0f, 0.0f };
    if (game_status.bit7) {
        f32 world_screen[16];
        f32 view_screen[16];
        f32 world_view[16];
        f32 unused[16];

        sceVu0ViewScreenMatrix(view_screen, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
        do {
            VuVector4 move;

            Display_SwapBuffers(0);
            func_001096E8();
            func_001095E0();
            func_0010B000();
            Input_Update();
            move.x = move.y = move.z = move.w = 0.0f;
            if (pad_state[0] & 0x2000) {
                if (pad_state[0] & 0x40) {
                    rotation.x = Angle_Wrap(rotation.x + 0.08726646f);
                }
                if (pad_state[0] & 0x10) {
                    rotation.x = Angle_Wrap(rotation.x - 0.08726646f);
                }
                if (pad_state[0] & 0x80) {
                    rotation.y = Angle_Wrap(rotation.y + 0.08726646f);
                }
                if (pad_state[0] & 0x20) {
                    rotation.y = Angle_Wrap(rotation.y - 0.08726646f);
                }
            } else {
                if (pad_state[0] & 0x80) {
                    move.x = -1.0f;
                }
                if (pad_state[0] & 0x20) {
                    move.x += 1.0f;
                }
                if (pad_state[0] & 0x10) {
                    move.y -= 1.0f;
                }
                if (pad_state[0] & 0x40) {
                    move.y += 1.0f;
                }
            }
            if (!reset_held) {
                if ((*(s32 *)pad_state & 0xC00) == 0xC00) {
                    reset_held = TRUE;
                    position.z = 10000.0f;
                    rotation.x = -1.5707964f;
                    rotation.y = player_state.rotation.y;
                    position.x = 0.0f;
                    position.y = 0.0f;
                    position.w = 0.0f;
                    rotation.z = 0.0f;
                    rotation.w = 0.0f;
                } else if (pad_state[0] & 0x400) {
                    move.z += 1.0f;
                } else if (pad_state[0] & 0x800) {
                    move.z -= 1.0f;
                }
            } else if (!(*(s32 *)pad_state & 0xC00)) {
                reset_held = FALSE;
            }
            if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
                Vector_Normalize(&move, &move);
                Vector_Scale(&move.x, &move.x, 100.0f);
                Vector_Add(&position, &position, &move);
                if (position.z < 2500.0f) {
                    position.z = 2500.0f;
                }
                if (position.z > 50000.0f) {
                    position.z = 50000.0f;
                }
            }
            Vector_Scale(&move.x, &player_state.position.x, -1.0f);
            sceVu0TransMatrix(world_view, D_00431AF0, &move.x);
            sceVu0RotMatrixY(world_view, world_view, -rotation.y);
            sceVu0RotMatrixX(world_view, world_view, -rotation.x);
            sceVu0TransMatrix(world_view, world_view, &player_state.position.x);
            Vector_Subtract(&move, &player_state, &position);
            Vector_Scale(&move.x, &move.x, -1.0f);
            sceVu0TransMatrix(world_view, world_view, &move.x);
            sceVu0MulMatrix(world_screen, view_screen, world_view);
            func_0010A7E0(world_screen);
            func_001095D0();
            Map_DrawFreeCameraView();
            Message_Update();
            func_001095D8();
            GameLoop_RunSoundLoads();
            func_00109800();
        } while (!(pad_state[1] & 0x2000));
        game_status.bit7 = FALSE;
    }
}

/* Draws the view of Map_RunFreeCamera: every record of both map slots with one plain light block, then
   four quarter-screen rectangles on top. */
void Map_DrawFreeCameraView(void) {
    ScreenQuad quad;
    MapSlot *slot;
    MapDrawRecord *record;
    int map;
    int i;

    func_0010B238(D_00446790);
    func_00109E88();
    D_00432E20[0] = 0;
    D_00432E20[30] = 0;
    D_00433050[0] = 0;
    D_00433050[30] = 0;
    func_00109A50();
    func_00109A80(0x1A, 0);
    func_00109A80(0x1B, 0x248);
    func_00109A80(GS_REG_ALPHA_2, 0x4000000068L);
    func_00109A80(GS_REG_TEST_2, 0x3080B);
    func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
    func_00109AD0();
    func_00109E18();
    for (map = 0; map < 2; map++) {
        slot = &map_slots[map];
        record = (MapDrawRecord *)slot->records;
        if (slot->unk00 != 0) {
            for (i = 0; i < ((MapSlotUnk08 *)slot->header)->record_count; i++, record++) {
                if (!(record->flags & 0x80000000)) {
                    if (record->flags & 0x10) {
                        record->model->unk148 = (s32)&map_draw_state.unkC0;
                        record->model->unk14C = &map_draw_state.unkA0;
                    } else {
                        record->model->unk148 = (s32)&map_draw_state.unkA90[map][record->unk35];
                        record->model->unk14C = &map_draw_state.unk290[map][record->unk34];
                    }
                    record->model->hit_count = 0;
                    // match: both blocks chosen above are replaced at once by the viewer's own
                    record->model->unk148 = (s32)&map_object_default_lights;
                    record->model->unk14C = &map_draw_state.unkA0;
                    func_0010D308(record->model);
                }
            }
        }
    }
    Game_BlendScreenOffsetDown();
    func_00109A50();
    func_00109A80(0x1A, 1);
    func_00109A80(GS_REG_ZBUF_2, 0xA000118);
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    memset(&quad, 0, sizeof(quad));
    quad.tag = 0x4007A80621312E40L;
    quad.unk08 = 0x60;
    quad.unk10 = 0x156;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x40;
    quad.first_x = 0x6C00;
    quad.first_y = 0x7200;
    quad.first_z = 0xFFFFFFFF;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    quad.second_z = 0xFFFFFFFF;
    quad.second_u = 0x1000;
    quad.second_v = 0x1000;
    func_0010A100(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x7200;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    quad.first_x = 0x6C00;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    func_00109A50();
    func_00109A80(8, 0);
    func_00109AD0();
    func_00109E18();
}

/* Points both drawing contexts at the 256-pixel-wide 16-bit buffer at GS block 0x2BC0 with a 256x256
   scissor: draw mode 2 draws its second view there (GameLoop_DrawFrame). */
void Map_SetOffscreenTarget(void) {
    func_00109F80(0xA04015E, 0xFF000000FF0000L, 0x780000007800L);
}

/* Stretches 320x224 texels of the 16-bit buffer at GS block 0x2BC0 over the whole 640x448 screen, z
   writes off and with register 8 set to 5 meanwhile. Nothing calls it. */
void Map_StretchOffscreenToScreen(void) {
    ScreenQuad quad;

    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(8, 5);
    func_00109AD0();
    quad.tag = 0x624A16BC0L;
    quad.unk08 = 0x60;
    quad.unk10 = 0x116;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x80;
    quad.first_x = 0x6C00;
    quad.first_y = 0x7200;
    quad.second_x = 0x9400;
    quad.second_y = 0x8E00;
    quad.second_u = 0x1400;
    quad.second_v = 0xE00;
    quad.first_z = 0;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.second_z = 0;
    func_0010A100(&quad);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109A80(8, 0);
    func_00109AD0();
}

/* Covers the screen with the colour in bytes 0x78-0x7B of the current map's header, blended (draw mode 1
   does this between its two views). */
void Map_CoverScreenWithMapTint(void) {
    MapSlot *slot;
    MapSlotUnk08 *header;

    slot = &map_slots[D_00446770];
    header = (MapSlotUnk08 *)slot->header;
    func_0010A4E8(header->unk78, header->unk79, header->unk7A, header->unk7B, 0x44);
}

/* Queues prebuilt packet index of the map slot (MapSlot.unk40) for sending. */
void Map_QueueSlotPacket(int slot, int index) {
    MapSlot *map_slot = &map_slots[slot];
    u32 *packet_start;
    u32 *packet;

    packet_start = packet = func_00109990(3);
    *packet++ = *map_slot->unk40.items[index] | 0x30000000;
    *packet++ = (u32)map_slot->unk40.items[index] + 0x10;
    *packet++ = 0;
    *packet++ = 0;
    func_00109650(packet_start, packet);
}

/* Queues a packet for sending. */
void Map_QueuePacket(int *packet) {
    u32 *packet_start;
    u32 *cursor;

    if (packet != 0) {
        packet_start = cursor = func_00109990(2);
        *cursor++ = *packet | 0x30000000;
        *cursor++ = (u32)packet + 0x10;
        *cursor++ = 0;
        *cursor++ = 0;
        func_00109650(packet_start, cursor);
    }
}

/* Returns packet index of the map slot. */
u32 *Map_GetSlotPacket(int slot, int index) {
    MapSlot *map_slot = &map_slots[slot];
    MapSlotPackets *packets = &map_slot->unk40;

    if (packets != NULL) {
        return packets->items[index];
    }
    return NULL;
}

/* Tests flag bit of flag block index of the slot's map; for a thing of the other map (other_slot) looks for
   it among the block's 16 pairs of that map's tag and flag instead. Returns 1 when set. */
int Map_TestFlagBlockBit(int slot, int index, int other_slot, int bit) {
    MapFlagBlock *block = map_slots[slot].flag_blocks + index;
    int other_offset = other_slot * 0x60; /* byte offset: retail multiplies before the branch */
    int i;

    if (slot == other_slot) {
        return (block->bits[bit / 8] >> (bit - bit / 8 * 8)) & 1;
    }
    for (i = 0; i < 0x10; i++) {
        if (block->tags[i] == ((MapSlot *)((char *)map_slots + other_offset))->tag && block->values[i] == bit) {
            return 1;
        }
    }
    return 0;
}

/* Tests the corners of a map record's box against the screen: 1 when it has to be drawn whole (or the test is
   off), -1 when all eight corners are outside, 0 otherwise. The model is not used. */
int Map_ClassifyRecordOnScreen(MapDrawRecord *draw_record, ModelInstance *model) {
    MapRecord *record = (MapRecord *)draw_record;
    MapRecordBounds *bounds = (MapRecordBounds *)draw_record;
    f32 matrix[16];
    f32 corner0[4];
    f32 corner1[4];
    f32 corner2[4];
    int screen0[4];
    int screen1[4];
    int screen2[4];
    int outside;

    if (!MAP_DRAW_FLAG_BITS(&map_draw_state)->bit46) {
        return 1;
    }
    if (bounds->unk3C & 0x40) {
        return 1;
    }
    if (bounds->unk30 == 0xFFFF) {
        return 1;
    }
    if (Map_TestFlagBlockBit(D_00446770, D_004141D0[0], (u8)(record->unk38 >> 27) & 1, (u8)record->unk38) == 0) {
        return 1;
    }
    sceVu0MulMatrix(matrix, D_004323E0, D_00431AF0);
    Vu0_LoadMatrix(matrix);
    corner2[3] = 1.0f;
    corner1[3] = 1.0f;
    corner0[3] = 1.0f;
    corner0[0] = bounds->unk40[0];
    corner0[1] = bounds->unk40[1];
    corner0[2] = bounds->unk40[2];
    corner1[0] = bounds->unk40[0];
    corner1[1] = bounds->unk40[1];
    corner1[2] = bounds->unk50[2];
    corner2[0] = bounds->unk40[0];
    corner2[1] = bounds->unk50[1];
    corner2[2] = bounds->unk40[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside = (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->unk40[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk50[2];
    corner1[0] = bounds->unk50[0];
    corner1[1] = bounds->unk40[1];
    corner1[2] = bounds->unk40[2];
    corner2[0] = bounds->unk50[0];
    corner2[1] = bounds->unk40[1];
    corner2[2] = bounds->unk50[2];
    Vu0_ProjectToScreen3(screen0, screen1, screen2, corner0, corner1, corner2);
    outside += (u32)screen0[2] > 0xFFFF0;
    outside += (u32)screen1[2] > 0xFFFF0;
    outside += (u32)screen2[2] > 0xFFFF0;
    corner0[0] = bounds->unk50[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk40[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    corner0[0] = bounds->unk50[0];
    corner0[1] = bounds->unk50[1];
    corner0[2] = bounds->unk50[2];
    Vu0_ProjectToScreen(screen0, corner0);
    outside += (u32)screen0[2] > 0xFFFF0;
    if (outside == 0) {
        return 1;
    }
    if (outside == 8) {
        return -1;
    }
    return 0;
}
