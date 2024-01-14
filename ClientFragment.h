/* client fragment callback */
typedef void(*cfcb_t)(s_t *);

typedef struct{
  bool Exist;
  uint32_t LastSequence;
  VEC_t Data;
  uintptr_t DataOffset;
  cfcb_t cb;
}ClientFragment_t;

void ClientFragment_Init(ClientFragment_t *cf){
  cf->Exist = false;
  VEC_init(&cf->Data, 1, A_resize);
}
void ClientFragment_Reinit(ClientFragment_t *cf, cfcb_t cb){
  if(cf->Exist == true){
    PR_abort();
  }

  cf->Exist = true;
  cf->LastSequence = (uint32_t)-1;
  cf->Data.Current = 0;
  cf->DataOffset = 0;
  cf->cb = cb;
}
void ClientFragment_Free(ClientFragment_t *cf){
  VEC_free(&cf->Data);
}
