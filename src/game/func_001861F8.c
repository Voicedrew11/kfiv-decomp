extern void KF_Vector_Subtract(float *, int, float *);
extern int sceVu0Normalize(int, void *);

void func_001861F8(int arg0, float *arg1, float arg2, float arg3) {
    float v[4];
    float temp_f20;
    float temp_f21;

    temp_f20 = arg2 + arg3;
    *(int *)0x70000010 = 0;
    KF_Vector_Subtract(v, arg0, arg1);
    temp_f21 = temp_f20;
    temp_f20 = temp_f20 * temp_f20;
    if (!(temp_f20 <= v[0] * v[0] + v[1] * v[1] + v[2] * v[2])) {
        *(int *)0x70000010 = 1;
        sceVu0Normalize(0x70000020, v);
        *(float *)0x70000000 = arg1[0] + *(float *)0x70000020 * temp_f21;
        *(float *)0x70000004 = arg1[1] + *(float *)0x70000024 * temp_f21;
        *(float *)0x70000008 = arg1[2] + *(float *)0x70000028 * temp_f21;
    }
}
