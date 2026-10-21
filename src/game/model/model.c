#include "model/model.h"
#include "model/model_matrix.h"
#include "map/map.h"
#include "gfx/gs.h"
#include <eeregs.h>
#include "math/vu0.h"
#include "gfx/packet.h"
#include <libvu0.h>
#include <math.h>
#include "math/angle.h"
#include "math/camera.h"

/* loads the matrix the children are multiplied by into vf1..vf4 */
#define LOAD_PARENT_MATRIX(matrix)                                                                 \
    __asm__ volatile(                                                                              \
        "lqc2 $vf1, 0x0(%0)\n"                                                                     \
        "lqc2 $vf2, 0x10(%0)\n"                                                                    \
        "lqc2 $vf3, 0x20(%0)\n"                                                                    \
        "lqc2 $vf4, 0x30(%0)\n"                                                                    \
        :                                                                                          \
        : "r"(matrix))
/* multiplies the matrix at `matrix` by the one loaded in vf1..vf4, in place */
#define MULTIPLY_BY_PARENT(matrix)                                                                 \
    __asm__ volatile(                                                                              \
        "lqc2 $vf22, 0x0(%0)\n"                                                                    \
        "lqc2 $vf23, 0x10(%0)\n"                                                                   \
        "lqc2 $vf24, 0x20(%0)\n"                                                                   \
        "lqc2 $vf25, 0x30(%0)\n"                                                                   \
        :                                                                                          \
        : "r"(matrix));                                                                            \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf22x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf22y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf22z\n"                                                         \
        "vmaddw.xyzw $vf22, $vf4, $vf22w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf23x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf23y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf23z\n"                                                         \
        "vmaddw.xyzw $vf23, $vf4, $vf23w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf24x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf24y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf24z\n"                                                         \
        "vmaddw.xyzw $vf24, $vf4, $vf24w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vmulax.xyzw ACC, $vf1, $vf25x\n"                                                          \
        "vmadday.xyzw ACC, $vf2, $vf25y\n"                                                         \
        "vmaddaz.xyzw ACC, $vf3, $vf25z\n"                                                         \
        "vmaddw.xyzw $vf25, $vf4, $vf25w\n");                                                      \
    __asm__ volatile(                                                                              \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "vnop\n"                                                                                   \
        "sqc2 $vf22, 0x0(%0)\n"                                                                    \
        "sqc2 $vf23, 0x10(%0)\n"                                                                   \
        "sqc2 $vf24, 0x20(%0)\n"                                                                   \
        "sqc2 $vf25, 0x30(%0)\n"                                                                   \
        : "=r"(matrix)                                                                             \
        : "0"(matrix))
typedef u32 u_long128 __attribute__((mode(TI)));
/* copies a 4x4 matrix through VU0 registers */
#define COPY_MATRIX(to, from)                                                                      \
    __asm__ volatile(                                                                              \
        "lqc2 $vf16, 0x0(%0)\n"                                                                    \
        "lqc2 $vf17, 0x10(%0)\n"                                                                   \
        "lqc2 $vf18, 0x20(%0)\n"                                                                   \
        "lqc2 $vf19, 0x30(%0)\n"                                                                   \
        "sqc2 $vf16, 0x0(%1)\n"                                                                    \
        "sqc2 $vf17, 0x10(%1)\n"                                                                   \
        "sqc2 $vf18, 0x20(%1)\n"                                                                   \
        "sqc2 $vf19, 0x30(%1)\n"                                                                   \
        : : "r"(from), "r"(to))
/* the largest float: a vertex's distance before it has been measured.
   match: written as a literal, the compiler keeps it in .sdata (largest_float_not_measured) */
#define NOT_MEASURED FLT_MAX
// match: the asm label (gcse hashes the name) gives retail's registers for Model_SetUpSkinned's three function addresses
void Model_UpdateSkinnedBonePoints(ModelInstance *) __asm__("Model_UpdateSkinnedBonePoints");
// match: called for an int result (the definition is void), which keeps $2 busy after the call
#define MODEL_BUILD_DRAW_MATRICES(model, normal_src, light_src, normal_dest, light_dest) \
    ((int (*)(ModelInstance *, s32, s32, void *, void *))ModelMatrix_BuildDrawMatrices)(model, normal_src, light_src, normal_dest, light_dest)

/* Multiply the matrices of a part's children by the part's own matrix. */
void Model_MultiplyChildMatrices(ModelPart *part) {
    register ModelPart *matrix asm("$2"); // match: the game keeps the matrix address in $2
    ModelPart *child;

    matrix = part;
    child = matrix->child;
    if (child != NULL) {
        LOAD_PARENT_MATRIX(matrix);
        matrix = child;
        MULTIPLY_BY_PARENT(matrix);
        part = matrix->sibling;
        if (part != NULL) {
            do {
                matrix = part;
                MULTIPLY_BY_PARENT(matrix);
                matrix = matrix->sibling;
                part = matrix;
            } while (matrix != NULL);
        }
    }
}

/* Sets a skinned model up from a model file (vertices can follow their part or its parent): its parts and
   bone points are carved out of *memory, and both cursors move past what was used. */
void Model_SetUpSkinned(ModelInstance *model, int *file_cursor, int *memory) {
    ModelFile *file;
    ModelFilePart *file_part;
    ModelPart *memory_base;
    char *cursor;
    ModelPart *part;
    Vector4 *positions;
    ModelPartData *data;
    s32 part_count;
    u16 length;
    s32 first;
    s32 i;
    s32 j;

    model->model_flags |= 0x4000;
    cursor = (char *)*file_cursor;
    memory_base = (ModelPart *)*memory;
    model->file = (ModelFile *)cursor;
    file = (ModelFile *)cursor;
    part_count = file->part_count;
    cursor = (char *)file->bone_anchors;
    model->bone_anchors = (BoneAnchor *)cursor;
    model->part_count = model->file_part_count = file->part_count;
    file_part = file->parts;
    positions = (Vector4 *)&memory_base[part_count];
    model->parts = memory_base;
    model->bone_positions = positions;
    *memory = (int)(positions + 32);
    part = memory_base;
    for (i = 0; i < file->part_count; i++, part++, file_part++) {
        part->data_count = file_part->data_count;
        part->data_addr = *file_cursor + file_part->data_offset;
        part->parent_index = file_part->parent;
        MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
        // match: retail keeps only the empty loop; a walk whose result nothing uses is deleted like this one
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < file_part->data_count; j++) {
            length = data->length;
            data = MODEL_PART_DATA_SKIP(data, length);
        }
        part->position[0] = part->position[1] = part->position[2] = 0.0f;
        part->position[3] = 1.0f;
        part->rotation_x = part->rotation_y = part->rotation_z = 0.0f;
        part->rotation_w = 1.0f;
        part->scale[0] = part->scale[1] = part->scale[2] = 1.0f;
        if (file_part->slot >= 1 && file_part->slot <= 3) {
            MODEL_SLOT_PARTS(model)[file_part->slot - 1] = part;
        }
    }
    model->draw_parts = Model_DrawSkinned;
    *file_cursor += file->size;
    model->update_bone_points = Model_UpdateSkinnedBonePoints;
    model->get_point = Model_GetSkinnedBonePoint;
    first = TRUE;
    model->draw_header_vector[0] = 0;
    model->draw_header_vector[1] = 0;
    model->draw_header_vector[2] = 0;
    model->draw_header_vector[3] = 0;
    model->colour_red = 128.0f;
    model->colour_green = 128.0f;
    model->colour_blue = 128.0f;
    model->alpha = 128.0f;
    /* the first block with a nonzero unk37 in the whole model gets unk61 = 2 */
    for (i = 0, part = model->parts; i < file->part_count; i++, part++) {
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < part->data_count; j++) {
            length = data->length;
            if (first && data->texture_word.clut_base != 0) {
                first = FALSE;
                data->texture_word.clut_load_mode = GS_TEX0_CLD_LOAD_TO_CBP0;
            }
            data = MODEL_PART_DATA_SKIP(data, length);
        }
    }
}

/* an angle brought back into -pi..pi (for angles above -pi) */
static inline f32 wrap_angle(f32 angle) {
    angle += PI;
    return angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
}

/* Draws a model (the draw function Model_SetUpSkinned installs): works out a turn from the block at
   unk148 and the model's rotation, queues a packet with the model's two matrices, its MapFogBlock
   block, its lights and the vectors at unk160 and unk150, then for every part that is switched on
   and not scaled to nothing its two matrices, its parent's two and a reference to each of its data
   blocks. */
void Model_DrawSkinned(ModelInstance *model) {
    void *matrix_a = MODEL_TURN_MATRIX_A;
    void *matrix_b = MODEL_TURN_MATRIX_B;
    f32 (*part_matrices)[16] = MODEL_TURN_PART_MATRICES;
    f32 (*part_turns)[16] = MODEL_TURN_PART_TURNS;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    s32 part_count;
    u32 word;
    f32 pitch;
    f32 yaw;

    i = 0;
    pitch = -asinf(((f32 *)model->light_block)[4]);
    yaw = Angle_Wrap(atan2f(((f32 *)model->light_block)[8], ((f32 *)model->light_block)[0]) + HALF_PI);
    sceVu0RotMatrixY(MODEL_TURN_SCRATCH, identity_matrix, yaw);
    sceVu0RotMatrixX(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, pitch);
    sceVu0RotMatrixX(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.x));
    sceVu0RotMatrixY(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.y));
    sceVu0RotMatrixZ(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.z));
    MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, MODEL_TURN_MATRIX_A, MODEL_TURN_MATRIX_B);
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        Matrix_Multiply(part_matrices[i], model->model_matrix, part);
        Matrix_Multiply(part_turns[i], MODEL_TURN_SCRATCH, part);
    }
    packet_start = packet = Packet_Alloc(0x20);
    *packet = 0x10000009;
    packet += 7;
    *packet++ = 0x6C08000C;
    COPY_MATRIX(packet, matrix_a);
    packet += 0x10;
    COPY_MATRIX(packet, matrix_b);
    packet += 0x10;
    *packet = 0x10000002;
    packet += 7;
    *packet++ = 0x6C01003C;
    *(Vector4 *)packet = *(Vector4 *)model->fog_block;
    packet += 4;
    *packet = 0x10000003;
    packet += 7;
    *packet++ = 0x6C02003D;
    *(u64 *)packet = 0x1000000000008001;
    packet += 2;
    *(u64 *)packet = 0xE;
    packet += 2;
    *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
    packet += 2;
    *packet++ = 0x3D;
    *packet++ = 0;
    *packet = 0x1000000A;
    packet += 7;
    *packet = 0x6C090040;
    packet += 4;
    *packet++ = model->hit_count;
    light_out = packet;
    light_out_b = packet + 0x10;
    for (i = 0; i < model->hit_count; i++) {
        *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
        *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
        light_out_b += 4;
        ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
        light_out += 4;
    }
    packet += 0x20;
    *packet++ = 0x10000003;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C020064;
    Vector_Copy(packet, model->draw_header_vector);
    Vector_Copy(packet + 4, &model->colour_red);
    part = model->parts;
    for (i = 0; i < model->part_count; i++, part++) {
        if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
            continue;
        }
        if (part->scale[0] == 0.0f && part->scale[1] == 0.0f && part->scale[2] == 0.0f) {
            continue;
        }
        packet = Packet_Alloc(2);
        packet[0] = 0x10000001;
        packet[7] = 0x13000000;
        packet = Packet_Alloc(0xC);
        *packet = 0x10000005;
        packet += 7;
        *packet++ = 0x6C040004;
        COPY_MATRIX(packet, part_matrices[i]);
        packet += 0x10;
        *packet++ = 0x10000005;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x6C040024;
        COPY_MATRIX(packet, part_turns[i]);
        if (MODEL_PART_PARENT_ANY(part) != -1) { // match: always true, the 7-bit field holds 0x7F for "no parent"
            packet = Packet_Alloc(0xC);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040008;
            COPY_MATRIX(packet, part_matrices[MODEL_PART_PARENT_ANY(part)]);
            packet += 0x10;
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040028;
            COPY_MATRIX(packet, part_turns[MODEL_PART_PARENT_ANY(part)]);
        }
        data = (ModelPartData *)(u32)part->data_addr;
        packet = Packet_Alloc(part->data_count);
        for (j = 0; j < part->data_count; j++) {
            word = *(u32 *)data;
            *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
            *packet = (u32)data + 0x10;
            packet += 3;
            data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
        }
    }
    packet = Packet_Alloc(1); // match: packet named after packet_start's last use, or the stores go through packet_start
    Packet_AppendToChain(packet_start, packet);
}

/* Draws a model like Model_DrawSkinned, but in two passes with their own Z buffer state: first the
   parts with bit 41 of their flag word set, with Z writes on, then the others with Z writes off. */
void Model_DrawSkinnedTwoPass(ModelInstance *model) {
    void *matrix_a = MODEL_TURN_MATRIX_A;
    void *matrix_b = MODEL_TURN_MATRIX_B;
    f32 (*part_matrices)[16] = MODEL_TURN_PART_MATRICES;
    f32 (*part_turns)[16] = MODEL_TURN_PART_TURNS;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    s32 part_count;
    s32 pass;
    u32 word;
    f32 pitch;
    f32 yaw;

    i = 0;
    pitch = -asinf(((f32 *)model->light_block)[4]);
    yaw = Angle_Wrap(atan2f(((f32 *)model->light_block)[8], ((f32 *)model->light_block)[0]) + HALF_PI);
    sceVu0RotMatrixY(MODEL_TURN_SCRATCH, identity_matrix, yaw);
    sceVu0RotMatrixX(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, pitch);
    sceVu0RotMatrixX(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.x));
    sceVu0RotMatrixY(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.y));
    sceVu0RotMatrixZ(MODEL_TURN_SCRATCH, MODEL_TURN_SCRATCH, wrap_angle(model->rotation.z));
    MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, MODEL_TURN_MATRIX_A, MODEL_TURN_MATRIX_B);
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        Matrix_Multiply(part_matrices[i], model->model_matrix, part);
        Matrix_Multiply(part_turns[i], MODEL_TURN_SCRATCH, part);
    }
    packet_start = packet = Packet_Alloc(0x20);
    *packet = 0x10000009;
    packet += 7;
    *packet++ = 0x6C08000C;
    COPY_MATRIX(packet, matrix_a);
    packet += 0x10;
    COPY_MATRIX(packet, matrix_b);
    packet += 0x10;
    *packet = 0x10000002;
    packet += 7;
    *packet++ = 0x6C01003C;
    *(Vector4 *)packet = *(Vector4 *)model->fog_block;
    packet += 4;
    *packet = 0x10000003;
    packet += 7;
    *packet++ = 0x6C02003D;
    *(u64 *)packet = 0x1000000000008001;
    packet += 2;
    *(u64 *)packet = 0xE;
    packet += 2;
    *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
    packet += 2;
    *packet++ = 0x3D;
    *packet++ = 0;
    *packet = 0x1000000A;
    packet += 7;
    *packet = 0x6C090040;
    packet += 4;
    *packet++ = model->hit_count;
    light_out = packet;
    light_out_b = packet + 0x10;
    for (i = 0; i < model->hit_count; i++) {
        *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
        *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
        light_out_b += 4;
        ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
        light_out += 4;
    }
    packet += 0x20;
    *packet++ = 0x10000003;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0;
    *packet++ = 0x6C020064;
    Vector_Copy(packet, model->draw_header_vector);
    Vector_Copy(packet + 4, &model->colour_red);
    packet = Packet_Alloc(1); // match: packet named after packet_start's last use, or the stores go through packet_start
    Packet_AppendToChain(packet_start, packet);
    for (pass = 0; pass < 2; pass++) {
        Packet_BeginGsRegisters();
        if (pass == 0) {
            Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
            Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118);
        } else {
            Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
            Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
        }
        Packet_EndGsRegisters();
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (((u8)(MODEL_PART_FLAG_WORD(part) >> 41) & 1) == pass) {
                continue;
            }
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->scale[0] == 0.0f && part->scale[1] == 0.0f && part->scale[2] == 0.0f) {
                continue;
            }
            packet_start = packet = Packet_Alloc(2);
            packet_start[0] = 0x10000001;
            packet_start[7] = 0x13000000;
            packet = Packet_Alloc(0xC);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            COPY_MATRIX(packet, part_matrices[i]);
            packet += 0x10;
            *packet++ = 0x10000005;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x6C040024;
            COPY_MATRIX(packet, part_turns[i]);
            if (MODEL_PART_PARENT_ANY(part) != -1) { // match: always true, the 7-bit field holds 0x7F for "no parent"
                packet = Packet_Alloc(0xC);
                *packet = 0x10000005;
                packet += 7;
                *packet++ = 0x6C040008;
                COPY_MATRIX(packet, part_matrices[MODEL_PART_PARENT_ANY(part)]);
                packet += 0x10;
                *packet = 0x10000005;
                packet += 7;
                *packet++ = 0x6C040028;
                COPY_MATRIX(packet, part_turns[MODEL_PART_PARENT_ANY(part)]);
            }
            data = (ModelPartData *)(u32)part->data_addr;
            packet = Packet_Alloc(part->data_count);
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
            }
            packet = Packet_Alloc(1); // match: packet named after packet_start's last use, or the stores go through packet_start
            Packet_AppendToChain(packet_start, packet);
        }
    }
    Packet_BeginGsRegisters();
    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
    Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118);
    Packet_EndGsRegisters();
}

/* Places a skinned model's 32 bone points: each anchor's offset through its part's matrix and then the
   model's, or the model's position for an anchor without a bone. */
void Model_UpdateSkinnedBonePoints(ModelInstance *actor) {
    float point[4];
    int i;
    BoneAnchor *anchor;
    Vector4 *position;

    anchor = actor->bone_anchors;
    position = actor->bone_positions;
    if (actor != NULL) {
        for (i = 0; i < 32; i++, anchor++, position++) {
            if (anchor->bone != 0xFF) {
                Vector_Copy(point, anchor);
                point[3] = 1.0f;
                Vector_ApplyMatrix(position, &actor->parts[anchor->bone], point);
                Vector_ApplyMatrix(position, actor->model_matrix, position);
            } else {
                Vector_Copy(position, &actor->position);
            }
        }
    }
}

/* Where point `index` of a model is: one of its 32 bone points, or its position for any other index. */
f32 *Model_GetSkinnedBonePoint(ModelInstance *model, int index) {
    if (model != NULL) {
        if ((u32)index < 32) {
            return &model->bone_positions[index].x;
        }
        return &model->position.x;
    }
    return NULL;
}

/* Set the doublewords at +0x30, +0x40, +0x50 and +0x60 of every data block of every part; -1 leaves
   one as it is. */
void Model_SetBlockGsWords(ModelInstance *model, s64 unk30, s64 unk40, s64 unk50, s64 unk60) {
    ModelPart *part;
    char *cursor;
    char *block;
    int i;
    int j;
    int count;
    int part_count;
    u16 length;

    part_count = model->part_count;
    part = model->parts;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        cursor = (char *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            block = cursor;
            length = ((ModelPartData *)block)->length;
            cursor += 0x30;
            if (unk30 != -1) {
                *(s64 *)cursor = unk30;
            }
            cursor += 0x10;
            if (unk40 != -1) {
                *(s64 *)cursor = unk40;
            }
            cursor += 0x10;
            if (unk50 != -1) {
                *(s64 *)cursor = unk50;
            }
            cursor += 0x10;
            if (unk60 != -1) {
                *(s64 *)cursor = unk60;
            }
            cursor = block + (length + 1) * 0x10;
        }
    }
}

/* Gives every vertex of a model drawn with its own texture the texture coordinates of the place
   it lands on the screen (0 to 1 across the screen). */
void Model_SetUvsFromScreen(void *model_arg) {
    ModelInstance *model = model_arg;
    ModelPart *part;
    f32 *matrix;
    int count;
    f32 *parent_matrix;
    int part_count;
    int j;
    int parent;
    int i; // match: declared after j and parent, which decides the order of the stack slots
    ModelProjectScratch *scratch = MODEL_PROJECT_SCRATCH;

    part = model->parts;
    part_count = model->part_count;
    matrix = MODEL_PROJECT_SCRATCH->matrices[0];
    for (i = 0; i < part_count; i++, part++) {
        Matrix_Multiply(matrix, model->model_matrix, part);
        Matrix_Multiply(matrix, world_screen_matrix, matrix);
        matrix += 16;
    }
    i = 0;
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        s32 last;
        s32 *cursor;

        matrix = scratch->matrices[i];
        parent = MODEL_PART_PARENT(part);
        if (parent != -1) { // match: always true, the 7-bit field holds 0x7F for "no parent" (see Model_SetUpSkinned)
            parent_matrix = scratch->matrices[parent];
        }
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        last = -1;
        j = 0;
        cursor = *(s32 **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            ModelWideData *block = (ModelWideData *)cursor;
            u16 length = block->length;
            s32 k = 0;
            s32 vertex_count;
            ModelWideVertex *vertex;

            cursor = &block->vertex_count;
            vertex_count = *cursor & GIF_TAG_NLOOP_MASK;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                if (last != vertex->on_parent) {
                    if (vertex->on_parent != 0) {
                        Vu0_LoadMatrix(parent_matrix);
                    } else {
                        Vu0_LoadMatrix(matrix);
                    }
                    last = vertex->on_parent;
                }
                Vector_Add(MODEL_PROJECT_SCRATCH->point, vertex->position, vertex->offset);
                Vu0_ProjectToScreen(MODEL_PROJECT_SCRATCH->screen, MODEL_PROJECT_SCRATCH->point);
                vertex->tex_s = (*&MODEL_PROJECT_SCRATCH->screen[0] - 0x6C00) / 10240.0f;
                vertex->tex_t = (*&MODEL_PROJECT_SCRATCH->screen[1] - 0x7200) / 7168.0f;
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
}

/* Saves the four GS doublewords at +0x30 to +0x60 of every block of a model drawn with its own
   texture, and each vertex's texture coordinates, to `save` (kept in wave_uvs for Model_RestoreGsWordsAndUvs).
   Returns where the saved words end. */
s64 *Model_SaveGsWordsAndUvs(ModelInstance *model, s64 *save) {
    s64 *out;
    int i;
    u16 part_count;
    ModelPart *part;
    int vertex_count;

    model->wave_uvs = (ModelWaveUv *)save; /* the field holds the save area on these models */
    part = model->parts;
    out = save;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        int j = 0;
        int count = (int)(MODEL_PART_WORD(part) >> 53);
        s64 *cursor = *(s64 **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            s64 *block = cursor;
            u16 length = *block;
            int k = 0;
            s64 *vertex_words;

            cursor += 6;
            *out++ = *cursor;
            vertex_count = ((ModelWideData *)block)->vertex_count & GIF_TAG_NLOOP_MASK;
            vertex_words = (s64 *)((ModelWideData *)block)->vertices;
            cursor += 2;
            *out++ = *cursor;
            cursor += 2;
            *out++ = *cursor;
            cursor += 2;
            *out++ = *cursor;
            for (; k < vertex_count; k++) {
                *out++ = vertex_words[4]; /* ModelWideVertex.tex_s and tex_t */
                vertex_words += 8;
            }
            cursor = block + (length + 1) * 2;
        }
    }
    return out;
}

/* Puts back what Model_SaveGsWordsAndUvs saved: the four GS doublewords of every block and each vertex's
   texture coordinates. */
void Model_RestoreGsWordsAndUvs(ModelInstance *model) {
    int i = 0;
    u16 part_count = model->part_count;
    ModelPart *part = model->parts;
    s64 *saved = (s64 *)model->wave_uvs;

    for (; i < part_count; i++, part++) {
        int j = 0;
        int count = (int)(MODEL_PART_WORD(part) >> 53);
        char *cursor = *(char **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            char *block = cursor;
            u16 length = *(u16 *)cursor;
            int vertex_count;
            int k = 0;
            ModelWideVertex *vertex;

            cursor += 0x30;
            *(s64 *)cursor = *saved++;
            vertex_count = ((ModelWideData *)block)->vertex_count & GIF_TAG_NLOOP_MASK;
            vertex = ((ModelWideData *)block)->vertices;
            cursor += 0x10;
            *(s64 *)cursor = *saved++;
            cursor += 0x10;
            *(s64 *)cursor = *saved++;
            cursor += 0x10;
            *(s64 *)cursor = *saved++;
            for (; k < vertex_count; k++) {
                *(s64 *)&vertex->tex_s = *saved++;
                vertex++;
            }
            cursor = block + (length + 1) * 16;
        }
    }
}

/* whether the primitive of a GIF tag is alpha blended, from the tag's high word */
static inline s32 is_blended(u64 tag_high) {
    return (tag_high >> 21) & 1;
}

/* Set a model instance up from a model file whose parts come with their pose: the parts are
   carved out of *memory, and both cursors move past what was used. */
void Model_SetUpPosed(ModelInstance *model, char **file_cursor, char **memory) {
    PosedModelFile *file;
    PosedModelFilePart *file_part;
    ModelPart *part;
    ModelPartData *data;
    s32 first = TRUE;
    s32 i = 0;
    s32 j;
    s32 opaque;
    s32 blended;
    s32 count;
    u16 length;
    char *base;
    char *memory_base;
    char *addr;

    base = *file_cursor;
    memory_base = *memory;
    model->file = (ModelFile *)base;
    file = (PosedModelFile *)base;
    model->parts = (ModelPart *)memory_base;
    base = (char *)((model->file_part_count = file->part_count) * sizeof(ModelPart)); // match: base reused for the size, the count stored on the way
    *memory = memory_base + (u32)base;
    model->part_count = model->file_part_count; // match: copied from the store above, not from the file
    part = (ModelPart *)memory_base;
    file_part = file->parts;
    if (POSED_MODEL_FILE_ANY(file).part_count != 0) { // match: the test reads the count after the cursor store
        do {
            part->data_count = file_part->data_count;
            memory_base = *file_cursor + file_part->data_offset;
            part->data_addr = (u32)memory_base;
            blended = is_blended(((ModelPartData *)memory_base)->gif_tag_high);
            data = (ModelPartData *)memory_base;
            opaque = FALSE;
            if (!blended || ((GsAlphaReg *)&data->blend)->f.a_source != 0 || ((GsAlphaReg *)&data->blend)->f.b_source != 2 || ((GsAlphaReg *)&data->blend)->f.d_source != 1) {
                opaque = TRUE;
            }
            MODEL_PART_FLAG_BITS(part)->bit41 = opaque;
            if (first) {
                count = file_part->data_count;
                data = (ModelPartData *)(u32)part->data_addr;
                for (j = 0; j < count; j++) {
                    length = data->length;
                    if (((ModelPartDataBits30 *)&data->tex0)->clut_base != 0) {
                        ((ModelPartDataBits30 *)&data->tex0)->clut_load_mode = GS_TEX0_CLD_LOAD_TO_CBP0;
                        first = FALSE;
                        break;
                    }
                    data = MODEL_PART_DATA_SKIP(data, length + 1);
                }
            }
            part->position[0] = file_part->position[0];
            part->position[1] = file_part->position[1];
            part->position[2] = file_part->position[2];
            part->position[3] = 1.0f;
            part->rotation_x = file_part->rotation[0];
            part->rotation_y = file_part->rotation[1];
            part->rotation_z = file_part->rotation[2];
            part->rotation_w = 1.0f;
            part->scale[0] = file_part->scale[0];
            part->scale[1] = file_part->scale[1];
            part->scale[2] = file_part->scale[2];
            part++;
            file_part++;
            i++;
        } while (i < file->part_count);
    }
    if (file->bone_anchors_offset != 0) {
        addr = *file_cursor;
        model->bone_positions = (Vector4 *)*memory;
        *memory += 32 * sizeof(Vector4);
        model->bone_anchors = (BoneAnchor *)(addr + file->bone_anchors_offset);
    } else {
        model->bone_anchors = NULL;
    }
    model->draw_parts = Model_DrawPosed;
    *file_cursor += file->size;
    model->update_bone_points = Model_UpdateBonePoints;
    model->get_point = Model_GetBonePoint;
}

/* Draws a model (ModelInstance.draw_parts of the models Model_SetUpPosed sets up): queues a packet with
   its two matrices, its MapFogBlock block, its lights and, when it has one, its own colour, then
   for every part that is switched on a packet with the part's matrix and a reference to each of
   its data blocks. */
void Model_DrawPosed(ModelInstance *model) {
    void *matrix_a = MODEL_DRAW_MATRIX_A;
    void *matrix_b = MODEL_DRAW_MATRIX_B;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    u32 word;
    f32 unused[4];

    if (model->parts != NULL && model->part_count != 0) {
        MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, matrix_a, matrix_b);
        if (model->model_flags & 0x800) {
            packet_start = packet = Packet_Alloc(0x21); // match: two calls, the compiler merges them
        } else {
            packet_start = packet = Packet_Alloc(0x1C);
        }
        *packet = 0x10000009;
        packet += 7;
        *packet++ = 0x6C08000C;
        COPY_MATRIX(packet, matrix_a);
        packet += 0x10;
        COPY_MATRIX(packet, matrix_b);
        packet += 0x10;
        *packet = 0x10000002;
        packet += 7;
        *packet++ = 0x6C01003C;
        *(Vector4 *)packet = *(Vector4 *)model->fog_block;
        packet += 4;
        *packet = 0x10000003;
        packet += 7;
        *packet++ = 0x6C02003D;
        *(u64 *)packet = 0x1000000000008001;
        packet += 2;
        *(u64 *)packet = 0xE;
        packet += 2;
        *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
        packet += 2;
        *packet++ = 0x3D;
        *packet++ = 0;
        *packet = 0x1000000A;
        packet += 7;
        *packet = 0x6C090040;
        packet += 4;
        *packet++ = model->hit_count;
        light_out = packet;
        light_out_b = packet + 0x10;
        for (i = 0; i < model->hit_count; i++) {
            *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
            *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
            light_out_b += 4;
            ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
            light_out += 4;
        }
        packet += 0x20;
        if (model->model_flags & 0x800) {
            *packet++ = 0x10000004;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x6C030064;
            Vector_Copy(packet, &model->colour_red);
            packet += 4;
            packet[3] = model->found_record_id;
            ((u_long128 *)packet)[1] = *(u_long128 *)model->found_record;
        }
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            data = (ModelPartData *)(u32)part->data_addr;
            packet = Packet_Alloc(part->data_count + 6);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            Matrix_Multiply(packet, model->model_matrix, part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
            }
        }
        packet = Packet_Alloc(1);
        Packet_AppendToChain(packet_start, packet);
    }
}

/* Draws a model like Model_DrawPosed, with the two matrices of ModelMatrix_BuildDrawMatrices on the stack and each
   part's own matrix sent as it is; parts scaled to nothing are left out. */
void Model_DrawPosedPrebuilt(ModelInstance *model) {
    VuVector4 matrix_a[4];
    VuVector4 matrix_b[4];
    void *matrix_b_ptr;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    u32 word;

    if (model->parts != NULL && model->part_count != 0) {
        matrix_b_ptr = matrix_b;
        MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, matrix_a, matrix_b_ptr);
        if (model->model_flags & 0x800) {
            packet_start = packet = Packet_Alloc(0x21); // match: two calls, the compiler merges them
        } else {
            packet_start = packet = Packet_Alloc(0x1C);
        }
        *packet = 0x10000009;
        packet += 7;
        *packet++ = 0x6C08000C;
        COPY_MATRIX(packet, matrix_a);
        packet += 0x10;
        COPY_MATRIX(packet, matrix_b_ptr);
        packet += 0x10;
        *packet = 0x10000002;
        packet += 7;
        *packet++ = 0x6C01003C;
        *(Vector4 *)packet = *(Vector4 *)model->fog_block;
        packet += 4;
        *packet = 0x10000003;
        packet += 7;
        *packet++ = 0x6C02003D;
        *(u64 *)packet = 0x1000000000008001;
        packet += 2;
        *(u64 *)packet = 0xE;
        packet += 2;
        *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
        packet += 2;
        *packet++ = 0x3D;
        *packet++ = 0;
        *packet = 0x1000000A;
        packet += 7;
        *packet = 0x6C090040;
        packet += 4;
        *packet++ = model->hit_count;
        light_out = packet;
        light_out_b = packet + 0x10;
        for (i = 0; i < model->hit_count; i++) {
            *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
            *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
            light_out_b += 4;
            ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
            light_out += 4;
        }
        packet += 0x20;
        if (model->model_flags & 0x800) {
            *packet++ = 0x10000004;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x6C030064;
            Vector_Copy(packet, &model->colour_red);
            packet += 4;
            packet[3] = model->found_record_id;
            ((u_long128 *)packet)[1] = *(u_long128 *)model->found_record;
        }
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->scale[0] == 0.0f && part->scale[1] == 0.0f && part->scale[2] == 0.0f) {
                continue;
            }
            packet = Packet_Alloc(part->data_count + 6);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            COPY_MATRIX(packet, part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
            }
        }
        packet = Packet_Alloc(1);
        Packet_AppendToChain(packet_start, packet);
    }
}

/* Draws a model like Model_DrawPosed (its own colour included), but as Model_DrawPosedWithAlpha does the
   parts: the first packet is queued at once, parts scaled to nothing are left out and each part
   gets its Z buffer and test state and its own packet. */
void Model_DrawPosedDepthByPart(ModelInstance *model) {
    void *matrix_a = MODEL_DRAW_MATRIX_A;
    void *matrix_b = MODEL_DRAW_MATRIX_B;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    u32 word;

    if (model->parts != NULL && model->part_count != 0) {
        MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, matrix_a, matrix_b);
        if (model->model_flags & 0x800) {
            packet_start = packet = Packet_Alloc(0x21); // match: two calls, the compiler merges them
        } else {
            packet_start = packet = Packet_Alloc(0x1C);
        }
        *packet = 0x10000009;
        packet += 7;
        *packet++ = 0x6C08000C;
        COPY_MATRIX(packet, matrix_a);
        packet += 0x10;
        COPY_MATRIX(packet, matrix_b);
        packet += 0x10;
        *packet = 0x10000002;
        packet += 7;
        *packet++ = 0x6C01003C;
        *(Vector4 *)packet = *(Vector4 *)model->fog_block;
        packet += 4;
        *packet = 0x10000003;
        packet += 7;
        *packet++ = 0x6C02003D;
        *(u64 *)packet = 0x1000000000008001;
        packet += 2;
        *(u64 *)packet = 0xE;
        packet += 2;
        *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
        packet += 2;
        *packet++ = 0x3D;
        *packet++ = 0;
        *packet = 0x1000000A;
        packet += 7;
        *packet = 0x6C090040;
        packet += 4;
        *packet++ = model->hit_count;
        light_out = packet;
        light_out_b = packet + 0x10;
        for (i = 0; i < model->hit_count; i++) {
            *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
            *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
            light_out_b += 4;
            ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
            light_out += 4;
        }
        packet += 0x20;
        if (model->model_flags & 0x800) {
            *packet++ = 0x10000004;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x6C030064;
            Vector_Copy(packet, &model->colour_red);
            packet += 4;
            packet[3] = model->found_record_id;
            ((u_long128 *)packet)[1] = *(u_long128 *)model->found_record;
        }
        Packet_AppendToChain(packet_start, Packet_Alloc(1)); /* the packet ends where the next one would start */
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->scale[0] == 0.0f && part->scale[1] == 0.0f && part->scale[2] == 0.0f) {
                continue;
            }
            Packet_BeginGsRegisters();
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT41) {
                Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
                Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            } else {
                Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
                Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
            }
            Packet_EndGsRegisters();
            packet_start = packet = Packet_Alloc(part->data_count + 7);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            Matrix_Multiply(packet, model->model_matrix, part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
            }
            Packet_AppendToChain(packet_start, packet);
        }
    }
}

/* Draws a model: queues a packet with its two matrices, its MapFogBlock block and its lights,
   then for every part that is switched on and not scaled to nothing the GS state and a packet
   with the part's matrix and a reference to each of its data blocks. */
void Model_DrawPosedWithAlpha(ModelInstance *model) {
    void *matrix_a = MODEL_DRAW_MATRIX_A;
    void *matrix_b = MODEL_DRAW_MATRIX_B;
    u32 *packet;
    u32 *packet_start;
    u32 *light_out;
    u32 *light_out_b;
    ModelPart *part;
    ModelPartData *data;
    s32 i;
    s32 j;
    u32 word;

    if (model->parts != NULL && model->part_count != 0) {
        MODEL_BUILD_DRAW_MATRICES(model, model->light_block, model->light_block + 0x40, matrix_a, matrix_b);
        /* the packet below is 0x1C quadwords either way: with the flag set the last 5 are never written */
        if (model->model_flags & 0x800) {
            packet_start = packet = Packet_Alloc(0x21); // match: two calls, the compiler merges them
        } else {
            packet_start = packet = Packet_Alloc(0x1C);
        }
        *packet = 0x10000009;
        packet += 7;
        *packet++ = 0x6C08000C;
        COPY_MATRIX(packet, matrix_a);
        packet += 0x10;
        COPY_MATRIX(packet, matrix_b);
        packet += 0x10;
        *packet = 0x10000002;
        packet += 7;
        *packet++ = 0x6C01003C;
        *(Vector4 *)packet = *(Vector4 *)model->fog_block;
        packet += 4;
        *packet = 0x10000003;
        packet += 7;
        *packet++ = 0x6C02003D;
        *(u64 *)packet = 0x1000000000008001;
        packet += 2;
        *(u64 *)packet = 0xE;
        packet += 2;
        *(u64 *)packet = ((MapFogBlockValues *)model->fog_block)->fog_colour;
        packet += 2;
        *packet++ = 0x3D;
        *packet++ = 0;
        *packet = 0x1000000A;
        packet += 7;
        *packet = 0x6C090040;
        packet += 4;
        *packet++ = model->hit_count;
        light_out = packet;
        light_out_b = packet + 0x10;
        for (i = 0; i < model->hit_count; i++) {
            *(u_long128 *)light_out = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk00;
            *(u_long128 *)light_out_b = *(u_long128 *)LINKED_POINT_LIGHTS[model->hit_handles[i]].unk10;
            light_out_b += 4;
            ((f32 *)light_out)[3] = LINKED_POINT_LIGHTS[model->hit_handles[i]].unk24;
            light_out += 4;
        }
        Packet_AppendToChain(packet_start, Packet_Alloc(1)); /* the packet ends where the next one would start */
        part = model->parts;
        for (i = 0; i < model->part_count; i++, part++) {
            if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT40) {
                continue;
            }
            if (part->scale[0] == 0.0f && part->scale[1] == 0.0f && part->scale[2] == 0.0f) {
                continue;
            }
            Packet_BeginGsRegisters();
            if (model->fade_enabled == 0) {
                Packet_AddGsRegister(0x1A, 1);
                Packet_AddGsRegister(GS_REG_TEST_1, 0x5000D);
                Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
                if (MODEL_PART_FLAG_WORD(part) & MODEL_PART_BIT41) {
                    Packet_AddGsRegister(GS_REG_ZBUF_1, 0xA000118);
                } else {
                    Packet_AddGsRegister(GS_REG_ZBUF_1, 0x10A000118L);
                }
            } else {
                Packet_AddGsRegister(0x1A, 0);
                Packet_AddGsRegister(0x1B, 0x278);
                data = (ModelPartData *)(u32)part->data_addr;
                Packet_AddGsRegister(0x15, *(s64 *)&data->texture_word);
                Packet_AddGsRegister(0x07, data->tex0);
                Packet_AddGsRegister(GS_REG_ALPHA_2,
                              (s32)((GsAlphaReg *)&data->blend)->f.a_source | ((s32)((GsAlphaReg *)&data->blend)->f.b_source << 2) | GS_ALPHA_C_FIX |
                                  ((s32)((GsAlphaReg *)&data->blend)->f.d_source << 6) | ((s64)(s16)model->fade_alpha << 32));
                Packet_AddGsRegister(GS_REG_TEST_2, 0x5000D);
                if ((s16)model->fade_alpha >= 0x80) {
                    Packet_AddGsRegister(GS_REG_ZBUF_2, 0xA000118);
                } else {
                    Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
                }
            }
            Packet_EndGsRegisters();
            packet_start = packet = Packet_Alloc(part->data_count + 7);
            *packet = 0x10000005;
            packet += 7;
            *packet++ = 0x6C040004;
            Matrix_Multiply(packet, model->model_matrix, part);
            packet += 0x10;
            data = (ModelPartData *)(u32)part->data_addr;
            for (j = 0; j < part->data_count; j++) {
                word = *(u32 *)data;
                *packet++ = (word & ~DMA_TAG_ID_MASK) | DMA_TAG_ID_REF;
                *packet = (u32)data + 0x10;
                packet += 3;
                data = MODEL_PART_DATA_SKIP(data, (word & 0xFFFF) + 1);
            }
            Packet_AppendToChain(packet_start, packet);
        }
        Packet_BeginGsRegisters();
        Packet_AddGsRegister(0x1A, 1);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L);
        Packet_AddGsRegister(GS_REG_ZBUF_2, 0x10A000118L); // match: ZBUF_2 twice; ZBUF_1 is left as the last part set it
        Packet_EndGsRegisters();
    }
}

/* Places the model's 32 bone points: each anchor's offset through its part's matrix and then the model's, or
   the model's position for an anchor without a bone. */
void Model_UpdateBonePoints(ModelInstance *model) {
    f32 point[4];
    int i;
    BoneAnchor *anchor;
    Vector4 *bone_position;

    if (model->bone_anchors != NULL) {
        anchor = model->bone_anchors;
        bone_position = model->bone_positions;
        for (i = 0; i < 32; i++, anchor++, bone_position++) {
            if (anchor->bone != 0xFF) {
                Vector_Copy(point, anchor);
                point[3] = 1.0f;
                Vector_ApplyMatrix(bone_position, model->parts[anchor->bone].matrix, point);
                Vector_ApplyMatrix(bone_position, model->model_matrix, bone_position);
            } else {
                Vector_Copy(bone_position, &model->position);
            }
        }
    }
}

/* Where bone `index` of a model is; the model's own position when it has no bones or there is no such bone. */
f32 *Model_GetBonePoint(ModelInstance *model, int index) {
    if (model->bone_anchors != NULL && index >= 0 && index < 32) {
        return &model->bone_positions[index].x;
    }
    return &model->position.x;
}

/* Rewrites the PRIM field (bits 15-25 of the GIF tag's high word at +0x74) of every data block of every part:
   the bits of clear_mask are cleared, those of set_value set. */
void Model_SetPrimBits(ModelInstance *model, s64 clear_mask, s64 set_value) {
    ModelPart *part;
    char *record;
    char *base;
    int i;
    int j;
    int count;
    int part_count;
    u16 length;
    int old_bits;
    int set_bits;
    int clear_bits;

    set_bits = set_value;
    set_bits &= GS_PRIM_MASK;
    set_bits <<= 15;
    clear_bits = clear_mask;
    clear_bits &= GS_PRIM_MASK;
    clear_bits <<= 15;
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        record = (char *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            base = record;
            length = *(u16 *)base;
            record += 0x70;
            old_bits = ((int *)record)[1] & GIF_TAG_PRIM_MASK;
            ((int *)record)[1] = (old_bits & ~clear_bits) | set_bits | (((int *)record)[1] & ~GIF_TAG_PRIM_MASK);
            record = base + (length + 1) * 0x10;
        }
    }
}

/* Sets TEX0 (+0x40) and TEX1 (+0x30) of every data block of every part. When TEX0 has a CLUT pointer (bits
   37-50), the model's first block gets 2 in its top three bits (CLUT load). */
void Model_SetTexture(ModelInstance *model, u64 unk40, s64 unk30) {
    ModelPart *part;
    ModelPartData *data;
    ModelPartData *block;
    s32 part_count;
    s32 count;
    s32 first;
    s32 i;
    s32 j;
    u16 length;

    first = ((ModelPartDataBits30 *)&unk40)->clut_base != 0;
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < count; j++) {
            block = data;
            length = block->length;
            *(s64 *)&block->texture_word = unk30;
            block->tex0 = unk40;
            if (first) {
                ((ModelPartDataBits30 *)&block->tex0)->clut_load_mode = GS_TEX0_CLD_LOAD_TO_CBP0;
                first = FALSE;
            }
            data = MODEL_PART_DATA_SKIP(block, length + 1);
        }
    }
}

/* Sets the ALPHA register value (+0x50) of every data block of every part. */
void Model_SetAlphaBlend(ModelInstance *model, s64 value) {
    s32 i = 0;
    u16 part_count = model->part_count;
    ModelPart *part = model->parts;

    for (; i < part_count; i++, part++) {
        s32 j = 0;
        s32 count = (s32)(MODEL_PART_WORD(part) >> 53);
        u64 *cursor = *(u64 **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            ModelPartData *block = (ModelPartData *)cursor;
            u16 length = block->length;

            cursor = &block->blend;
            *cursor = value;
            cursor = (u64 *)((char *)block + (length + 1) * 16);
        }
    }
}

/* Sets the VIF MSCALF that ends the first data block of every part: `value` is the VU1 microprogram address
   it starts. */
void Model_SetVu1Program(ModelInstance *model, s32 value) {
    ModelPart *part;
    ModelPartData *data;
    u32 *last_qword;
    s32 i;

    part = model->parts;
    for (i = 0; i < model->part_count; i++, part++) {
        data = (ModelPartData *)(u32)part->data_addr;
        last_qword = (u32 *)((char *)data + data->length * 16);
        last_qword[3] = value | 0x15000000;
    }
}

/* Set the colour of every vertex of a model; a component outside 0..0xFF is left as it is. */
void Model_SetVertexColour(void *model_arg, int red, int green, int blue, int alpha) {
    ModelInstance *model = model_arg;
    s16 scaled[4];
    s32 i = 0;
    u16 part_count = model->part_count;
    ModelPart *part;

    scaled[0] = red << 12;
    scaled[1] = green << 12;
    scaled[2] = blue << 12;
    scaled[3] = alpha << 12;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        s32 j = 0;
        s32 count = (s32)(MODEL_PART_WORD(part) >> 53);
        s32 *cursor = *(s32 **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            ModelPartData *block = (ModelPartData *)cursor;
            u16 length = block->length;
            s32 k = 0;
            s32 vertex_count;
            ModelPartVertex *vertex;

            cursor = &block->vertex_count;
            vertex_count = *cursor & GIF_TAG_NLOOP_MASK;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                if ((u32)red < 0x100) {
                    vertex->colour[0] = red;
                }
                if ((u32)green < 0x100) {
                    vertex->colour[1] = green;
                }
                if ((u32)blue < 0x100) {
                    vertex->colour[2] = blue;
                }
                if ((u32)alpha < 0x100) {
                    vertex->colour[3] = alpha;
                }
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
}

/* Step the colour of every vertex within `radius` of `centre` towards `targets`, one component at
   a time by `steps`. Returns how many components have not arrived yet. */
int Model_FadeVertexColoursNear(int model_arg, void *centre_arg, int *steps, int *targets, float radius) {
    ModelInstance *model = (ModelInstance *)model_arg;
    f32 *centre = centre_arg;
    // match: the order of these declarations decides the stack slots of the four loop cursors (gcse hashes
    // `variable + step` by the variable's register number, and the slots follow that order)
    s32 changed;
    s32 i;
    u16 part_count;
    ModelPart *part;
    ModelPartVertex *vertex;
    s32 j;
    s32 count;
    s32 *cursor;
    ModelPartData *block;
    u16 length;
    s32 k;
    s32 vertex_count;
    f32 distance;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;
    s32 component;

    changed = 0;
    i = 0;
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        j = 0;
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        cursor = *(s32 **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            block = (ModelPartData *)cursor;
            length = block->length;
            k = 0;
            cursor = &block->vertex_count;
            vertex_count = *cursor & GIF_TAG_NLOOP_MASK;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                distance = NOT_MEASURED;
                for (component = 0; component < 4; component++) {
                    if (steps[component] > 0 && vertex->colour[component] < targets[component]) {
                        if (distance == NOT_MEASURED) {
                            delta_x = centre[0] - vertex->x;
                            delta_y = centre[1] - vertex->y;
                            delta_z = centre[2] - vertex->z;
                            distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
                        }
                        if (distance <= radius) {
                            if (vertex->colour[component] + steps[component] < targets[component]) {
                                vertex->colour[component] += steps[component];
                            } else {
                                vertex->colour[component] = targets[component];
                            }
                        }
                        changed++;
                    } else if (steps[component] < 0 && targets[component] < vertex->colour[component]) {
                        if (distance == NOT_MEASURED) {
                            delta_x = centre[0] - vertex->x;
                            delta_y = centre[1] - vertex->y;
                            delta_z = centre[2] - vertex->z;
                            distance = sqrtf(delta_x * delta_x + delta_y * delta_y + delta_z * delta_z);
                        }
                        if (distance <= radius) {
                            if (targets[component] < vertex->colour[component] + steps[component]) {
                                vertex->colour[component] += steps[component];
                            } else {
                                vertex->colour[component] = targets[component];
                            }
                        }
                        changed++;
                    }
                }
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
    return changed;
}

/* Points the TEX0 at +0x40 of every data block of a model at another texture: its base pointer
   becomes texture_base and its CLUT pointer clut_base (0: left as it is). With a CLUT the model's
   first block also gets 2 in the top three bits. */
void Model_SetTextureBase(ModelInstance *model, int texture_base, int clut_base) {
    int i = 0;
    int first = clut_base != 0;
    u16 part_count = model->part_count;
    ModelPart *part = model->parts;

    for (; i < part_count; i++, part++) {
        int j = 0;
        int count = (int)(MODEL_PART_WORD(part) >> 53);
        char *cursor = *(char **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            char *block = cursor;
            u16 length = *(u16 *)block;

            cursor = block + 0x40;
            if (texture_base != 0) {
                ((ModelPartDataBits30 *)cursor)->texture_base = texture_base;
            }
            if (clut_base != 0) {
                ((ModelPartDataBits30 *)cursor)->clut_base = clut_base;
            }
            if (first) {
                first = 0;
                ((ModelPartDataBits30 *)cursor)->clut_load_mode = GS_TEX0_CLD_LOAD_TO_CBP0;
            }
            cursor = block + (length + 1) * 16;
        }
    }
}
