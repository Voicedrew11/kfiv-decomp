#include "kf_map.h"
#include "kf_collision.h"

extern MapSlot D_0038CD20[];
extern f32 D_00431AF0[16];
extern f32 D_004323E0[16];

void func_00216778(void *, void *);
void func_00216B48(int, int *, int);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);

/* Puts the model of every record of a map where the record says, and builds its matrices. */
void func_001155F8(int map) {
    MapSlot *slot = &D_0038CD20[map];
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
            func_00216778(&model->position, record->position);
            model->position.w = 1.0f;
            func_00216778(&model->rotation, record->rotation);
            model->rotation.w = 1.0f;
            func_00216778(&model->unkE0, record->scale);
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
            func_00216B48((int)model, (int *)D_004323E0, (int)matrix);
            for (j = 0; j < model->part_count; j++) {
                // match: the callee returns a value here, which keeps $2 busy
                ((int (*)(ModelInstance *, ModelPart *, ModelPart *))func_0020B8B0)(model, &model->parts[j], &model->parts[j]);
            }
            sceVu0CopyMatrix(model->unk80, matrix);
        }
    }
}
