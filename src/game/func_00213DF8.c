void func_00115550(void *, float *);
void *func_00116160(int, int);
extern int D_00446770;
typedef struct 
{
  int a00;
  int a04;
  int a08;
  int a0c;
  float f10;
  int a14;
  float f18;
  int a1c;
  float f20;
  int a24;
  int a28;
} Obj;
typedef struct 
{
  char pad[0xC0];
  float x;
  float y;
  float z;
} Act;
typedef struct 
{
  char pad[0x7C];
  Act *act;
} Actor;
void func_00213DF8(Obj *arg0)
{
  float vec[4];
  Actor *temp_v0;
  Act *temp_a2;
  int n;
  float d;
  temp_v0 = func_00116160(D_00446770, arg0->a08);
  if (temp_v0 == 0)
  {
    arg0->a00 = 0;
    return;
  }
  temp_a2 = temp_v0->act;
  if (((arg0->f10 == temp_v0->act->x) && (arg0->f18 == temp_v0->act->y)) && (arg0->f20 == temp_a2->z))
  {
    arg0->a00 = 0;
    return;
  }
  n = arg0->a28;
  if (n == 0)
  {
    arg0->a28 = 1;
    n = 1;
  }
  d = (float) n;
  vec[0] = (arg0->f10 - temp_v0->act->x) / d;
  vec[1] = (arg0->f18 - temp_v0->act->y) / d;
  vec[2] = (arg0->f20 - temp_v0->act->z) / d;
  vec[3] = 0.0f;
  func_00115550(temp_v0, vec);
  if (--arg0->a28 == 0)
  {
    arg0->a00 = 0;
  }
}
