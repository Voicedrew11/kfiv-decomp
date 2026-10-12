#include "kf_game.h"
#include "kf_link.h"
#include "kf_load.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_screen.h"
#include "kf_texture.h"

extern u8 D_00369830[];
extern LevelDef D_004147F8[380];
extern s32 D_00421050[12];          /* the key items held, in the order they were got; -1: empty */
extern f32 D_00425510[0x100];       /* one alpha per menu part; [255] the text's */
extern PoolSlot D_0042A8F0[32];
extern f32 D_0042B7D0[16];
extern const KeyItemNumbers D_0043F7C8;
extern const KeyItemTexturePages D_0043FE48;
extern const KeyItemTexturePages D_0043FE60;
extern const KeyItemPositions D_0043FE80;
extern const KeyItemNameWords D_0043FEE0;
extern u32 D_00446A6C;              /* how many parts the screen has */
extern void *D_00446B20;
extern s32 D_00446BB8;              /* the row of three the list starts at, 0 to 2 */
extern s32 D_00446BBC;
extern MenuColor D_00446C78;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
extern SpritePacket *D_00447260;
extern s8 D_004472C5;
extern s8 D_004472C9;               /* 0: fading in, 1: open, 2/3: waiting for the loads, 4/5: fading out */
extern s8 D_004472DA;               /* nonzero while the screen takes input */
extern ChunkUpload D_005CEF30[];

void *memset(void *, int, u32);
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);
void sceVu0TransMatrix(float *, float *, float *);
s32 func_0019CDC0(s32);
void func_001D45D0(void);
void func_001D8398(void); // match: this file only matches with this type; func_001D8398.c defines it differently
void func_001E4568(void);
int func_001E5210(u8);
void func_001FDCD0(MenuColor);
void func_001FF428(LevelDef *, s32);
void func_002020A0(RenderSlot *, RenderSlotArg);
void func_00204180(TextureHeader *, LoadedBlockRef *);
void func_00207478(int, int, int, int);

/* The key item screen: six of the twelve slots at a time, each with its model and, once got, its name. */
void func_001DC2A8(void) {
    KeyItemNumbers numbers = D_0043F7C8;
    KeyItemTexturePages texture_pages = D_0043FE48;
    KeyItemTexturePages clut_pages = D_0043FE60;
    KeyItemPositions positions = D_0043FE80;
    f32 view[16];
    RenderSlotArg slot_arg;
    KeyItemModelFile *file;
    TextureHeader *texture;
    u32 i;
    u32 full_alpha;
    s32 slot;
    s32 *number;
    struct PoolPart *part;
    ChunkUpload *upload;

    memset(view, 0, 0x40);
    view[10] = view[5] = view[0] = 1.0f;
    view[15] = 1.0f;
    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        func_0019CDC0(1);
        D_00446BB8--;
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        func_0019CDC0(1);
        D_00446BB8++;
    }
    D_00446BB8 += 3;
    D_00446BB8 %= 3;
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 0.5f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
    for (i = 0; i < D_00446A6C; i++) {
        func_001E5210(D_00447260[i].unk42);
        func_00207478((int)&D_00447260[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }
    D_00446BBC++;
    for (i = 0; (s32)i < 6; i++) {
        slot = D_00421050[i + D_00446BB8 * 3];
        if (slot < 0) {
            continue;
        }
        upload = (ChunkUpload *)((s32)D_005CEF30 + slot * sizeof(ChunkUpload)); // match: &D_005CEF30[slot] as a sum of numbers gives the madd
        if (upload->done != 0) {
            file = upload->pool_node->buffer;
            texture = (TextureHeader *)((char *)file + file->texture_offset);
            part = (struct PoolPart *)((char *)file + file->part_offset + 0x10);
            TEXTURE_HEADER_BITS(texture)->tex0.f.tbp0 = texture_pages.base[i];
            TEXTURE_HEADER_BITS(texture)->tex0.f.cbp = clut_pages.base[i];
            D_0042A8F0[31].part = part;
            // match: both words are read through unions, which keeps the two NULL stores below them
            D_0042A8F0[31].unk58 = TEXTURE_HEADER_BITS(texture)->tex0.raw;
            D_0042A8F0[31].unk60 = TEXTURE_HEADER_BITS(texture)->unk18.raw;
            D_0042A8F0[31].unk48 = NULL;
            D_0042A8F0[31].unk40 = NULL;
            func_00204180(texture, NULL);
            sceVu0TransMatrix((f32 *)&D_0042A8F0[31], view, positions.position[i]);
            func_002020A0((RenderSlot *)&D_0042A8F0[31], slot_arg); // match: slot_arg is never set
            number = &numbers.number[D_00421050[i + D_00446BB8 * 3]];
            if (D_00369830[*number] != 0) {
                KeyItemNameWords words = D_0043FEE0;
                LevelDef *def = &D_004147F8[*number];

                ((s32 (*)(MenuColor))func_001FDCD0)(D_00446C78); // match: called as if it returned a value, which swaps $2 and $3 below
                func_001FDB88(words.word[i]);
                func_001FF428(def, 0x10);
            }
        }
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472C9 = 3;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    } else if ((D_00446DE8 & 0x8000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472C9 = 2;
        D_004472DA = 0;
        if (D_00446C90 != NULL) {
            D_00446C90->next = NULL;
        }
    }
    if (D_004472C9 == 2 && D_00446C90 == NULL) {
        D_004472C9 = 4;
    }
    if (D_004472C9 == 3 && D_00446C90 == NULL) {
        D_004472C9 = 5;
    }

    switch (D_004472C9) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A6C; i++) {
            D_00425510[i] += D_00447260[i].alpha / 10.0f;
            if (D_00425510[i] > D_00447260[i].alpha) {
                for (i = 0; i < D_00446A6C; i++) {
                    full_alpha = D_00447260[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C9 = 1;
                break;
            }
        }
        break;
    case 4:
    case 5:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A6C; i++) {
            D_00425510[i] -= D_00447260[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446BB8 = 0;
                D_00446B20 = (void *)func_001D8398;
                ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                ((void (*)(s32))func_001E4568)(0); // match: the callee takes nothing
                if (D_004472C9 == 5) {
                    D_004472C5 = 3;
                }
                D_004472C9 = 0;
                D_00446BBC = 0;
                break;
            }
        }
        break;
    }
}
