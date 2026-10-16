#include "common.h"
#include "kf_model.h"

/* Matrix and vector scratch the effect, model and draw code shares. */

f32 identity_matrix[0x10] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
}; /* 0x431AF0 identity */

f32 scale_matrices[0x30] = {
    1.00014997f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.00014997f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
    1.20000005f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.20000005f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.20000005f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,
}; /* 0x431B30 a scale by 1.00015 in x and z, a scale by 1.2 and a zero matrix; the readers copy only the first */

f32 fixed_normal_matrix[0x10] = {
    0.731999993f, -0.488000005f, -0.317000002f, 0.0f,
    -0.488000005f, 0.365999997f, 0.244000003f, 0.0f,
    -0.365999997f, -0.792999983f, 0.731999993f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
}; /* 0x431BF0 a turn */

f32 fixed_light_matrix[0x14] = {
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f,
    100000000.0f, -100000000.0f, 100000000.0f, 1.0f,
}; /* 0x431C30 three ones and a scale row */

Vector4 origin_point = {
    0.0f, 0.0f, 0.0f, 1.0f,
}; /* 0x431C80 the w axis */

u32 D_00431C90[0x16]                         = { 0 }; /* 0x431C90 zero fill */
