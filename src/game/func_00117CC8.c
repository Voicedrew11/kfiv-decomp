#include "kf_map.h"
#include "kf_file.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern void (*D_0038D5E0[])(void);
extern MapLoadState D_0038D610;
extern PlayerState D_00413F80;
extern int D_00446770;
extern int D_00446794;

void func_00101618(void);
int func_001005E8(int);
int func_00102230(void);
void func_00102228(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010B000(void);
void func_00117F98(void);
void func_00123288(void);
void Sound_Update(void);
void func_001B5778(void);
void Display_SwapBuffers(int);

/* Starts, queues and steps the background load of the map next to where the player stands. */
void func_00117CC8(void) {
    MapSlot *current = &D_0038CD20[D_00446770];
    MapSlot *other = (D_00446770 == 0) ? &D_0038CD20[1] : &D_0038CD20[0];
    MapRecordInfo *record;
    MapLoadRequest *request;

    if (D_00446794 != 0) {
        func_00117F98();
        return;
    }
    if (D_00413F80.map_index != 0xFFFF) {
        record = (MapRecordInfo *)current->records + D_00413F80.map_index;
    } else {
        record = NULL;
    }
    if (record != NULL && record->unk3A != 0xFF) {
        request = (MapLoadRequest *)&current->origins[record->unk3A];
        if (current->tag != request->map) {
            if (other->unk00 == 0 || other->tag != request->map) {
                if (D_0038D610.request == NULL || D_0038D610.request->map != request->map) {
                    if (D_0038D610.step == 0) {
                        D_0038D610.request = request;
                    } else {
                        D_0038D610.next_request = request;
                    }
                }
            }
        }
    }
    if (D_0038D610.next_request != NULL && D_0038D610.step != 4) {
        D_0038D610.step = 2;
        D_0038D610.request = D_0038D610.next_request;
        D_0038D610.next_request = NULL;
        if (D_0038D610.file_handle != -1) {
            func_001024B0(D_0038D610.file_handle);
            if (func_00102230() != 0) {
                func_00102228();
            }
            D_0038D610.file_handle = -1;
        }
    }
    if ((D_0038D610.flags.raw & 6) == 4) {
        if (D_0038D610.step == 0 || D_0038D610.step == 3 || D_0038D610.step == 5) {
            D_0038D610.flags.f.bit1 = TRUE;
            D_0038D610.flags.f.bit2 = FALSE;
        }
    }
    if (!((s32)(D_0038D610.flags.raw >> 1) & 1)) {
        if (record != NULL && record->unk39 != 0) {
            if ((u32)(D_0038D610.step - 1) < 5) {
                func_001B5778();
                while (D_0038D610.step != 5) {
                    func_001005E8(0);
                    func_00109800();
                    Display_SwapBuffers(0);
                    func_001096E8();
                    func_001095E0();
                    func_0010B000();
                    D_0038D5E0[D_0038D610.step]();
                    func_00123288();
                    func_00101618();
                    Sound_Update();
                }
                D_0038D610.step = 6;
            }
        }
        D_0038D5E0[D_0038D610.step]();
    }
}
