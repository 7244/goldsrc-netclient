void ssc_Open(ssc_t *ssc){
  ssc->srcSequence = 1;
  ssc->dstSequence = 0;
  ssc->ConnectionState = 0;

  ssc->lr.Size = (uintptr_t)-1;

  VEC_init(&ssc->ServerFragment, 1, A_resize);
  ClientFragment_Init(&ssc->ClientFragment);
  VEC_init(&ssc->Resources, sizeof(Resource_t), A_resize);
  VEC_init(&ssc->UserMessages, sizeof(umsg_t), A_resize);
}
void ssc_Close(ssc_t *ssc){
  if(ssc->lr.Size != (uintptr_t)-1){
    EV_timer_stop(&pile.listener, &ssc->lr.NoACKTimer);
  }

  VEC_free(&ssc->UserMessages);
  VEC_free(&ssc->Resources);
  ClientFragment_Free(&ssc->ClientFragment);
  VEC_free(&ssc->ServerFragment);
}
