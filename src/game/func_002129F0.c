int func_0011E1F0(int, int);
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
void func_002129F0(Obj *arg0)
{
  int r = func_0011E1F0(D_00446770, arg0->a08);
  if (r == 0)
  {
    arg0->a00 = 0;
    return;
  }
  ;
  if (((arg0->f10 == (*((Act **) r))->x) && (arg0->f18 == (*((Act **) r))->y)) && (arg0->f20 == (*((Act **) r))->z))
  {
    arg0->a00 = 0;
  }
  else
  {
    int n = arg0->a28;
    float d;
    int m;
    if (n == 0)
    {
      arg0->a28 = 1;
      n = 1;
    }
    d = (float) n;
    m = n - 1;
    arg0->a28 = m;
    (*((Act **) r))->x = (*((Act **) r))->x + ((arg0->f10 - (*((Act **) r))->x) / d);
    (*((Act **) r))->y = (*((Act **) r))->y + ((arg0->f18 - (*((Act **) r))->y) / d);
    (*((Act **) r))->z = (*((Act **) r))->z + ((arg0->f20 - (*((Act **) r))->z) / d);
    if (m <= 0)
    {
      arg0->a00 = 0;
    }
  }
}
