#include "effect/effect.h"
#include "core/file.h"
#include "gfx/gs.h"
#include "core/game.h"
#include "model/linked_point.h"
#include "map/map.h"
#include "core/memory.h"
#include "player/player.h"
#include "gfx/screen.h"

void *memset(void *, int, u32);
int printf(const char *, ...);
int FlushCache(int);
int sceGsSyncPath(int, int);
void sceGsSetDefStoreImage(void *, s16, s16, s16, s16, s16, s16, s16);
void sceGsExecStoreImage(void *, void *);
char *Texture_UploadTable(char *base, int upload);
void Texture_SendTexFlush(void);
void Vector_Copy(void *dst, void *src);
void Vector_Add(void *, void *, void *);
void Packet_BeginGsRegisters(void);
void Packet_AddGsRegister(int reg, s64 value);
void Packet_EndGsRegisters(void);
void GsDraw_QueueFlush(void);
void ModelTexture_SendForModel(void *model_arg);
u32 *Packet_Alloc(int);
void Packet_AppendToChain(u32 *, u32 *);
void GsDraw_QueueTexFlush(void);
int Map_IsPointInRecord(f32 *point, AreaNode *node, f32 margin);
AreaNode *Map_FindRecordAt(f32 *point, f32 margin);
void ModelMatrix_Build(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void ModelMatrix_BuildPartLocal(ModelInstance *, ModelPart *, ModelPart *);
void DrawList_Add(ModelInstance *, s32, s32);
char Sound_PlayTrackedKind(int index, int first, int second, int third, int fourth);
void Sound_StopTracked(s8 handle);
void Model_SetUpPosed(ModelInstance *, char **, char **);
int Wobble_ModelUsesTextures(ModelInstance *model, int first, int last);
u32 *Map_GetSlotPacket(int, int);

extern GameStatus game_status;
extern MapSlot map_slots[2];
extern char map_draw_slot_blocks[];
extern MapObject map_slot_objects[][0x200];
extern Entity entity_pool[][0x80];
extern Entity entity_pool_last;           /* the last entity of entity_pool */
extern void (*effect_draw_hooks[4][4])(ModelInstance *);
extern void (*effect_kind_handlers[][2])();
extern FxTableEntry fx_table[];
extern PlayerState player_state;
extern Vector4 origin_point;
extern int current_map_slot;
extern char *effect_area;

static EffectSlot effect_slots[EFFECT_SLOT_COUNT] = { 0 }; /* 0x3F1750 the spawner ("SFXGEN") pool */
EffectActor effect_pool[EFFECT_COUNT] = { 0 };              /* 0x3F3550 the effect ("SFX") pool */

int effect_sound_count = 0;                 /* 0x4468C8 live effects that hold a sound slot */
static int effect_slot_count = 0;   /* 0x4468CC live spawner slots */
static int effect_count = 0;        /* 0x4468D0 live effects */

static u32 *effect_texture_packet;  /* 0x4470F8 DMA packet holding the effect textures read back from the GS */
static char *effect_model_file;     /* 0x4470FC sfx/sfx.dat */
static char *effect_model_memory;   /* 0x447100 0x800 bytes of model memory per effect */
static char **effect_model_table;   /* 0x447104 the model of each EffectDef.model_index, inside effect_model_file */

/* 0x502260 param/sfx.prm; entries is the view whose bit-field tests stay array reads */
static union {
    EffectDef defs[EFFECT_DEF_COUNT];
    EffectTableEntry entries[EFFECT_DEF_COUNT];
} effect_table;
static ModelInstance effect_models[EFFECT_COUNT];    /* 0x54A260 one model per effect */
static Vector4 effect_light_block[8];                /* 0x574260 */

static void Effect_CaptureTextures(void);
static void Effect_RunSpawner(EffectSlot *slot);
static void Effect_SetupModel(int index);
static void Effect_FindModelRecord(ModelInstance *model, int unused_arg1, int record_id, int id_bits);

/* Clears the effect pools, lays out the effect buffers, loads the three effect files and
   turns the angles of the effect definitions from degrees into radians. */
void Effect_Init(void) {
    char unused_buffer[0x2C0];
    char *buffer;
    int handle;
    char **table;
    int i;
    int key;
    EffectDef *def;
    EffectDef *next_def;
    Vector4 *keys;
    f32 scale; /* degrees to radians */

    effect_count = 0;
    effect_slot_count = 0;
    memset(effect_pool, 0, sizeof(effect_pool));
    memset(effect_slots, 0, sizeof(effect_slots));
    buffer = effect_area;
    effect_texture_packet = (u32 *)buffer;
    buffer += 0x70400;
    effect_model_memory = buffer;
    buffer += 0xC0000;
    effect_model_file = buffer;
    buffer += 0x9FC00;
    effect_model_table = (char **)effect_model_file;
    memset(effect_light_block, 0, sizeof(effect_light_block));
    effect_light_block[7].z = 1.0f;
    effect_light_block[7].y = 1.0f;
    effect_light_block[7].x = 1.0f;
    effect_light_block[7].w = 1.0f;
    handle = File_Open("param/sfx.prm", 0, 1);
    File_Read(handle, effect_table.defs, -1);
    File_Close(handle);
    handle = File_Open("sfx/sfx.tmx", 0, 1);
    File_Read(handle, buffer, -1);
    File_Close(handle);
    Texture_UploadTable(buffer, 1);
    Texture_SendTexFlush();
    handle = File_Open("sfx/sfx.dat", 0, 1);
    File_Read(handle, effect_model_file, -1);
    File_Close(handle);
    table = effect_model_table;
    for (i = 0; i < 0x200; i++) {
        table[i] += (int)table; /* file offsets become pointers */
    }
    Effect_CaptureTextures();
    scale = 0.017453292f;
    def = effect_table.defs;
    for (i = 0; i < EFFECT_DEF_COUNT; i++, def = next_def) {
        next_def = def + 1; // match: the next entry is worked out first
        keys = def->keys_a;
        for (key = 0; key < 4; key++, keys++) {
            keys[4].x *= scale; // match: keys_b, reached from keys_a so that one pointer walks it
            keys[4].y *= scale;
            keys[4].z *= scale;
        }
        def->rotation_base[0] *= scale;
        def->rotation_base[1] *= scale;
        def->rotation_base[2] *= scale;
        def->rotation_spread[0] *= scale;
        def->rotation_spread[1] *= scale;
        def->rotation_spread[2] *= scale;
        if (def->texture_record_base != 0) {
            def->texture_record_base += 0x1400;
        }
    }
}

/* Builds the texture packet's DMA tags, then reads the effect textures back from the GS into it. */
static void Effect_CaptureTextures(void) {
    char store_image[0x70];
    u32 *packet;

    packet = effect_texture_packet;
    *packet++ = 0x7007;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x51007006;
    *(u64 *)packet = 0x1000000000000004;
    packet += 2;
    *(u64 *)packet = 0xE;
    packet += 2;
    *(u64 *)packet = 0x8B90L << 34;
    packet += 2;
    *packet++ = 0x50;
    *packet++ = 0;
    *(u64 *)packet = 0;
    packet += 2;
    *packet++ = 0x51;
    *packet++ = 0;
    *(u64 *)packet = 0x38000000080;
    packet += 2;
    *packet++ = 0x52;
    *packet++ = 0;
    *(u64 *)packet = 0;
    packet += 2;
    *packet++ = 0x53;
    *packet++ = 0;
    *(u64 *)packet = 0x080000000000F000;
    packet += 2;
    *(u64 *)packet = 0;
    packet += 2;
    sceGsSetDefStoreImage(store_image, 0x2E40, 2, 0, 0, 0, 0x80, 0x380);
    FlushCache(0);
    sceGsExecStoreImage(store_image, packet);
    sceGsSyncPath(0, 0);
}

/* Debug: prints and frees every spawner slot and effect the player does not own. */
void Effect_DebugFreeAllButPlayer(void) {
    EffectSlot *slot = effect_slots;
    EffectActor *effect = effect_pool;
    int i;

    for (i = 0; i < EFFECT_SLOT_COUNT; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            printf("SFXGEN[%d] OWNER[0x%x]\n", i, slot->owner_entity);
            if (!(slot->flags & EFFECT_SLOT_LIVE)) {
                continue;
            }
            if (slot->owner_entity == (Entity *)&player_state) {
                continue;
            }
            Effect_FreeSpawner(slot);
            if (effect_slot_count <= 0) {
                break;
            }
        }
    }

    for (i = 0; i < EFFECT_COUNT; i++, effect = (EffectActor *)((char *)effect + sizeof(EffectActor))) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            printf("SFX[%d] OWNER[0x%x]\n", i, effect->owner_entity);
            if (!(effect->flags & EFFECT_FLAG_LIVE)) {
                continue;
            }
            if (effect->owner_entity == (Entity *)&player_state) {
                continue;
            }
            Effect_Free(effect);
            if (effect_count <= 0) {
                break;
            }
        }
    }
}

/* Transforms the positions of the live spawner slots and effects by a matrix. */
void Effect_TransformAll(void *transform) {
    s64 slot_in_use_bit;
    s64 actor_in_use_bit;
    EffectSlot *slot;
    EffectActor *effect;
    Vector4 *model_position;
    int pool_index;
    int live_left;

    slot_in_use_bit = EFFECT_SLOT_LIVE;
    slot = effect_slots;
    effect = effect_pool;
    live_left = effect_slot_count;
    for (pool_index = 0; pool_index < EFFECT_SLOT_COUNT; pool_index++, slot++) {
        if (slot->flags & slot_in_use_bit) {
            Vector_Add(&slot->position, transform, &slot->position);
            live_left--;
            if (live_left <= 0) {
                break;
            }
        }
    }

    actor_in_use_bit = EFFECT_FLAG_LIVE;
    live_left = effect_count;
    for (pool_index = 0; pool_index < EFFECT_COUNT; pool_index++, effect++) {
        if (effect->flags & actor_in_use_bit) {
            live_left--;
            model_position = &effect->model->position;
            Vector_Add(model_position, transform, model_position);
            Vector_Add(&effect->unk70, transform, &effect->unk70);
            if (live_left <= 0) {
                break;
            }
        }
    }
}

/* Takes a free spawner slot for effect_id at position and rotation; returns its index, or -1. */
int Effect_StartSpawner(int effect_id, void *position, void *rotation, void *owner_entity) {
    EffectSlot unused_slot;
    EffectDef *def;
    EffectSlot *slot;
    int slot_index;
    u64 placement;

    if ((u32)effect_id >= EFFECT_DEF_COUNT) {
        return -1;
    }
    def = &effect_table.defs[effect_id];
    if (def->model_index >= 0x200) {
        return -1;
    }
    if (effect_slot_count >= EFFECT_SLOT_COUNT) {
        return -1;
    }
    slot = effect_slots;
    for (slot_index = 0; slot_index < EFFECT_SLOT_COUNT; slot_index++, slot++) {
        if (!(slot->flags & EFFECT_SLOT_LIVE)) {
            break;
        }
    }
    if (slot_index >= EFFECT_SLOT_COUNT) {
        return -1;
    }
    slot->flags |= EFFECT_SLOT_LIVE;
    effect_slot_count++;
    EFFECT_SLOT_FLAG_PARTS(slot)->level = def->max_level;
    EFFECT_SLOT_FLAG_PARTS(slot)->unk27 = def->spawn_start_delay;
    slot->owner_entity = owner_entity;
    EFFECT_SLOT_FLAG_PARTS(slot)->def = def;
    EFFECT_SLOT_FLAG_PARTS(slot)->unk26 = 0;
    slot->owner_effect = NULL;
    slot->unk30 = NULL;
    slot->unk34 = NULL;
    placement = EFFECT_DEF_FLAG_WORD(def) & 0x600000;
    if (placement == 0 || placement == 0x600000) {
        Vector_Copy(&slot->position, position);
        Vector_Copy(&slot->unk10, rotation);
        if (EFFECT_DEF_FLAGS(def) & 0x10000) {
            slot->unk30 = position;
            slot->unk34 = rotation;
        }
    } else {
        Vector_Copy(&slot->position, &origin_point);
        Vector_Copy(&slot->unk10, &origin_point);
        if (EFFECT_DEF_FLAGS(def) & 0x10000) {
            slot->unk34 = slot->unk30 = &origin_point;
        }
    }
    slot->flags &= ~(EFFECT_SLOT_LIVE << 1);
    slot->effect_param = 0xFFFF;
    return slot_index;
}

/* Spawns effect effect_id at position and rotation; returns its slot, or -1 when none is free. */
int Effect_Spawn(int effect_id, Vector4 *position, Vector4 *rotation, Entity *owner_entity, u16 effect_param, void *owner) {
    int slot_index;
    EffectSlot *slot;

    slot_index = Effect_StartSpawner(effect_id, position, rotation, owner_entity);
    if (slot_index != -1) {
        slot = &effect_slots[slot_index];
        slot->effect_param = effect_param;
        slot->owner = owner;
    }
    return slot_index;
}

/* Runs the spawner slots and the effects for one frame. While the game status has bit 0x400
   set, only those of the owner in unk40 and of the kinds 0x280, 0x282 and 0x284 run. */
void Effect_UpdateAll(void) {
    Vector4 unused[2]; /* unused, but retail reserves the bytes */
    EffectDef *kind_280 = &effect_table.defs[0x280];
    EffectDef *kind_282 = &effect_table.defs[0x282];
    EffectDef *kind_284 = &effect_table.defs[0x284];
    Entity *only_owner = NULL;
    EffectSlot *slot;
    EffectActor *effect;
    int i;
    int key;

    if (game_status.update_hold_flags & 0x400) {
        only_owner = game_status.effect_owner;
    }
    if (effect_slot_count > 0) {
        slot = effect_slots;
        for (i = 0; i < EFFECT_SLOT_COUNT; i++, slot++) {
            if (slot->flags & EFFECT_SLOT_LIVE) {
                if (only_owner == NULL || only_owner == slot->owner_entity || EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_280 ||
                    EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_282 || EFFECT_SLOT_FLAG_PARTS(slot)->def == kind_284) {
                    Effect_RunSpawner(slot);
                    if (!effect_slots) { // match: tests the pool's address, which is never null
                        break;
                    }
                }
            }
        }
    }
    effect = effect_pool;
    for (i = 0; i < EFFECT_COUNT; i++, effect++) {
        if (!(effect->flags & EFFECT_FLAG_LIVE)) {
            if (effect->unk90 > 0) {
                effect->unk90--;
            }
        } else if (only_owner == NULL || only_owner == effect->owner_entity || effect->def == kind_280 ||
                   effect->def == kind_282 || effect->def == kind_284) {
            Vector_Copy(&effect->unk70, &effect->model->position);
        }
    }
    effect = effect_pool;
    for (i = 0; i < EFFECT_COUNT; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (only_owner == NULL || only_owner == effect->owner_entity || effect->def == kind_280 || effect->def == kind_282 || effect->def == kind_284) {
                if (EFFECT_FLAG_PARTS(effect)->unk40 > 0) {
                    EFFECT_FLAG_PARTS(effect)->unk40--;
                }
                for (key = 0; key < 3; key++) {
                    if (effect->def->frame_spawn_frames[key] == effect->frame) {
                        Effect_SpawnFromEffect(effect->def->frame_spawn_effects[key], effect);
                    }
                }
                effect_kind_handlers[effect->def->kind_index][1](effect);
            }
        }
    }
}

/* Sends the texture packet, putting the effect textures back into the GS. */
void Effect_UploadTextures(void) {
    u32 *first_word;
    u32 *cursor;

    GsDraw_QueueFlush();
    first_word = cursor = Packet_Alloc(2);
    *cursor++ = effect_texture_packet[0] | 0x30000000;
    *cursor++ = (u32)(effect_texture_packet + 4);
    *cursor++ = 0;
    *cursor = 0;
    Packet_AppendToChain(first_word, cursor + 1);
    GsDraw_QueueTexFlush();
}

/* Draws the live effects for pass 0 or 1: finds the map record each is in, picks its texture
   frame, places its parts and sends the model. In pass 0 it also frees the effects whose
   frame has reached keyframe_end. */
void Effect_DrawAll(int pass) {
    Vector4 unused[3]; /* unused, but retail reserves the bytes */
    EffectActor *effect;
    ModelInstance *model;
    ModelPart *part;
    s64 placement;
    int i;
    int part_index;
    int step;
    int record_id;

    if (effect_count == 0) {
        return;
    }
    Effect_UploadTextures();
    effect = effect_pool;
    for (i = 0; i < EFFECT_COUNT; i++, effect++) {
        if (!(effect->flags & EFFECT_FLAG_LIVE)) {
            continue;
        }
        placement = EFFECT_DEF_FLAGS(effect->def) & 0x600000;
        model = effect->model;
        if (placement == 0 || placement == 0x600000) {
            // match: the margin is set up before the node, so the call passes it first
            if (((int (*)(f32 *, f32, AreaNode *))Map_IsPointInRecord)((f32 *)&model->position, 0.0f, (AreaNode *)effect->unk58) == 0) {
                effect->unk58 = (char *)Map_FindRecordAt((f32 *)&effect->model->position, 0.0f);
            }
            if (effect->unk58 == NULL && effect->owner_entity == (Entity *)&player_state && ((PlayerState *)effect->owner_entity)->map_index != 0xFFFF) {
                effect->unk58 = map_slots[current_map_slot].records + (((PlayerState *)effect->owner_entity)->map_index << 7);
            }
            if (pass == 0 && effect->unk58 == NULL) {
                if (effect->frame >= effect->def->keyframe_end) {
                    Effect_Free(effect);
                }
                continue;
            }
        }
        if (effect->def->texture_frame_count != 0) {
            step = (effect->frame + 1) / (effect->def->texture_frame_interval + 1);
            switch (effect->def->texture_anim_mode) {
            case 0:
                record_id = step % effect->def->texture_frame_count;
                break;
            case 1:
                // match: past the last frame record_id keeps whatever the effect before left in it
                if (step < effect->def->texture_frame_count) {
                    record_id = step;
                }
                break;
            case 2:
                record_id = effect->def->texture_frame_count - 1;
                if (step < effect->def->texture_frame_count) {
                    record_id = step;
                }
                break;
            default:
                record_id = 0;
                break;
            }
            record_id = effect->def->texture_record_base + record_id * effect->def->texture_frame_stride;
            Effect_FindModelRecord(model, model->unk104, effect->def->texture_record_base, record_id);
        }
        Vector_Copy(&model->colour_red, effect);
        // match: it returns a value
        ((int (*)(ModelInstance *, Vector4 *, Vector4 *, f32 *))ModelMatrix_Build)(model, &model->position, &model->rotation, &model->scale_x);
        part = effect->model->parts;
        for (part_index = 0; part_index < model->part_count; part_index++, part++) {
            ModelMatrix_BuildPartLocal(model, part, part);
        }
        model->fog_block = map_draw_slot_blocks + (current_map_slot << 10);
        // match: the flag is read through the union so the test stays behind the unk14C store
        if (!(((EntityFlagWord *)&effect->def->sub_effect_id)->word & (0x8000L << 25))) {
            model->light_block = (s32)effect_light_block;
        } else {
            model->light_block = (s32)(map_draw_slot_blocks - 0x1D0);
        }
        model->hit_count = 0;
        if (pass == 0 || !(EFFECT_DEF_WORD_F0(effect->def) & (0x8000L << 26))) {
            DrawList_Add(model, ((EffectTableEntry *)effect->def)->placement, pass);
        }
        if (pass == 0 && effect->frame >= effect->def->keyframe_end) {
            Effect_Free(effect);
        }
    }
}

/* Runs a spawner slot for one frame: after its start delay, every unkA7 frames, it makes unkA5
   effects of its definition and counts its level down, freeing itself at 0. */
static void Effect_RunSpawner(EffectSlot *slot) {
    EffectDef *def;
    EffectActor *effect;
    int index;
    int made;
    s64 placement;

    if (EFFECT_SLOT_VIEW(slot)->unk27 != 0) {
        EFFECT_SLOT_VIEW(slot)->unk27--;
        return;
    }
    def = EFFECT_SLOT_VIEW(slot)->def;
    if (def->spawn_interval != 0) {
        u8 timer = EFFECT_SLOT_VIEW(slot)->unk26--;

        if (timer != 0) {
            return;
        }
        EFFECT_SLOT_VIEW(slot)->unk26 = def->spawn_interval;
    }
    if (effect_count >= EFFECT_COUNT) {
        if (--EFFECT_SLOT_VIEW(slot)->level == 0) {
            Effect_FreeSpawner(slot);
        }
        return;
    }
    for (made = 0; made < def->spawn_batch_count; made++) {
        effect = effect_pool;
        for (index = 0; index < EFFECT_COUNT; index++, effect++) {
            if (!(effect->flags & EFFECT_FLAG_LIVE) && effect->unk90 == 0) {
                break;
            }
        }
        if (index >= EFFECT_COUNT) {
            break;
        }
        effect_count++;
        Memory_ClearWords(effect, sizeof(EffectActor) / 4);
        EFFECT_FLAG_BITS(effect)->live = TRUE;
        effect->frame = 0;
        effect->start_position = slot->unk30;
        effect->model = &effect_models[index];
        effect->def = EFFECT_SLOT_VIEW(slot)->def;
        effect->owner_entity = slot->owner_entity;
        effect->target = slot->owner_effect;
        effect->effect_param = slot->effect_param;
        effect->owner = slot->owner;
        effect->start_rotation = EFFECT_SLOT_VIEW(slot)->start_rotation;
        EFFECT_FLAG_PARTS(effect)->unk44 = -1;
        effect->unk90 = 4; /* frames the actor stays reserved once it is freed */
        effect->unk6C = 0;
        Effect_SetupModel(index);
        effect_kind_handlers[effect->def->kind_index][0](effect, slot);
        EFFECT_FLAG_BITS(effect)->bit59 = effect->def->sound_id != 0;
        if (fx_table[effect->effect_param].item_menu_magic != 0) {
            EFFECT_FLAG_PARTS(effect)->unk46 = Sound_PlayTrackedKind(EFFECT_FLAG_BITS(effect)->bit59, effect->def->sound_id, 0, (int)&player_state, EFFECT_FLAG_PARTS(effect)->unk46);
        } else if (EFFECT_FLAG_BITS(effect)->bit59 || effect->effect_param == player_state.equipped_magic) {
            EFFECT_FLAG_PARTS(effect)->unk46 = Sound_PlayTrackedKind(EFFECT_FLAG_BITS(effect)->bit59, effect->def->sound_id, 0, (int)&effect->model->position, EFFECT_FLAG_PARTS(effect)->unk46);
        }
        if (EFFECT_FLAG_PARTS(effect)->unk46 > 0) {
            effect_sound_count++;
        }
        placement = EFFECT_DEF_FLAGS(effect->def) & 0x600000;
        if (placement == 0 || placement == 0x600000) {
            effect->unk58 = (char *)Map_FindRecordAt((f32 *)&effect->model->position, 0.0f);
        }
        def = EFFECT_SLOT_VIEW(slot)->def;
    }
    if (--EFFECT_SLOT_VIEW(slot)->level == 0) {
        Effect_FreeSpawner(slot);
    }
}

/* Starts a spawner slot for effect_id at the effect's own model and links it to the effect. */
int Effect_SpawnFromEffect(int effect_id, EffectActor *effect) {
    int slot_index;
    EffectSlot *slot;

    slot_index = Effect_StartSpawner(effect_id, &effect->model->position, &effect->model->rotation, effect->owner_entity);
    if (slot_index != -1) {
        slot = &effect_slots[slot_index];
        slot->effect_param = effect->effect_param;
        slot->owner = effect->owner;
        slot->owner_effect = effect;
        EFFECT_SLOT_FLAG_BITS(slot)->bit33 = EFFECT_FLAG_BITS(effect)->bit59;
        if (effect_table.entries[effect_id].track) {
            EFFECT_FLAG_BITS(effect)->has_slots = TRUE;
        }
    }
    return slot_index;
}

/* Starts a spawner slot for effect_id at position, turned like the effect, and links it to the effect. */
int Effect_SpawnFromEffectAt(int effect_id, EffectActor *effect, Vector4 *position) {
    int slot_index;
    EffectSlot *slot;

    slot_index = Effect_StartSpawner(effect_id, position, &effect->model->rotation, effect->owner_entity);
    if (slot_index != -1) {
        slot = &effect_slots[slot_index];
        slot->effect_param = effect->effect_param;
        slot->owner = effect->owner;
        slot->owner_effect = effect;
        EFFECT_SLOT_FLAG_BITS(slot)->bit33 = EFFECT_FLAG_BITS(effect)->bit59;
        if (effect_table.entries[effect_id].track) {
            EFFECT_FLAG_BITS(effect)->has_slots = TRUE;
        }
    }
    return slot_index;
}

/* Frees an effect, then the spawner slots and effects that it owns or targets. */
void Effect_Free(EffectActor *effect) {
    EFFECT_FLAG_BITS(effect)->live = 0;
    effect->unk90 = 4;
    effect_count--;
    if (EFFECT_FLAG_PARTS(effect)->unk46 > 0) {
        effect_sound_count--;
    }
    Sound_StopTracked(EFFECT_FLAG_PARTS(effect)->unk46);
    EFFECT_FLAG_PARTS(effect)->unk46 = -1;
    if (EFFECT_FLAG_PARTS(effect)->unk44 != -1) {
        LinkedPoint_ReleaseIndex(EFFECT_FLAG_PARTS(effect)->unk44);
    }
    if (effect->flags & EFFECT_FLAG_HAS_SLOTS) {
        Effect_FreeSpawnersOf(effect);
    }
    if (effect->flags & EFFECT_FLAG_HAS_TARGETERS) {
        Effect_FreeTargeting(effect);
    }
}

/* Frees a spawner slot: clears its live flag and counts one slot down. */
void Effect_FreeSpawner(EffectSlot *slot) {
    EFFECT_SLOT_FLAG_BITS(slot)->live = 0;
    effect_slot_count--;
}

/* Frees every effect that targets the given effect. */
void Effect_FreeTargeting(EffectActor *target) {
    EffectActor *effect;
    int i;

    if (effect_count > 0) {
        effect = effect_pool;
        for (i = 0; i < EFFECT_COUNT; i++, effect++) {
            if (effect->flags & EFFECT_FLAG_LIVE) {
                if (effect->target == target) {
                    Effect_Free(effect);
                    if (effect_count == 0) {
                        break;
                    }
                }
            }
        }
    }
}

/* Frees every spawner slot that the given effect spawned. */
void Effect_FreeSpawnersOf(EffectActor *owner) {
    EffectSlot *slot;
    int i;

    if (effect_slot_count > 0) {
        i = 0;
        slot = effect_slots;
        for (; i < EFFECT_SLOT_COUNT; i++, slot++) {
            if ((slot->flags & EFFECT_SLOT_LIVE) && slot->owner_effect == owner) {
                Effect_FreeSpawner(slot);
                if (effect_slot_count == 0) {
                    break;
                }
            }
        }
    }
}

/* Sets up the model of effect number index from its definition's model file. */
static void Effect_SetupModel(int index) {
    char *file_cursor;
    char *memory;
    EffectActor *effect = &effect_pool[index];
    ModelInstance *model = effect->model;
    EffectDef *def;
    int map;

    Memory_ClearWords(model, sizeof(ModelInstance) / 4);
    def = effect->def;
    if (EFFECT_DEF_FLAGS(def) & 0x20000) {
        if ((EFFECT_DEF_FLAGS(def) & 0x600000) == 0 || (EFFECT_DEF_FLAGS(def) & 0x600000) == 0x600000) {
            model->draw_flags |= 0x300;
        } else {
            model->draw_flags |= 0x400;
        }
    }
    model->model_flags |= 0x1800;
    file_cursor = effect_model_table[def->model_index];
    memory = effect_model_memory + (index << 11);
    Model_SetUpPosed(model, &file_cursor, &memory);
    model->draw = effect_draw_hooks[((EffectTableEntry *)effect->def)->placement][effect->def->draw_hook_index];
    model->model_flags |= 0x800;
    if (Wobble_ModelUsesTextures(model, 0x3BC0, 0x3C3F) != 0) {
        // match: the owner is read from the effect again for every test (a macro in the game, most likely)
        if (effect->owner_entity == (Entity *)&map_slots[0]) {
            map = 0; /* the owner is a map slot itself */
        } else if (effect->owner_entity == (Entity *)&map_slots[1]) {
            map = 1;
        } else if ((MapObject *)effect->owner_entity >= map_slot_objects[0] && (MapObject *)effect->owner_entity <= &map_slot_objects[1][0x1FF]) {
            map = (u8)((MapObject *)effect->owner_entity)->state_flags & 1;
        } else if (effect->owner_entity >= entity_pool[0] && effect->owner_entity <= &entity_pool_last) {
            map = effect->owner_entity->group_id; /* an entity: the map it is in */
        } else {
            map = current_map_slot;
        }
        effect->model->texture_slot = 3;
        effect->model->texture_data = (int)Map_GetSlotPacket(map, 4);
        effect->model->shared_texture_slot = 0;
        effect->model->shared_texture_packet = (int)Map_GetSlotPacket(map, 5);
    }
}

/* Finds the record with the given id in a model's parts and copies it to model->found_record. */
// match: the caller passes a second argument that this function never reads
static void Effect_FindModelRecord(ModelInstance *model, int unused_arg1, int record_id, int id_bits) {
    int part_total = model->part_count;
    ModelPart *part;
    char *record;
    int part_index;
    int record_index;

    model->found_record_id = 0;
    *(Uint128 *)model->found_record = 0;
    part = model->parts;
    for (part_index = 0; part_index < part_total; part_index++, part++) {
        int record_count = part->data_count;

        record = (char *)(int)part->data_addr;
        for (record_index = 0; record_index < record_count; record_index++, record += 0x10) {
            if ((((ModelPartRecord *)record)->id_word & 0x3FFF) == record_id) {
                *(UnalignedQword *)model->found_record = *(UnalignedQword *)&((ModelPartRecord *)record)->id_word;
                model->found_record_id = record_id;
                model->found_record[0] = (model->found_record[0] & 0xFFFFC000) | id_bits;
                return;
            }
            // match: the record is walked as bytes, its payload size read mid-step
            record += 0x60;
            {
                int payload_size = *(int *)record;

                record += 0x10;
                record += (payload_size & 0x7FFF) << 6;
            }
        }
    }
}

/* Moves every live spawner slot and effect that takes the spawn position by an offset. */
void Effect_OffsetAll(void *offset) {
    EffectSlot *slot = effect_slots;
    EffectActor *effect = effect_pool;
    int i;

    if (effect_slot_count > 0) {
        for (i = 0; i < EFFECT_SLOT_COUNT; i++, slot++) {
            if (slot->flags & EFFECT_SLOT_LIVE) {
                if (!(EFFECT_DEF_FLAGS(EFFECT_SLOT_VIEW(slot)->def) & 0x600000)) {
                    Vector_Add(&slot->position, &slot->position, offset);
                }
            }
        }
    }
    if (effect_count > 0) {
        for (i = 0; i < EFFECT_COUNT; i++, effect++) {
            if (effect->flags & EFFECT_FLAG_LIVE) {
                if (!(EFFECT_DEF_FLAGS(effect->def) & 0x600000)) {
                    Vector_Add(&effect->model->position, &effect->model->position, offset);
                    Vector_Add(&effect->unk70, &effect->unk70, offset);
                }
            }
        }
    }
}

/* Frees the spawner slots and effects owned by anything of map slot map (its entities, its
   map objects, the slot itself) and makes the other effects forget what they knew of it. */
void Effect_FreeForMap(int map) {
    MapSlot *map_slot = &map_slots[map];
    char *first_record = map_slot->records;
    char *last_record = first_record + map_slot->header->record_count * 0x80;
    Entity *first_entity = entity_pool[map];
    Entity *last_entity = &entity_pool[map][0x7F];
    MapObject *first_object = map_slot_objects[map];
    MapObject *last_object = &map_slot_objects[map][0x1FF];
    EffectActor *effect = effect_pool;
    EffectSlot *slot = effect_slots;
    Entity *owner;
    int i;
    int effects_left;

    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            owner = slot->owner_entity;
            if ((owner >= first_entity && owner <= last_entity) ||
                ((MapObject *)owner >= first_object && (MapObject *)owner <= last_object) || owner == (Entity *)map_slot) {
                Effect_FreeSpawner(slot);
            }
        }
    }
    effects_left = effect_count;
    for (i = 0; i < EFFECT_COUNT && effects_left > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            owner = effect->owner_entity;
            if ((owner >= first_entity && owner <= last_entity) ||
                ((MapObject *)owner >= first_object && (MapObject *)owner <= last_object) || owner == (Entity *)map_slot) {
                Effect_Free(effect);
                effects_left = effect_count;
            }
        }
    }
    effect = effect_pool;
    for (i = 0; i < EFFECT_COUNT && effects_left > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (effect->unk6C != 0 && ((Entity *)effect->unk6C)->group_id == map) {
                effect->unk6C = 0;
            }
            // match: last_record is one past the last record of the map, and the test still includes it
            if (effect->unk58 != NULL && effect->unk58 >= first_record && effect->unk58 <= last_record) {
                effect->unk58 = NULL;
            }
        }
    }
}

/* Frees every spawner slot and effect that the given entity owns. */
void Effect_FreeForOwner(Entity *owner) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = effect_slots;
    effect = effect_pool;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->owner_entity == owner) {
            Effect_FreeSpawner(slot);
        }
    }
    for (i = 0; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->owner_entity == owner) {
            Effect_Free(effect);
        }
    }
}

/* Frees the spawner slots and effects that have no effect_param and belong to owner. */
void Effect_FreeUntaggedForOwner(Entity *owner) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = effect_slots;
    effect = effect_pool;
    i = 0;
    for (; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->effect_param == 0xFFFF && slot->owner_entity == owner) {
            Effect_FreeSpawner(slot);
        }
    }
    i = 0;
    for (; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->effect_param == 0xFFFF && effect->owner_entity == owner) {
            Effect_Free(effect);
        }
    }
}

/* Frees every spawner slot and effect whose effect_param is the given id. */
void Effect_FreeByParam(int effect_param) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = effect_slots;
    effect = effect_pool;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->effect_param == effect_param) {
            Effect_FreeSpawner(slot);
        }
    }
    for (i = 0; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->effect_param == effect_param) {
            Effect_Free(effect);
        }
    }
}

/* Frees the player's spawner slots and effects of the weapon kinds 0x280, 0x282 and 0x284. */
void Effect_FreePlayerWeaponKinds(void) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = effect_slots;
    effect = effect_pool;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) &&
            (EFFECT_SLOT_FLAG_PARTS(slot)->def == &effect_table.defs[0x280] || EFFECT_SLOT_FLAG_PARTS(slot)->def == &effect_table.defs[0x282] ||
             EFFECT_SLOT_FLAG_PARTS(slot)->def == &effect_table.defs[0x284]) &&
            slot->owner_entity == (Entity *)&player_state) {
            Effect_FreeSpawner(slot);
        }
    }
    for (i = 0; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) &&
            (effect->def == &effect_table.defs[0x280] || effect->def == &effect_table.defs[0x282] || effect->def == &effect_table.defs[0x284]) &&
            effect->owner_entity == (Entity *)&player_state) {
            Effect_Free(effect);
        }
    }
}

/* Frees the player's tagged spawner slots and effects that may be stopped, but for the weapon kinds. */
void Effect_FreePlayerMagic(void) {
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    slot = effect_slots;
    effect = effect_pool;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->owner_entity == (Entity *)&player_state && slot->effect_param != 0 &&
            fx_table[slot->effect_param].item_menu_magic == 0 && EFFECT_SLOT_FLAG_PARTS(slot)->def != &effect_table.defs[0x280] &&
            EFFECT_SLOT_FLAG_PARTS(slot)->def != &effect_table.defs[0x282] && EFFECT_SLOT_FLAG_PARTS(slot)->def != &effect_table.defs[0x284]) {
            Effect_FreeSpawner(slot);
        }
    }
    for (i = 0; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if ((effect->flags & EFFECT_FLAG_LIVE) && effect->owner_entity == (Entity *)&player_state && effect->effect_param != 0 &&
            fx_table[effect->effect_param].item_menu_magic == 0 && effect->def != &effect_table.defs[0x280] &&
            effect->def != &effect_table.defs[0x282] && effect->def != &effect_table.defs[0x284]) {
            Effect_Free(effect);
        }
    }
}

/* Frees every live spawner slot whose effect_param is effect_param, while any are live. */
void Effect_FreeSpawnersByParam(int effect_param) {
    EffectSlot *slot;
    int i;

    slot = effect_slots;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if ((slot->flags & EFFECT_SLOT_LIVE) && slot->effect_param == effect_param) {
            Effect_FreeSpawner(slot);
        }
    }
}

/* Returns 1 if a live spawner or effect uses an EffectDef from first_def to last_def. */
int Effect_IsDefRangeInUse(int first_def, int last_def) {
    EffectDef *first;
    EffectDef *last;
    EffectSlot *slot;
    EffectActor *effect;
    int i;

    if (last_def < first_def || first_def > 0 || last_def > 0) {
        return 0;
    }
    first = &effect_table.defs[first_def];
    last = &effect_table.defs[last_def];
    slot = effect_slots;
    for (i = 0; i < EFFECT_SLOT_COUNT && effect_slot_count > 0; i++, slot++) {
        if (slot->flags & EFFECT_SLOT_LIVE) {
            if (EFFECT_SLOT_FLAG_PARTS(slot)->def >= first && EFFECT_SLOT_FLAG_PARTS(slot)->def <= last) {
                return 1;
            }
        }
    }
    effect = effect_pool;
    for (i = 0; i < EFFECT_COUNT && effect_count > 0; i++, effect++) {
        if (effect->flags & EFFECT_FLAG_LIVE) {
            if (effect->def >= first && effect->def <= last) {
                return 1;
            }
        }
    }
    return 0;
}

/* GS values for the model draw hooks below (effect_draw_hooks, picked by Effect_SetupModel) */
#define EFFECT_ZBUF_NO_WRITE 0x10A000118L       /* ZBUF_1: the depth buffer, masked: no depth writes */
#define EFFECT_ZBUF_WRITE 0xA000118L            /* ZBUF_1: the depth buffer, written */
#define EFFECT_ALPHA_ADD 0x48                   /* ALPHA_1: Cs * As + Cd */
#define EFFECT_ALPHA_SUBTRACT 0x42              /* ALPHA_1: Cd - Cs * As */
#define EFFECT_ALPHA_BLEND 0x44                 /* ALPHA_1: (Cs - Cd) * As + Cd */
#define EFFECT_ALPHA_OPAQUE 0x80000000A8L       /* ALPHA_1: Cs (FIX 0x80) */
#define EFFECT_TEST_COLOUR_DEPTH 0x53001        /* TEST_1: colour only (alpha test NEVER, AFAIL RGB_ONLY), depth test GEQUAL */
#define EFFECT_TEST_COLOUR_ON_TOP 0x33001       /* TEST_1: colour only, depth test ALWAYS */
#define EFFECT_TEST_CUTOUT_DEPTH 0x5000D        /* TEST_1: pixels with alpha above 0, depth test GEQUAL */
#define EFFECT_TEST_CUTOUT_ON_TOP 0x3000D       /* TEST_1: pixels with alpha above 0, depth test ALWAYS */

/* Draw hook: additive blending behind the scenery's depth, no depth writes; flushes with
   GsDraw_QueueFlush, then sends the model's textures. */
void Effect_PrepareAdditiveDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_ADD);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_DEPTH);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    ModelTexture_SendForModel(model);
}

/* Draw hook: subtractive blending, depth-tested, no depth writes; then sends the model's textures. */
void Effect_PrepareSubtractiveDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_SUBTRACT);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_DEPTH);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Draw hook: alpha blending, depth-tested, no depth writes; then sends the model's textures. */
void Effect_PrepareBlendedDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_BLEND);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_DEPTH);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Draw hook: no blending, pixels with alpha above 0 only, depth-tested and written; then sends
   the model's textures. */
void Effect_PrepareCutoutDraw(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_OPAQUE);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_CUTOUT_DEPTH);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Effect_PrepareAdditiveDraw drawn over everything (no depth test). */
void Effect_PrepareAdditiveDrawOnTop(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_ADD);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_ON_TOP);
    Packet_EndGsRegisters();
    GsDraw_QueueFlush();
    ModelTexture_SendForModel(model);
}

/* Effect_PrepareSubtractiveDraw drawn over everything (no depth test). */
void Effect_PrepareSubtractiveDrawOnTop(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_SUBTRACT);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_ON_TOP);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Effect_PrepareBlendedDraw drawn over everything (no depth test). */
void Effect_PrepareBlendedDrawOnTop(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_NO_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_BLEND);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_COLOUR_ON_TOP);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}

/* Effect_PrepareCutoutDraw without the depth test; it still writes depth. */
void Effect_PrepareCutoutDrawOnTop(ModelInstance *model) {
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, EFFECT_ZBUF_WRITE);
    Packet_AddGsRegister(GS_REG_ALPHA_1, EFFECT_ALPHA_OPAQUE);
    Packet_AddGsRegister(GS_REG_TEST_1, EFFECT_TEST_CUTOUT_ON_TOP);
    Packet_EndGsRegisters();
    ModelTexture_SendForModel(model);
}
