/* Model and part matrices: built from a scale, three angles (wrapped into -pi..pi) and a position with the
   sceVu0 matrix functions, most then multiplied by one of the view matrices through Matrix_Multiply; and the two
   matrices a model is drawn with. Several builders are not called by anything. */
#include "model/model_matrix.h"
#include "math/camera.h"
#include "core/game.h"
#include "core/file.h"
#include <libvu0.h>
#include <math.h>
#include "math/vu0.h"


#define SCRATCH_MATRIX MODEL_MATRIX_SCRATCH

/* Brings an angle back into -pi..pi. */
static inline f32 ModelMatrix_WrapAngle(f32 angle) {
    angle += PI;
    return angle - (f32)(s32)(angle / TWO_PI) * TWO_PI - PI;
}

/* Builds a part's local matrix from its scale, its three angles (applied x, z, y) and its offset. */
void ModelMatrix_BuildPartLocal(ModelInstance *model, ModelPart *part, ModelPart *dest) {
    f32 *matrix = dest->matrix;

    sceVu0CopyMatrix(matrix, identity_matrix);
    matrix[0] = part->scale[0];
    matrix[5] = part->scale[1];
    matrix[10] = part->scale[2];
    sceVu0RotMatrixX(matrix, matrix, ModelMatrix_WrapAngle(part->rotation_x));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(part->rotation_z));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(part->rotation_y));
    sceVu0TransMatrix(matrix, matrix, part->position);
}

/* Builds a part's rotation matrix (x, z, y), and adds its offset when with_offset is set. */
static void ModelMatrix_BuildPartRotation(ModelInstance *model, ModelPart *part, f32 *matrix, int with_offset) {
    sceVu0RotMatrixX(matrix, identity_matrix, ModelMatrix_WrapAngle(part->rotation_x));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(part->rotation_z));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(part->rotation_y));
    if (with_offset) {
        sceVu0TransMatrix(matrix, matrix, part->position);
    }
}

/* Builds a model's matrix from its rotation and position; flagged models only turn about z and take flagged_model_transform_matrix. */
static void ModelMatrix_BuildFromModel(ModelInstance *model) {
    f32 *matrix;

    if (!(model->draw_flags & MODEL_DRAW_FACE_CAMERA)) {
        matrix = model->model_matrix;
        sceVu0RotMatrixX(matrix, identity_matrix, ModelMatrix_WrapAngle(model->rotation.x));
        sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.y));
        sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    } else {
        matrix = model->model_matrix;
        sceVu0RotMatrixZ(matrix, identity_matrix, ModelMatrix_WrapAngle(model->rotation.z));
        Matrix_Multiply(matrix, flagged_model_transform_matrix, matrix);
        sceVu0TransMatrix(matrix, matrix, &model->position.x);
    }
}

/* ModelMatrix_BuildDrawMatrices with fixed sources: copies fixed_normal_matrix with its rows divided by the model's
   scale, and fixed_light_matrix. */
static void ModelMatrix_CopyFixedDrawMatrices(ModelInstance *model, void *normal_dest, void *light_dest) {
    f32 (*normal)[4] = normal_dest;

    sceVu0CopyMatrix(normal, fixed_normal_matrix);
    sceVu0DivVectorXYZ(normal[0], normal[0], model->scale_x);
    sceVu0DivVectorXYZ(normal[1], normal[1], model->scale_y);
    sceVu0DivVectorXYZ(normal[2], normal[2], model->scale_z);
    sceVu0CopyMatrix(light_dest, fixed_light_matrix);
}

/* Empty. */
static void ModelMatrix_UnusedStub(void) {
}

/* Builds a matrix from the model's x and z angles with its y row cleared, turned a quarter about x and pushed
   16 along z, and applies model_extra_transform_matrix to it into the model's matrix. Nothing calls it. */
static void ModelMatrix_BuildFlattenedTilt(ModelInstance *model) {
    f32 matrix[16];
    f32 offset[4];

    memset(offset, 0, sizeof(offset));
    offset[2] = 16.0f;
    sceVu0RotMatrixX(matrix, identity_matrix, ModelMatrix_WrapAngle(model->rotation.x));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
    matrix[4] = matrix[5] = matrix[6] = matrix[7] = 0.0f;
    sceVu0RotMatrixX(matrix, matrix, -HALF_PI);
    sceVu0TransMatrix(matrix, matrix, offset);
    Matrix_Multiply(model, model_extra_transform_matrix, matrix);
}

/* Builds a model's matrix from its scale, rotation and position, then applies world_screen_matrix to a copy of it whose
   y row is (-0.707, 0, -0.707, 0). */
static void ModelMatrix_BuildFlattenedSheared(ModelInstance *model) {
    f32 work[16];
    f32 *matrix;

    sceVu0CopyMatrix(work, identity_matrix);
    matrix = model->model_matrix;
    work[0] *= model->scale_x;
    work[5] *= model->scale_y;
    work[10] *= model->scale_z;
    sceVu0RotMatrixX(matrix, work, ModelMatrix_WrapAngle(model->rotation.x));
    sceVu0RotMatrixY(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.y));
    sceVu0RotMatrixZ(matrix, matrix, ModelMatrix_WrapAngle(model->rotation.z));
    sceVu0TransMatrix(matrix, matrix, &model->position.x);
    sceVu0CopyMatrix(work, matrix);
    work[5] = work[7] = 0.0f;
    work[6] = work[4] = -0.707f;
    Matrix_Multiply(model, world_screen_matrix, work);
}

/* Builds a model's matrix from a scale, a rotation (x, y, z) and a position, then applies world_screen_matrix. */
static void ModelMatrix_BuildXYZ(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, identity_matrix);
    matrix = model->model_matrix;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] *= scale[2];
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &position->x);
    Matrix_Multiply(model, world_screen_matrix, matrix);
}

/* Builds a model's matrix from a scale, a rotation (z, y, x) and a position, then applies world_screen_matrix. */
static void ModelMatrix_BuildZYX(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, identity_matrix);
    matrix = model->model_matrix;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] *= scale[2];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
    sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &position->x);
    Matrix_Multiply(model, world_screen_matrix, matrix);
}

/* Builds a model's matrix from a position, a rotation and a scale, from scale_matrices. Models flagged 0x100 or
   0x200 are turned to face the camera (about x, about y) and keep only their own turn about z (their own
   position and rotation are used then); models flagged 0x400 turn about z alone. */
void ModelMatrix_Build(ModelInstance *model, Vector4 *position, Vector4 *rotation, f32 *scale) {
    f32 to_camera[4];
    f32 angle;
    f32 *scratch = SCRATCH_MATRIX;

    if (model->draw_flags & MODEL_DRAW_FACE_CAMERA) {
        sceVu0SubVector(to_camera, camera_state.position, position);
        sceVu0CopyMatrix(SCRATCH_MATRIX, scale_matrices);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(model->rotation.z));
        if (model->draw_flags & MODEL_DRAW_FACE_CAMERA_X) {
            sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX,
                             atan2f(to_camera[1], sqrtf(to_camera[0] * to_camera[0] + to_camera[2] * to_camera[2])));
        }
        if (model->draw_flags & MODEL_DRAW_FACE_CAMERA_Y) {
            angle = -atan2f(to_camera[2], to_camera[0]) - HALF_PI;
            if (angle < -PI) {
                angle += TWO_PI;
            }
            sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, angle);
        }
        sceVu0TransMatrix(model->model_matrix, SCRATCH_MATRIX, &model->position.x);
    } else if (model->draw_flags & MODEL_DRAW_TURN_Z_ONLY) {
        sceVu0CopyMatrix(SCRATCH_MATRIX, scale_matrices);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
        sceVu0TransMatrix(model->model_matrix, SCRATCH_MATRIX, &position->x);
    } else {
        sceVu0CopyMatrix(SCRATCH_MATRIX, scale_matrices);
        scratch[0] *= scale[0];
        scratch[5] *= scale[1];
        scratch[10] *= scale[2];
        sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
        sceVu0RotMatrixX(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->x));
        sceVu0RotMatrixY(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->y));
        sceVu0TransMatrix(model->model_matrix, SCRATCH_MATRIX, &position->x);
    }
}

/* Builds a model's matrix from a scale (x, y), a turn about z, flagged_model_transform_matrix and a position, then applies
   world_screen_matrix. */
static void ModelMatrix_BuildFlatScaled(ModelInstance *model, f32 *position, Vector4 *rotation, f32 *scale) {
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, identity_matrix);
    matrix = model->model_matrix;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    Matrix_Multiply(SCRATCH_MATRIX, flagged_model_transform_matrix, SCRATCH_MATRIX);
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, position);
    Matrix_Multiply(model, world_screen_matrix, matrix);
}

/* Builds the matrix of a flat, mirrored model that turns about z and rises with the camera's pitch,
   then applies view_screen_matrix. */
static void ModelMatrix_BuildMirroredFlat(ModelInstance *model, Vector4 *rotation, f32 *scale) {
    Vector4Arg offset = mirrored_flat_offset;
    f32 *scratch = SCRATCH_MATRIX;
    f32 *matrix;

    sceVu0CopyMatrix(SCRATCH_MATRIX, identity_matrix);
    matrix = model->model_matrix;
    scratch[0] *= scale[0];
    scratch[5] *= scale[1];
    scratch[10] = -1.0f;
    sceVu0RotMatrixZ(SCRATCH_MATRIX, SCRATCH_MATRIX, ModelMatrix_WrapAngle(rotation->z));
    offset.y += offset.z * sinf(camera_state.pitch);
    sceVu0TransMatrix(matrix, SCRATCH_MATRIX, &offset.x);
    Matrix_Multiply(model, view_screen_matrix, matrix);
}

/* Builds the rotation matrix of three angles: a turn by the second, a tilt by the first, a turn by the third. */
static void Matrix_BuildEulerRotation(f32 (*matrix)[4], f32 *angles) {
    f32 cos_tilt = cosf(ModelMatrix_WrapAngle(angles[0]));
    f32 sin_tilt = sinf(ModelMatrix_WrapAngle(angles[0]));
    f32 cos_first = cosf(ModelMatrix_WrapAngle(angles[1]));
    f32 sin_first = sinf(ModelMatrix_WrapAngle(angles[1]));
    f32 cos_last = cosf(ModelMatrix_WrapAngle(angles[2]));
    f32 sin_last = sinf(ModelMatrix_WrapAngle(angles[2]));

    sceVu0CopyMatrix(matrix, identity_matrix);
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
   the second, with MODEL_FLAG_FLAT_COLOUR, emptied but for the model's colour times its alpha. */
void ModelMatrix_BuildDrawMatrices(ModelInstance *model, void *normal_src, void *light_src, void *normal_dest,
                                   void *light_dest) {
    f32 (*normal)[4] = normal_dest;
    f32 (*light)[4] = light_dest;

    sceVu0CopyMatrix(normal, normal_src);
    sceVu0DivVectorXYZ(normal[0], normal[0], model->scale_x);
    sceVu0DivVectorXYZ(normal[1], normal[1], model->scale_y);
    sceVu0DivVectorXYZ(normal[2], normal[2], model->scale_z);
    sceVu0CopyMatrix(light, light_src);
    if (model->model_flags & MODEL_FLAG_FLAT_COLOUR) {
        light[0][0] = 0.0f;
        light[0][1] = 0.0f;
        light[0][2] = 0.0f;
        light[1][0] = 0.0f;
        light[1][1] = 0.0f;
        light[1][2] = 0.0f;
        light[2][0] = 0.0f;
        light[2][1] = 0.0f;
        light[2][2] = 0.0f;
        light[3][0] = model->colour_red / 255.0f * model->alpha / 255.0f;
        light[3][1] = model->colour_green / 255.0f * model->alpha / 255.0f;
        light[3][2] = model->colour_blue / 255.0f * model->alpha / 255.0f;
    }
}
