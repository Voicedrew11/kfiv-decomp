float atan2f(float, float);
float sqrtf(float);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
extern float D_00413F40[];
extern int D_00413F30[];

typedef struct {
    char pad0[0x1B4];
    void *(*fn1B4)(void *);
    void *(*fn1B8)(void *, int);
} Dev;

extern Dev *D_00446978;

void func_001B7DF8(void) {
    float vec[4];
    void *r0;
    void *r1;

    D_00446978->fn1B4(D_00446978);
    r0 = D_00446978->fn1B8(D_00446978, 0);
    r1 = D_00446978->fn1B8(D_00446978, 1);
    KF_Vector_Subtract(vec, r1, r0);
    func_00216778(D_00413F30, r1);
    D_00413F40[0] = atan2f(-vec[1], sqrtf(vec[0] * vec[0] + vec[2] * vec[2]));
    D_00413F40[1] = atan2f(vec[0], vec[2]);
}
