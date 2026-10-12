#include "kf_model.h"

u16 func_001090E0(char *);
int func_0011E3A0(void *);
int func_0011E1F0(int, int);
void func_0011E0C8(MapObject *);
void func_0011E140(void *);

/* Update handler: runs the animation, fading the model out over it when the
   definition asks for that; once it has ended the object goes out of use and
   the object named by its source comes into use. */
void func_0011EFC8(MapObject *object) {
    int total;
    int alpha;
    MapObject *other;

    if (MODEL_CHANNEL(object->model) != NULL && MODEL_CHANNEL(object->model)->param != 0) {
        if (object->unk04->unk68 & 0x20000) {
            total = func_001090E0((char *)object->model);
            if (total != 0) {
                alpha = ((total - MODEL_CHANNEL(object->model)->param) << 7) / total;
            } else {
                alpha = total; // match: 0, through the variable
            }
            if (object->model->unk100 & 0x4000) {
                object->model->alpha = alpha;
            } else {
                object->model->unk145 = 1;
                object->model->unk146 = alpha;
            }
        }
        func_0011E3A0((char *)object);
    } else {
        if (!(object->unk04->unk68 & 0x20000)) {
            other = (MapObject *)func_0011E1F0(MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk38.bytes[3]);
            if (other != NULL) {
                func_0011E0C8(other);
            }
        }
        func_0011E140(object);
    }
}
