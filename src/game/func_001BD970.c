#include "kf_player.h"
#include "kf_entity.h"

extern PlayerState D_00413F80;
extern ObjectReference *D_0044712C;
extern u16 D_0044692C;
extern u16 D_0044692E;
extern u16 D_00446930;
extern u16 D_00446932;
extern u16 D_00446934;
extern u16 D_00446936;
extern int D_00447128;
extern int D_0044713C;
extern int D_00447140;
extern int D_00447144;
extern int D_00447148;
extern int D_0044714C;
extern Vector4 D_0057B380;
extern Vector4 D_0057B390;

float sinf(float);
void KF_Vector_Scale(void *, void *, float);
void func_00216C48(void *, void *, void *);
int func_001BDD38(void);

void func_001BD970(u16 buttons) {
    Vector4 offset;
    f32 angle;
    f32 fraction;

    if (!(D_0044712C->unk10 & 0x10000)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if (buttons & (D_00446934 | D_00446936)) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    if ((buttons & (D_00446930 | D_00446932)) && !(buttons & (D_0044692C | D_0044692E))) {
        D_00413F80.unk1C8 &= ~(1L << 36);
    }
    D_0044714C++;
    angle = 1.9198622f;
    fraction = 1.0f;
    fraction /= (f32)D_0044713C;
    fraction = fraction * sinf((f32)D_0044714C * angle / (f32)D_00447148) / sinf(angle);
    KF_Vector_Scale(&offset, &D_0057B390,
                    fraction + (f32)(D_00447140 * D_00447148) / (f32)(D_0044713C * D_00447148));
    func_00216C48(&D_00413F80.position, &D_0057B380, &offset);
    if (D_0044714C >= D_00447148) {
        D_00447140 = D_00447144;
        D_00447128--;
        func_001BDD38();
    }
}
