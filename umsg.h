typedef struct{
  uint8_t Size;
  uint8_t Name[16];
}umsg_t;

void umsg_Write(VEC_t *umsgs, svc_data_newusermsg *num){
  if(num->i < svc_id_UserMessageStart){
    PR_abort();
  }

  uint8_t Type = num->i - svc_id_UserMessageStart;

  if(Type >= umsgs->Current){
    uintptr_t oc = umsgs->Current;
    umsgs->Current = Type + 1;
    VEC_handle(umsgs);

    for(; oc < Type; oc++){
      umsg_t *umsg = &((umsg_t *)umsgs->ptr)[oc];
      umsg->Size = 0;
      MEM_set(0, umsg->Name, 16);
    }
  }

  umsg_t *umsg = &((umsg_t *)umsgs->ptr)[Type];
  umsg->Size = num->Size;
  MEM_copy(num->Data, umsg->Name, 16);
}

umsg_t *umsg_Read(VEC_t *umsgs, uint8_t Type){
  Type -= svc_id_UserMessageStart;

  if(Type >= umsgs->Current){
    return NULL;
  }

  return &((umsg_t *)umsgs->ptr)[Type];
}
