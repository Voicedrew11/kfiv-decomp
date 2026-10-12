typedef struct {
    int f0;
    char pad4[0x40];
    int f44;
    int f48;
    char pad4C[0x4];
    int f50;
} Obj;

void func_0021CCC8(int);
int sceSdRemote(int, ...);

int func_0021C3D8(Obj *arg0) {
    func_0021CCC8(0x7FFF);
    sceSdRemote(1, 0x80E0, 0, 0x13, arg0->f44, (arg0->f48 / 0x400) * 0x400, arg0->f44 + arg0->f50);
    arg0->f0 = 2;
    return 1;
}
