/* Model and part matrices: built from a scale, three angles (wrapped into -pi..pi) and a position with the
   sceVu0 matrix functions, most then multiplied by one of the view matrices through Matrix_Multiply; and the two
   matrices a model is drawn with. Several builders are not called by anything. */
#include "kf_model_matrix.h"
#include "kf_game.h"

extern f32 D_00431AF0[16];          /* the unit matrix */
extern f32 D_00431B30[4][4];
extern f32 D_00431BF0[16];
extern f32 D_00431C30[16];
extern f32 D_004322A0[16];
extern f32 D_004322E0[16];
extern f32 D_004323E0[16];
extern f32 D_00432620[16];
extern CameraState D_004325A0;
extern Vector4Arg D_00441490;

void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);
void sceVu0DivVectorXYZ(void *, void *, float);
void sceVu0SubVector(void *, void *, void *);
void Matrix_Multiply(void *, void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);
void *memset(void *, int, u32);
float atan2f(float, float);
float sqrtf(float);
float sinf(float);
float cosf(float);

#define SCRATCH_MATRIX MODEL_MATRIX_SCRATCH

/* Brings an angle back into -pi..pi. */
static inline f32 ModelMatrix_WrapAngle(f32 angle) {
    angle += 3.1415927f;
    return angle - (f32)(s32)(angle / 6.2831855f) * 6.2831855f - 3.1415927f;
}

/* Builds a part's local matrix from its scale, its three angles (applied x, z, y) and its offset. */
void ModelMatrix_BuildPartLocal(ModelInstance *model, ModelPart *part, ModelPart *dest) {
    f32 *matrix = (f32 *)dest->unk00;

    sceVu0CopyMatrix(matrix, D_00431AF0);
    matrix[0] = part->unk60[0];
    matrix[5] = part->unk60[1];
    matrix[10] = part->unk60[2];
    sceVu0RotMatrixX(matrix, matrix, ModelMatrix_WrapAngle(part->unk50));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(part->unk58));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(part->unk54));
    sceVu0TransMatrix(matrix, matrix, part->unk40);
}

/* Builds a part's rotation matrix (x, z, y), and adds its offset when with_offset is set. */
static void ModelMatrix_BuildPartRotation(ModelInstance *model, ModelPart *part, f32 *matrix, int with_offset) {
    sceVu0RotMatrixX(matrix, D_00431AF0, ModelMatrix_WrapAngle(part->unk50));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(part->unk58));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(part->unk54));
    if (with_offset) {
        sceVu0TransMatrix(matrix, matrix, part->unk40);
    }
}

/* Builds a model's matrix from its rotation and position; flagged models only turn about z and take D_00432620. */
static void ModelMatrix_BuildFromModel(ModelInstance *model) {
    f32 *matrix;

    if (!(model->unk102 & 0x300)) {
        matrix = (f32 *)model->unk40;
        sceVu0RotMatrixX(matrix, D_00431AF0, ModelMatrix_WrapAngle(model->rotation.x));
        sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.y));
        sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    } else {
        matrix = (f32 *)model->unk40;
        sceVu0RotMatrixZ(matrix, D_00431AF0, ModelMatrix_WrapAngle(model->rotation.z));
        Matrix_Multiply(matrix, D_00432620, matrix);
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    }
}

/* ModelMatrix_BuildDrawMatrices with fixed sources: copies D_00431BF0 with its rows divided by the model's
   scale, and D_00431C30. */
static void func_0020BCB8(ModelInstance *model, void *normal_dest, void *light_dest) {
    f32 (*normal)[4] = normal_dest;

    sceVu0CopyMatrix(normal, D_00431BF0);
    sceVu0DivVectorXYZ(normal[0], normal[0], model->unkE0);
    sceVu0DivVectorXYZ(normal[1], normal[1], model->unkE4);
    sceVu0DivVectorXYZ(normal[2], normal[2], model->unkE8);
    sceVu0CopyMatrix(light_dest, D_00431C30);
}

/* Empty. */
static void func_0020BD40(void) {
}

/* Builds a matrix from the model's x and z angles with its y row cleared, turned a quarter about x and pushed
   16 along z, and applies D_004322E0 to it (a shadow or a marker under the model, perhaps). */
static void func_0020BD48(ModelInstance *model) {
    f32 matrix[16];
    f32 offset[4];

    memset(offset, 0, sizeof(offset));
    offset[2] = 16.0f;
    sceVu0RotMatrixX(matrix, D_00431AF0, ModelMatrix_WrapAngle(model->rotation.x));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
    matrix[4] = matrix[5] = matrix[6] = matrix[7] = 0.0f;
    sceVu0RotMatrixX(matrix, matrix, -1.5707964f);
    sceVu0TransMatrix(matrix, matrix, offset);
    Matrix_Multiply(model, D_004322E0, matrix);
}

/* Builds a model's matrix from its scale, rotation and position, then applies D_004323E0 to a copy of it whose
   y row is (-0.707, 0, -0.707, 0). */
static void func_0020BE70(ModelInstance *model) {
    f32 work[16];
    f32 *matrix;

    sceVu0CopyMatrix(work, D_00431AF0);
    matrix = (f32 *)model->unk40;
    work[0] *= model->unkE0;
    work[5] *= model->unkE4;
    work[10] *= model->unkE8;
    sceVu0RotMatrixX(matrix, work, ModelMatrix_WrapAngle(model->rotation.x));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.y));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
    sceVu0TransMatrix(matrix, matrix, &model->position.x);
    sceVu0CopyMatrix(work, matrix);
    work[5] = work[7] = 0.0f;
    work[6] = work[4] = -0.707f;
    Matrix_Multiply(model, D_004323E0, work);
}

/* Builds a model's matrix from a scale, a rotation (x, y, z) and a position, then applies D_004323E0. */
static void ModelMatrix_BuildXYZ(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] *= scale[2];
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &position->x);
    Matrix_Multiply(model, D_004323E0, matrix);
}

/* Builds a model's matrix from a scale, a rotation (z, y, x) and a position, then applies D_004323E0. */
static void ModelMatrix_BuildZYX(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] *= scale[2];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &position->x);
    Matrix_Multiply(model, D_004323E0, matrix);
}

/* Builds a model's matrix from a position, a rotation and a scale, from D_00431B30. Models flagged 0x100 or
   0x200 are turned to face the camera (about x, about y) and keep only their own turn about z (their own
   position and rotation are used then); models flagged 0x400 turn about z alone. */
void ModelMatrix_Build(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 to_camera[4];
    f32 angle;
    f32 *scratch = SCRATCH_MATRIX;

    if (model->unk102 & 0x300) {
        sceVu0SubVector(to_camera, D_004325A0.position, position);
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(model->rotation.z));
        if (model->unk102 & 0x100) {
            sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX,
                             atan2f(to_camera[1], sqrtf(to_camera[0] * to_camera[0] + to_camera[2] * to_camera[2])));
        }
        if (model->unk102 & 0x200) {
            angle = -atan2f(to_camera[2], to_camera[0]) - 1.5707964f;
            if (angle < -3.1415927f) {
                angle += 6.2831855f;
            }
            sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, angle);
        }
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &model->position.x);
    } else if (model->unk102 & 0x400) {
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &position->x);
    } else {
        sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431B30);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
        sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
        sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
        sceVu0TransMatrix((f32 *)model->unk40, SCRATCH_MATRIX, &position->x);
    }
}

/* Builds a model's matrix from a scale (x, y), a turn about z, D_00432620 and a position, then applies
   D_004323E0. */
static void func_0020C6A0(ModelInstance *model, f32 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    Matrix_Multiply(SCRATCH_MATRIX, D_00432620, SCRATCH_MATRIX);
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, position);
    Matrix_Multiply(model, D_004323E0, matrix);
}

/* Builds the matrix of a flat, mirrored model that turns about z and rises with the camera's pitch,
   then applies D_004322A0. */
static void func_0020C7A8(ModelInstance *model, Vector4 *rotation, f32 *scale) {
    Vector4Arg offset = D_00441490;
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, D_00431AF0);
    matrix = (f32 *)model->unk40;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] = -1.0f;
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    offset.y += offset.z * sinf(D_004325A0.unk10);
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &offset.x);
    Matrix_Multiply(model, D_004322A0, matrix);
}

/* Builds the rotation matrix of three angles: a turn by the second, a tilt by the first, a turn by the third. */
static void Matrix_BuildEulerRotation(f32 (*matrix)[4], f32 *angles) {
    f32 cos_tilt = cosf(ModelMatrix_WrapAngle(angles[0]));
    f32 sin_tilt = sinf(ModelMatrix_WrapAngle(angles[0]));
    f32 cos_first = cosf(ModelMatrix_WrapAngle(angles[1]));
    f32 sin_first = sinf(ModelMatrix_WrapAngle(angles[1]));
    f32 cos_last = cosf(ModelMatrix_WrapAngle(angles[2]));
    f32 sin_last = sinf(ModelMatrix_WrapAngle(angles[2]));

    sceVu0CopyMatrix(matrix, D_00431AF0);
    // match: the order of these nine statements decides the order of the multiplies
    matrix[2][2] = cos_tilt * cos_first * cos_last - sin_first * sin_last;
    matrix[2][0] = -cos_tilt * cos_first * sin_last - sin_first * cos_last;
    matrix[0][2] = cos_tilt * sin_first * cos_last + cos_first * sin_last;
    matrix[2][1] = sin_tilt * cos_first;
    matrix[0][0] = -cos_tilt * sin_first * sin_last + cos_first * cos_last;
    matrix[0][1] = sin_tilt * sin_first;
    matrix[1][1] = cos_tilt;
    matrix[1][2] = -sin_tilt * cos_last;
    matrix[1][0] = sin_tilt * sin_last;
}

/* Rotates the three axis rows of a matrix by the rotation part of another (3x3 product) with VU0. */
static void Matrix_MultiplyRotation(void *dest, void *rotation, void *matrix) {
    __asm__ volatile(
        "lqc2 $vf1, 0x0(%1)\n"
        "lqc2 $vf2, 0x10(%1)\n"
        "lqc2 $vf3, 0x20(%1)\n"
        "lqc2 $vf4, 0x30(%1)\n"
        "lqc2 $vf22, 0x0(%2)\n"
        "lqc2 $vf23, 0x10(%2)\n"
        "lqc2 $vf24, 0x20(%2)\n"
        "lqc2 $vf25, 0x30(%2)\n"
        "vmulax.xyz ACC, $vf1, $vf22x\n"
        "vmadday.xyz ACC, $vf2, $vf22y\n"
        "vmaddz.xyz $vf22, $vf3, $vf22z\n"
        "vmulax.xyz ACC, $vf1, $vf23x\n"
        "vmadday.xyz ACC, $vf2, $vf23y\n"
        "vmaddz.xyz $vf23, $vf3, $vf23z\n"
        "vmulax.xyz ACC, $vf1, $vf24x\n"
        "vmadday.xyz ACC, $vf2, $vf24y\n"
        "vmaddz.xyz $vf24, $vf3, $vf24z\n"
        "sqc2 $vf22, 0x0(%0)\n"
        "sqc2 $vf23, 0x10(%0)\n"
        "sqc2 $vf24, 0x20(%0)\n"
        : : "r"(dest), "r"(rotation), "r"(matrix));
}

/* Transforms src_vec by src_matrix without its translation, into dst. */
static void Matrix_RotateVector(void *dst, void *src_matrix, void *src_vec) {
    f32 *scratch = MODEL_MATRIX_SCRATCH;

    sceVu0CopyMatrix(scratch, src_matrix);
    scratch[12] = 0.0f;
    scratch[13] = 0.0f;
    scratch[14] = 0.0f;
    Vector_ApplyMatrix(dst, scratch, src_vec);
}

/* Makes the two matrices a model is drawn with: the first with its rows divided by the model's scale;
   the second, for models flagged 0x400, emptied but for the model's colour times its alpha. */
void ModelMatrix_BuildDrawMatrices(ModelInstance *model, s32 normal_src, s32 light_src, void *normal_dest,
                                   void *light_dest) {
    f32 (*normal)[4] = normal_dest;
    f32 (*light)[4] = light_dest;

    sceVu0CopyMatrix(normal, (void *)normal_src);
    sceVu0DivVectorXYZ(normal[0], normal[0], model->unkE0);
    sceVu0DivVectorXYZ(normal[1], normal[1], model->unkE4);
    sceVu0DivVectorXYZ(normal[2], normal[2], model->unkE8);
    sceVu0CopyMatrix(light, (void *)light_src);
    if (model->unk100 & 0x400) {
        light[0][0] = 0.0f;
        light[0][1] = 0.0f;
        light[0][2] = 0.0f;
        light[1][0] = 0.0f;
        light[1][1] = 0.0f;
        light[1][2] = 0.0f;
        light[2][0] = 0.0f;
        light[2][1] = 0.0f;
        light[2][2] = 0.0f;
        light[3][0] = model->unk150 / 255.0f * model->alpha / 255.0f;
        light[3][1] = model->unk154 / 255.0f * model->alpha / 255.0f;
        light[3][2] = model->unk158 / 255.0f * model->alpha / 255.0f;
    }
}
