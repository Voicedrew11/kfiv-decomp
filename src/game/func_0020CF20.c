float atan2f(float, float);
float sqrtf(float);
void KF_Vector_Subtract(void *, void *, void *);

float func_0020CF20(void *arg0, void *arg1) {
    float v[3];

    KF_Vector_Subtract(v, arg0, arg1);
    return atan2f(v[1], sqrtf(v[0] * v[0] + v[2] * v[2]));
}
