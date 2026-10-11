typedef struct {
    char unk0[0x20];
    int unk20;
    int unk24;
    char unk28[0x14];
    unsigned char unk3C;
    unsigned char unk3D;
    unsigned char unk3E;
    unsigned char unk3F[7];
    char unk46[0x6E];
    int unkB4;
    int unkB8;
} State;

typedef struct {
    char unk0[0xE];
    unsigned short flags;
    char unk10[0x10];
} Def20;


extern State D_00413F80;
extern Def20 D_0041A6F8[150];
extern unsigned char D_00369830[380];
extern unsigned char D_00367980[6][5];
extern unsigned short D_00367940[6][5];

void func_001030B8(void) {
    int i;
    int j;
    int n;

    if (D_00413F80.unk3C != 0) {
        D_00369830[D_00413F80.unk3C + 150] = 1;
    }
    if (D_00413F80.unk3E != 0) {
        D_00369830[D_00413F80.unk3E + 220] = 1;
    }
    if (D_00413F80.unk3F[0] != 30) {
        D_00369830[D_00413F80.unk3F[0] + 220] = 1;
    }
    if (D_00413F80.unk3F[1] != 55) {
        D_00369830[D_00413F80.unk3F[1] + 220] = 1;
    }
    if (D_00413F80.unk3F[2] != 80) {
        D_00369830[D_00413F80.unk3F[2] + 220] = 1;
    }
    if (D_00413F80.unk3F[3] != 105) {
        D_00369830[D_00413F80.unk3F[3] + 220] = 1;
    }
    if (D_00413F80.unk3F[4] != 130) {
        D_00369830[D_00413F80.unk3F[4] + 220] = 1;
    }
    if (D_00413F80.unk3F[5] != 140) {
        D_00369830[D_00413F80.unk3F[5] + 220] = 1;
    }
    if (D_00413F80.unk3F[6] != 150) {
        D_00369830[D_00413F80.unk3F[6] + 220] = 1;
    }
    for (i = 0; i < 150; i++) {
        if (D_00369830[i] >= 2 && (D_0041A6F8[i].flags >> 1 & 1)) {
            D_00369830[i] = 1;
        }
    }
    for (i = 150; i < 220; i++) {
        if (D_00369830[i] >= 2) {
            D_00369830[i] = 1;
        }
    }
    for (i = 220; i < 380; i++) {
        if (D_00369830[i] >= 2) {
            D_00369830[i] = 1;
        }
    }
    D_00369830[0] = 1;
    D_00369830[150] = 1;
    D_00369830[220] = 1;
    D_00369830[250] = 1;
    D_00369830[275] = 1;
    D_00369830[300] = 1;
    D_00369830[325] = 1;
    D_00369830[350] = 1;
    D_00369830[360] = 1;
    D_00369830[370] = 1;
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (D_00367980[i][j] == 0) {
                for (; j < 5; j++) {
                    D_00367980[i][j] = 0;
                }
            }
        }
    }
    /* through an int: a bare byte compare is narrowed and gives bnez, not bgtz */
    if (D_00413F80.unk3D == 0 || (n = D_00367980[(D_00413F80.unk3D - 1) / 15][(D_00413F80.unk3D - 1) / 3 % 5]) <= 0) {
        D_00413F80.unk3D = 0;
    }
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 5; j++) {
            if (D_00367940[i][j] == 0) {
                switch (D_00367980[i][j]) {
                case 0:
                    D_00367940[i][j] = 0;
                    break;
                case 1:
                    D_00367940[i][j] = 0;
                    break;
                case 2:
                    D_00367940[i][j] = 50;
                    break;
                case 3:
                    D_00367940[i][j] = 150;
                    break;
                default:
                    D_00367940[i][j] = 0;
                    D_00367980[i][j] = 0;
                    break;
                }
            }
        }
    }
    D_00413F80.unkB4 = D_00413F80.unk20;
    D_00413F80.unkB8 = D_00413F80.unk24;
}
