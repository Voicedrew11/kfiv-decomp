unsigned short func_001090E0(char *arg0) {
    char *p;
    char **slot;

    p = *(char **)(arg0 + 0xFC);
    slot = (char **)(p + (unsigned char)p[0x81] * 4);
    return *(unsigned short *)(*slot - 4);
}
