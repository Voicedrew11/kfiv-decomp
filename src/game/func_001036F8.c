extern unsigned char D_0034C928[0x8000];

int func_001036F8(int arg0, int arg1, int arg2) {
    unsigned int mask = 1;
    if ((unsigned int)arg0 >= 0x80) {
        return 0;
    }
    if ((unsigned int)arg1 >= 0x200) {
        return 0;
    }
    if ((unsigned int)arg2 >= 4) {
        return 0;
    }
    mask = (mask << arg2) & 0xFF;
    mask = (mask << ((arg1 % 2) * 4)) & 0xFF;
    return (D_0034C928[arg1 / 2 + arg0 * 0x100] & mask) != 0;
}
