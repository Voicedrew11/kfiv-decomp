#include "kf_entity.h"

void func_0018B778(float, void *, void *, void *);
int rand(void);

/* picks a point like func_0018B778, then moves its height by up to +-height_range */
void func_0018B6F8(f32 *point, f32 range, f32 height_range, void *centre, void *position) {
    s32 offset;

    func_0018B778(range, point, centre, position);
    offset = (s32)(height_range + height_range);
    offset = rand() % offset - offset / 2;
    point[1] += offset;
}
