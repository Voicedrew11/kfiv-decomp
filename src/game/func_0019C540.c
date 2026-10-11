extern void func_0019E758(void);
extern void func_0019DC00(void);
extern int func_001A14D8(void);
extern int func_001CBAC0(void);
extern void func_001CEBE0(void);
extern char D_004470E2;

void func_0019C540(void) {
    func_0019E758();
    if (D_004470E2 == 0) {
        func_0019DC00();
        func_001A14D8();
        func_001CEBE0();
        return;
    }
    if (func_001CBAC0() == 0) {
        D_004470E2 = 0;
    }
}
