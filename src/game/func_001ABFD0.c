extern int D_00413F80[];

void func_001AF928(void);
void func_001AF640(void *, int);
void KF_Vector_Subtract(void *, void *, void *);
void func_00216778(void *, void *);
void func_001C1430(float, void *, void *);
void func_001AF5B0(void *, void *);
float func_001B0178(void *, void *);
void func_001AF538(float, void *, void *);

void func_001ABFD0(char *arg0, int arg1) {
    int vec[4];
    char *temp_s0;
    char *temp_s2;

    temp_s2 = arg0 + 0x10;
    func_001AF928();
    func_001AF640(arg0, arg1);
    *(float *)(temp_s2 + 0x20) = 0;
    KF_Vector_Subtract(vec, D_00413F80, *(char **)(arg0 + 0x50) + 0xC0);
    func_001C1430(1.0f, *(char **)(arg0 + 0x50) + 0xC0, vec);
    if (*(int *)0x70000010 != 0) {
        func_00216778(arg0 + 0x20, (void *)0x70000000);
    } else {
        func_00216778(arg0 + 0x20, D_00413F80);
        *(float *)(temp_s2 + 0x14) = *(float *)(temp_s2 + 0x14) - 180.0f;
    }
    temp_s0 = temp_s2 + 0x10;
    KF_Vector_Subtract(vec, temp_s0, *(char **)(arg0 + 0x50) + 0xC0);
    func_001AF5B0(*(char **)(arg0 + 0x50) + 0xD0, vec);
    *(float *)(temp_s2 + 0x20) = func_001B0178(temp_s0, *(char **)(arg0 + 0x50) + 0xC0);
    func_001AF538(100.0f, temp_s2, *(char **)(arg0 + 0x50) + 0xD0);
}
