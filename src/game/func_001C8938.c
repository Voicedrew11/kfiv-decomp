extern void *D_004469D4;

void func_001CA338(int);
void *func_001CA520(void *);

typedef struct KfNode {
    char pad00[0xC];
    int f0C;
    char pad10[0x14];
    struct KfNode *f24;
} KfNode;

void func_001C8938(int arg0) {
    KfNode *p;

    p = D_004469D4;
    while (p != 0) {
        if ((p->f0C & 1) == arg0) {
            p = func_001CA520(p);
        } else {
            p = p->f24;
        }
    }
    func_001CA338(arg0);
}
