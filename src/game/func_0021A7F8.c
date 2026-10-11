int sceMcFormat(int, int);
int sceMcSync(int, int *, int *);
typedef struct 
{
  int pad0[32];
  int p80;
  int p84;
  int p88;
  int state;
  int pad1[2];
  int p98;
  int p9c;
} McState;
extern McState D_01E4F278;
int func_0021A7F8(void)
{
  McState *s = &D_01E4F278;
  int result;
  int st = s->state;
  int a = s->p80;
  int b = s->p84;
  switch (st)
  {
    case 0:
      s->state = 1;
      s->p9c = 0;
      break;

    case 1:
      if (sceMcFormat(a, b) != 0)
    {
      if (s->p9c == 2)
      {
        s->p98 = 0x32;
        return -10;
      }
      s->p9c++;
    }
    else
    {
      s->state++;
    }
      break;

    case 2:
      if (sceMcSync(1, 0, &result) != 0)
    {
      if (result == 0)
      {
        s->p88 = 0;
        s->p98 = 0x34;
        return 1;
      }
      if (s->p9c == st)
      {
        s->p88 = 0;
        s->p98 = 0x32;
        return result;
      }
      s->p9c++;
      s->state--;
    }
      break;

  }

  return 0;
}
