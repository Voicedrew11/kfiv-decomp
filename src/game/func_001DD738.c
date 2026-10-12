#include "kf_link.h"
#include "kf_load.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_state.h"
#include <eeregs.h>

extern u8 D_0034C4E0[50];
extern f32 D_00421090[4];
extern LinkNode D_00426470;
extern f32 D_0042B7D0[16];
extern u32 D_00446A54;              /* how many parts the screen has */
extern void *D_00446B20;
extern MenuColor D_00446C78;
extern u16 D_00446DE8;
extern s32 D_00446F38;
extern SpritePacket *D_00447250;
extern s8 D_004472DA;
extern ChunkUpload D_005CEEE0;

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
s32 func_001CFC58(s32);
void func_001DCA80(HandlerContext *);
int func_001E5210(u8);
int func_001FE350(int, int);
void func_001FF428(LevelDef *, s32);
void func_00200068(s32, s32);
void func_00201E10(LinkNode *, f32 *, MenuColor); // match: this file only matches with this type; func_00201E10.c defines it differently
void func_00203F28(int, int, int);

/* The offset of text `number` in the text table, 0 when it has none. */
static inline s32 func_001DD738_TextOffset(s32 table, s32 number) {
    return ((s32 *)table)[number];
}

/* The page of one entry (context->unk04, 3000 and up, 50 to a page): its parts, its model once loaded, and three texts. */
void func_001DD738(HandlerContext *context) {
    f32 view[16];
    f32 local[16];
    ChunkPoolNode model_node;
    GsPackedWord word;
    PackageChunk *chunk;
    u32 *packet;
    u32 i;
    s32 page;
    s32 number;
    s32 table;
    s32 entry;
    u8 kind;

    page = (context->unk04 - 3000) / 50;
    for (i = 0; i < D_00446A54; i++) {
        kind = D_00447250[i].unk3E;
        func_001E5210(D_00447250[i].unk42);
        func_00203F28((int)&D_00447250[i], kind, 1);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    if (D_005CEEE0.done != 0) {
        memset(view, 0, 0x40);
        view[10] = view[5] = view[0] = 0.8f;
        view[15] = 1.0f;
        sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
        sceVu0TransMatrix(local, view, D_00421090);
        packet = func_001FBE70();
        packet[0] = 0x8002;
        packet[1] = 0x10000000;
        packet[2] = 0xE;
        packet[3] = 0;
        packet[4] = 0x7660;
        packet[5] = 0x7890;
        packet[6] = 0x19;
        packet[7] = 0;
        packet[8] = 0x44;
        packet[9] = 0;
        packet[10] = 0x43;
        packet[11] = 0;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        chunk = (PackageChunk *)((char *)D_005CEEE0.pool_node->buffer + 0x10);
        for (i = 0; (s32)i < page; i++) {
            chunk = (PackageChunk *)((char *)chunk + chunk->unk0C + chunk->unk08);
        }
        D_00426470.pool_node = &model_node; // match: a local of this function; the node keeps the address after it returns
        D_00426470.unk20 = 1;
        model_node.buffer = chunk;
        func_00201E10(&D_00426470, local, D_00446C78);
    }
    table = func_001CFC58(D_00446F38);
    word.raw = 0x000A875081106E80;
    number = page * 50 + 3001;
    entry = func_001DD738_TextOffset(table, number + 49);
    func_001FDB88(word);
    if (entry != 0) {
        func_001FF428((LevelDef *)(table + entry), 0x17); // the text; func_001E88A0 declares the callee with a LevelDef
    }
    word.raw = 0x000A875083106FE0;
    entry = func_001DD738_TextOffset(table, number + D_0034C4E0[page] - 1);
    func_001FDB88(word);
    if (entry != 0) {
        func_001FE350(table + entry, 0x33);
    }
    word.raw = 0x000AAE607C307F80;
    func_001FDB88(word);
    entry = ((s32 *)table)[context->unk04 + 1];
    if (entry != 0) {
        if (page == 0) {
            func_001FE350(table + entry, 0x1B);
        } else {
            func_00200068(table + entry, 0x1B);
        }
    }
    if (D_00446DE8 & 0x2000) {
        func_0019CDC0(2);
        context->unk04 = 0;
        D_00446B20 = (void *)func_001DCA80;
        D_004472DA = 1;
    }
}
