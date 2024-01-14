const unsigned char mungify_table[] =
{
  0x7A, 0x64, 0x05, 0xF1,
  0x1B, 0x9B, 0xA0, 0xB5,
  0xCA, 0xED, 0x61, 0x0D,
  0x4A, 0xDF, 0x8E, 0xC7
};
void COM_Munge(unsigned char *data, int len, int seq)
{
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++)
  {
    pc = (int *)&data[i * 4];
    c = *pc;
    c ^= ~seq;
    c = byteswap32(c);

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
    {
      *p++ ^= (0xa5 | (j << j) | j | mungify_table[(i + j) & 0x0f]);
    }

    c ^= seq;
    *pc = c;
  }
}
void COM_UnMunge(unsigned char *data, int len, int seq)
{
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++)
  {
    pc = (int *)&data[i * 4];
    c = *pc;
    c ^= seq;

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
    {
      *p++ ^= (0xa5 | (j << j) | j | mungify_table[(i + j) & 0x0f]);
    }

    c = byteswap32(c);
    c ^= ~seq;
    *pc = c;
  }
}

const unsigned char mungify_table2[] =
{
  0x05, 0x61, 0x7A, 0xED,
  0x1B, 0xCA, 0x0D, 0x9B,
  0x4A, 0xF1, 0x64, 0xC7,
  0xB5, 0x8E, 0xDF, 0xA0
};
void COM_UnMunge2(unsigned char *data, int len, int seq){
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++){
    pc = (int *)&data[i * 4];
    c = *pc;
    c ^= seq;

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
      *p++ ^= (0xa5 | (j << j) | j | mungify_table2[(i + j) & 0x0f]);

    c = byteswap32(c);
    c ^= ~seq;
    *pc = c;
  }
}
void COM_Munge2(void *data, int len, int seq){
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++){
    pc = (int *)&((uint8_t *)data)[i * 4];
    c = *pc;
    c ^= ~seq;
    c = byteswap32(c);

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
      *p++ ^= (0xa5 | (j << j) | j | mungify_table2[(i + j) & 0x0f]);

    c ^= seq;
    *pc = c;
  }
}

unsigned char mungify_table3[] ={
  0x20, 0x07, 0x13, 0x61,
  0x03, 0x45, 0x17, 0x72,
  0x0A, 0x2D, 0x48, 0x0C,
  0x4A, 0x12, 0xA9, 0xB5
};
void COM_UnMunge3(void *data, int len, int seq){
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++)
  {
    pc = (int *)&((uint8_t *)data)[i * 4];
    c = *pc;
    c ^= seq;

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
    {
      *p++ ^= (0xa5 | (j << j) | j | mungify_table3[(i + j) & 0x0f]);
    }

    c = byteswap32(c);
    c ^= ~seq;
    *pc = c;
  }
}
void COM_Munge3(void *data, int len, int seq){
  int i;
  int mungelen;
  int c;
  int *pc;
  unsigned char *p;
  int j;

  mungelen = len & ~3;
  mungelen /= 4;

  for (i = 0; i < mungelen; i++)
  {
    pc = (int *)&((uint8_t *)data)[i * 4];
    c = *pc;
    c ^= ~seq;
    c = byteswap32(c);

    p = (unsigned char *)&c;
    for (j = 0; j < 4; j++)
    {
      *p++ ^= (0xa5 | (j << j) | j | mungify_table3[(i + j) & 0x0f]);
    }

    c ^= seq;
    *pc = c;
  }
}
