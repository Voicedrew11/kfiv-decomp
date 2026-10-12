#include "kf_map.h"
#include "kf_player.h"
#include "kf_screen.h"
#include "kf_sound.h"

extern SoundLoadQueue D_0038A130;
extern MapSlot D_0038CD20[];
extern ScreenFade D_0038D680;
extern PlayerState D_00413F80;

float cosf(float);
float sinf(float);
void func_001034F8(int);
void func_00106178(int, int, int, int);
void func_00114840(int);
void func_00114AA8(int);
void func_001155F8(int);
void func_00116F20(int);
void func_00119360(int);
void func_0011C708(int);
void func_0011CC50(int);
int func_0011E1F0(int, int);
void func_00123158(int);
void func_00124F00(int);
void func_00125188(int);
void func_0019EA98(int);
void func_001A15F0(void);
void func_001C82C0(int);
void func_001C87D8(int);
void func_001CA920(int);
void func_00210930(int);
int func_002109C8(int, int);
void func_00216778(void *, void *);

/* Map change step: sets the loaded map up in slot 0, puts the player in it and ends the change. */
void func_001190C8(void) {
    MapSlot *slot = &D_0038CD20[0];
    MapObject *object;
    MapSlotUnk08 *header;
    f32 sine;
    f32 cosine;

    slot->tag = D_0038D680.map;
    func_00114840(0);
    func_00114AA8(0);
    func_001155F8(0);
    func_00116F20(0);
    func_001A15F0();
    func_0011C708(0);
    func_001C82C0(0);
    func_001CA920(0);
    switch (D_0038D680.kind) {
    case 1:
        func_00216778(&D_00413F80, &D_0038D680);
        break;
    case 2:
        object = (MapObject *)func_0011E1F0(0, D_0038D680.unk14);
        if (object != NULL) {
            sine = sinf(-object->model->rotation.y);
            cosine = cosf(-object->model->rotation.y);
            func_00216778(&D_00413F80, &object->model->position);
            D_00413F80.position.x += cosine * 0.0f - sine * -250.0f;
            D_00413F80.position.z += sine * 0.0f + cosine * -250.0f;
            D_00413F80.rotation.y = object->model->rotation.y;
            break;
        }
    case 0:
        header = slot->header;
        D_00413F80.position.x = header->unk60;
        D_00413F80.position.y = header->unk64;
        D_00413F80.position.z = header->unk68;
        D_00413F80.rotation.y = header->unk6C;
        break;
    }
    D_00413F80.map_index = 0xFFFF;
    D_00413F80.unk250 = 0xFF;
    D_00413F80.unk208 = 0;
    D_00413F80.unk204 = 0;
    D_00413F80.unk20C = 0;
    D_0038CD20[0].unk00 = 1;
    func_0011CC50(0);
    func_00124F00(0);
    func_00123158(0);
    func_001C87D8(0);
    func_00125188(0);
    func_00119360(0);
    func_001034F8(D_0038CD20[0].tag);
    func_00210930(D_0038CD20[0].unk5C);
    func_002109C8(0x7FFF, 0);
    D_0038A130.unk00 = ((MapSlotUnk08 *)D_0038CD20[0].header)->unk70;
    D_0038A130.unk04 = 1;
    func_0019EA98(1);
    D_0038A130.unk14 = &D_0038CD20[0];
    func_00106178(0, 0, 0, 30);
    D_0038D680.step = 0;
}
