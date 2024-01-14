#define MaxFragmentStreams 2

#pragma pack(push, 1)
  typedef struct{
    uint16_t To;
    uint16_t At;
    uint16_t StartPosition;
    uint16_t Size;
  }frag_head_t;
#pragma pack(pop)
