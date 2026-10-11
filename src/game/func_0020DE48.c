void KF_Vector_Subtract(void *, void *, void *);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(void *, void *, float);
void func_00216D20(void *, void *);
float sqrtf(float);

void func_0020DE48(void *arg0, void *arg1, float arg2, float arg3) {
    float v[3];
    float len;
    float dd;

    dd = arg2 + arg3;
    *(int *)0x70000010 = 0;
    KF_Vector_Subtract(v, arg1, arg0);
    dd = dd * dd;
    len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (!(dd < len)) {
        len = sqrtf(len);
        func_00216D20((void *)0x70000020, v);
        KF_Vector_Scale((void *)0x70000000, (void *)0x70000020, len);
        func_00216C48((void *)0x70000000, (void *)0x70000000, arg0);
        *(float *)0x70000014 = len;
        *(int *)0x70000010 = 1;
    }
}
