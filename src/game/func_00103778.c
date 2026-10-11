extern unsigned char D_0034C928[0x8000];

void func_00103778(int arg0, int arg1, int arg2, int arg3) {
    if ((unsigned int)arg0 < 0x80 && (unsigned int)arg1 < 0x200 && (unsigned int)arg2 < 4) {
        unsigned char mask = (unsigned char)(1 << arg2);
        unsigned char *p;
        mask = (unsigned char)(mask << ((arg1 % 2) * 4));
        p = &D_0034C928[arg1 / 2 + arg0 * 0x100];
        *p = *p & ~mask;
        if (arg3 != 0) {
            *p = *p | mask;
        }
    }
}
