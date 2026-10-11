typedef struct 
{
  char pad[0x44];
  int f44;
  int f48;
  int f4C;
} Ctx;
void func_0021C9E0(int *a0, int *a1, int *a2, int *a3, Ctx *s, int x)
{
  int d = s->f48;
  int base = s->f4C;
  int t;
  x = (((x + d) - base) - 0x400) % d;
  t = x;
  t = (t / 1024) * 1024;
  if ((d - base) >= t)
  {
    *a0 = s->f44 + s->f4C;
    *a1 = t;
    *a2 = 0;
    *a3 = 0;
    return;
  }
  *a0 = s->f44 + s->f4C;
  *a1 = s->f48 - s->f4C;
  *a2 = s->f44;
  *a3 = t - (s->f48 - s->f4C);
}
