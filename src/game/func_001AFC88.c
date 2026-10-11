void func_00216D20();
void KF_Vector_Scale(float, void *, void *);
void func_00216C48(void *, void *, void *);

void func_001AFC88(void *arg0, float f1, float f2) {
    int tmp[4];
    func_00216D20(tmp);
    KF_Vector_Scale(f2, tmp, tmp);
    func_00216C48(arg0, arg0, tmp);
    func_00216D20(arg0, arg0);
    KF_Vector_Scale(f1, arg0, arg0);
}
