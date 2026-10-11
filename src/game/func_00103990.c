typedef struct {
    unsigned char bytes[8];
} Row;

extern Row D_0034C350[50];
extern unsigned char D_0034C4E0[50];

void *memset(void *, int, unsigned int);

void func_00103990(unsigned int a, unsigned int b) {
    if (a < 50 && b < 50) {
        D_0034C4E0[b] = D_0034C4E0[a];
        D_0034C350[b] = D_0034C350[a];
        memset(&D_0034C350[a], 0, sizeof(Row));
        D_0034C4E0[a] = 0;
    }
}
