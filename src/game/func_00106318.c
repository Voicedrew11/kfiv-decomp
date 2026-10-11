typedef float Mat[4][4] __attribute__((aligned(16)));
typedef float Vec[4] __attribute__((aligned(16)));
typedef int IVec[4] __attribute__((aligned(16)));

extern unsigned int D_0038A158[8];
extern int D_0038A178[8];
extern int D_00446744;
extern Mat D_00431AF0;
extern Mat D_00432320;

void *memset(void *, int, unsigned int);
void sceVu0TransMatrix(Mat, Mat, Vec);
void sceVu0RotMatrixX(Mat, Mat, float);
void sceVu0RotMatrixY(Mat, Mat, float);
void sceVu0ViewScreenMatrix(Mat, float, float, float, float, float, float, float, float, float);
void sceVu0MulMatrix(Mat, Mat, Mat);
void func_00216798(Mat);
void func_002167E8(IVec, Vec);

void func_00106318(void) {
    Mat m;
    Mat work;
    Mat view;
    Vec tv;
    IVec ov;
    int i;

    memset(tv, 0, sizeof(tv));
    tv[3] = 1.0f;
    sceVu0TransMatrix(work, D_00431AF0, tv);
    sceVu0RotMatrixY(work, D_00432320, 0.0f);
    sceVu0RotMatrixX(work, D_00432320, 0.0f);
    sceVu0ViewScreenMatrix(view, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 1.0f, 1048560.0f, 1.0f, 65535.0f);
    sceVu0MulMatrix(m, view, work);
    sceVu0MulMatrix(m, m, D_00431AF0);
    func_00216798(m);
    D_00446744 = 0;
    for (i = 0; i < 8; i++) {
        if (D_0038A158[i] == 0) {
            break;
        }
        if (i > 0 && D_0038A158[i] >= D_0038A158[i - 1]) {
            break;
        }
        tv[2] = D_0038A158[i];
        func_002167E8(ov, tv);
        D_0038A178[i] = ov[2] = ov[2] / 16;
        D_00446744++;
    }
}
