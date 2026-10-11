extern unsigned char D_00354928[0x80][0x40];

int func_00103548(int arg0, int arg1) {
    int v;

    if ((unsigned int)arg0 < 0x80U && arg1 >= 0 && arg1 < 0x80) {
        v = D_00354928[arg0][arg1 / 2];
        if (arg1 & 1) {
            v >>= 4;
        } else {
            v &= 0xF;
        }
        return v;
    }
    return 0;
}
