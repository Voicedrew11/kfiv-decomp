extern unsigned char D_0034C918[];

int func_001034A8(int arg0) {
    if ((unsigned int)arg0 < 0x80U) {
        return D_0034C918[arg0 / 8] >> (arg0 % 8) & 1;
    }
    return 0;
}
