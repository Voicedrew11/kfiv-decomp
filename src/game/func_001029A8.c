typedef struct {
    char unk0[0x3C];
    unsigned char unk3C;
    char unk3D[3];
} Def40;

typedef struct {
    char unk0[0x12];
    unsigned char unk12;
    char unk13[0xF];
} Rec22;

typedef struct {
    char unk0[0x17C];
} Blk17C;

typedef struct {
    unsigned char unk0;
    unsigned char unk1;
    short unk2;
    unsigned char unk4;
    unsigned char unk5;
    short unk6;
    char unk8[0xC];
} Cfg14;

extern struct {
    char unk0[0x2580];
    Def40 a[70];
    Def40 b[160];
} D_004147F8;

extern char D_00349690[0x2000];
extern char D_0034B690[0x40];
extern char D_0034B6D0[0xC80];
extern char D_0034C350[0x190];
extern char D_0034C4E0[0x32];
extern char D_0034C518[0x400];
extern char D_0034C918[0x10];
extern char D_0034C928[0x8000];
extern char D_00354928[0x2000];
extern char D_00356928[0x2000];
extern char D_00358928[0xA000];
extern char D_00362928[0x5000];
extern Cfg14 D_00367928;
extern char D_00367940[0x3C];
extern char D_00367980[0x1E];
extern Rec22 D_003679A0[70];
extern Rec22 D_003682F0[160];
extern Blk17C D_00369830;
extern Blk17C D_003699B0;
extern Blk17C D_00369B30;
extern Blk17C D_00369CB0;
extern Blk17C D_00369E30;
extern Blk17C D_00369FB0;
extern char D_0038A130[0x28];
extern const Blk17C D_0043D9B0;
extern const Blk17C D_0043DB30;
extern const Blk17C D_0043DCB0;
extern const Blk17C D_0043DE30;
extern const Blk17C D_0043DFB0;

void *memset(void *, int, unsigned int);
void func_001DC0E0(void);

void func_001029A8(void) {
    int i;

    memset(D_00349690, 0, sizeof(D_00349690));
    memset(D_0034B690, 0, sizeof(D_0034B690));
    memset(D_0034B6D0, 0, sizeof(D_0034B6D0));
    memset(D_0034C350, 0, sizeof(D_0034C350));
    memset(D_0034C4E0, 0, sizeof(D_0034C4E0));
    D_0034C4E0[0] = 1;
    memset(D_0034C518, 0, sizeof(D_0034C518));
    memset(D_0034C918, 0, sizeof(D_0034C918));
    memset(D_0034C928, 0, sizeof(D_0034C928));
    memset(D_00354928, 0, sizeof(D_00354928));
    memset(D_00356928, 0, sizeof(D_00356928));
    memset(D_00358928, 0, sizeof(D_00358928));
    memset(D_00362928, 0, sizeof(D_00362928));
    memset(&D_00367928, 0, sizeof(D_00367928));
    D_00367928.unk0 = 0x66;
    D_00367928.unk1 = 200;
    D_00367928.unk2 = 0x87;
    D_00367928.unk4 = 0xD;
    D_00367928.unk5 = 200;
    D_00367928.unk6 = 0x88;
    memset(D_003679A0, 0, sizeof(D_003679A0));
    memset(D_003682F0, 0, sizeof(D_003682F0));
    for (i = 0; i < 70; i++) {
        D_003679A0[i].unk12 = D_004147F8.a[i].unk3C;
    }
    for (i = 0; i < 160; i++) {
        D_003682F0[i].unk12 = D_004147F8.b[i].unk3C;
    }
    memset(&D_00369830, 0, sizeof(D_00369830));
    D_003699B0 = D_0043D9B0;
    D_00369B30 = D_0043DB30;
    D_00369CB0 = D_0043DCB0;
    D_00369E30 = D_0043DE30;
    D_00369FB0 = D_0043DFB0;
    memset(D_00367940, 0, sizeof(D_00367940));
    memset(D_00367980, 0, sizeof(D_00367980));
    func_001DC0E0();
    memset(D_0038A130, 0, sizeof(D_0038A130));
}
