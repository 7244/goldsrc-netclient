/* ugly because of performance reasons */
const uint8_t str_getchallenge[4 + 3 + 9 + 1 + 5 + 1] = {
  0xff, 0xff, 0xff, 0xff,
  'g','e','t','c','h','a','l','l','e','n','g','e',' ','s','t','e','a','m','\n'
};

void s_Open(ssl_NodeReference_t ssnr){
  ssl_NodeData_t *ssd = &ssl_GetNodeByReference(&pile.sp.ssl, ssnr)->data;

  sil_NodeReference_t sinr = ssd->sinr;
  sil_NodeData_t *sid = &sil_GetNodeByReference(&pile.sp.sil, sinr)->data;

  s_t *s = (s_t *)A_resize(0, sizeof(s_t));
  {
    SocketMapInput_t i;
    i.SocketIndex = 0;
    i.addr = sid->addr;
    SocketMap_InNew(&pile.sp.SocketMap, &i, &s);
  }

  net_sendto(&pile.sock, str_getchallenge, sizeof(str_getchallenge), &sid->addr);

  s->SocketIndex = 0;
  s->sinr = sinr;
  s->ss = ss_GetChallenge;

  {
    s->timeoutnr = SessionTimeOutList_NewNodeLast(&pile.sp.SessionTimeOutList);
    SessionTimeOutList_GetNodeByReference(&pile.sp.SessionTimeOutList, s->timeoutnr)->data = s;
  }
  s->LastAnswerTime = EV_nowi(&pile.listener);

  sid->StartedSessionCount++;
  if(sid->StartedSessionCount >= 1){
    ssl_unlrec(&pile.sp.ssl, ssnr);
    sid->ssnr = ssl_gnric();
    EV_timer_stop(&pile.listener, &pile.Timer);
  }
}

void s_Close(s_t *s){
  sil_NodeReference_t sinr = s->sinr;
  sil_NodeData_t *sid = &sil_GetNodeByReference(&pile.sp.sil, sinr)->data;

  {
    SocketMapInput_t i;
    i.SocketIndex = s->SocketIndex;
    i.addr = sid->addr;
    SocketMap_Remove(&pile.sp.SocketMap, &i);
  }

  if(s->ss == ss_Connected){
    ssc_Close(&s->c);
  }

  SessionTimeOutList_unlrec(&pile.sp.SessionTimeOutList, s->timeoutnr);

  sid->StartedSessionCount--;

  A_resize(s, 0);

  _sp_AddToSessionStartIfNeeded(sinr);
}
