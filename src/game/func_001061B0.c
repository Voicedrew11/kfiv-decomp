extern int D_00447000[2];
extern int D_00447010;
extern int D_00447014;
extern int D_00468180[];

void func_001061B0(int arg0, int arg1, int arg2, int arg3) {
    int *p = D_00468180;
    p[0] = arg0;
    D_00447000[3] = -1;
    p[1] = arg1;
    p[2] = arg2;
    D_00447014 = (arg3 > 0) ? arg3 : 1;
    D_00447010 = arg3;
}
