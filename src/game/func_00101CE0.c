typedef struct {
    char pad00[0x68];
    int a68;
} Blk;

extern Blk D_0038A1B0[];
void func_001095E0(void);
void func_0010B000(void);
void func_001096E8(void);
void func_00109800(void);
void Display_SwapBuffers(int);
int func_001005E8(int);

void func_00101CE0(void) {
    int temp_s1;

    func_001095E0();
    func_001095E0();
    func_0010B000();
    temp_s1 = D_0038A1B0->a68;
    D_0038A1B0->a68 = 0;
    func_001005E8(0);
    D_0038A1B0->a68 = temp_s1;
    func_00109800();
    Display_SwapBuffers(0);
    func_001096E8();
    func_00109800();
}
