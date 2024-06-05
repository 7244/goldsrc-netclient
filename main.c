#include "common.h"

#include "munge.h"
#include "svc.h"
#include "clc.h"
#include "msg.h"
#include "delta.h"
#include "res.h"
#include "frag.h"
#include "ent.h"
#include "snd.h"

#include "umsg.h"

/* session state */
typedef enum{
  ss_Begin,
  ss_GetChallenge,
  ss_Connect_0,
  ss_Connected
}ss_t;

typedef struct s_t s_t;
#include "ClientFragment.h"
#include "ssc.h"
#include "s.h"

/* session pile */
typedef struct{
  sil_t sil;
  ssl_t ssl;

  SessionTimeOutList_t SessionTimeOutList;
  EV_timer_t Timer_SessionTimeOut;

  SocketMap_t SocketMap;
}sp_t;

typedef struct{
  EV_t listener;

  EV_event_t InputEvent;

  EV_timer_t Timer;

  EV_event_t Read;

  NET_socket_t sock;

  sp_t sp;
}pile_t;
pile_t pile;

void s_Open(ssl_NodeReference_t);

void ssl_Trigger(ssl_NodeReference_t ssnr){
  s_Open(ssnr);
}

bool CanWeStartSession(){
  return true;
}

void cb_Timer(EV_t *listener, EV_timer_t *evt){
  ssl_NodeReference_t ssnr = ssl_GetNodeFirst(&pile.sp.ssl);

  while(1){
    if(ssl_inre(ssnr, pile.sp.ssl.dst)){
      break;
    }
    if(!CanWeStartSession()){
      break;
    }

    ssl_Node_t *ssn = ssl_GetNodeByReference(&pile.sp.ssl, ssnr);
    ssl_NodeReference_t next = ssn->NextNodeReference;
    ssl_Trigger(ssnr);
    ssnr = next;
  }
}

void _sp_AddToSessionStartIfNeeded(sil_NodeReference_t sinr){
  sil_NodeData_t *d = &sil_GetNodeByReference(&pile.sp.sil, sinr)->data;

  if(d->StartedSessionCount >= 1 || !ssl_inric(d->ssnr)){
    return;
  }

  if(ssl_Usage(&pile.sp.ssl) == 0){
    EV_timer_init(&pile.Timer, 0.05, cb_Timer);
    EV_timer_start(&pile.listener, &pile.Timer);
  }

  ssl_NodeReference_t ssnr = ssl_NewNodeLast(&pile.sp.ssl);
  ssl_NodeData_t *ssd = &ssl_GetNodeByReference(&pile.sp.ssl, ssnr)->data;

  ssd->sinr = sinr;
}

void sp_AddServer(NET_addr_t *addr){
  sil_NodeReference_t nr = sil_NewNodeLast(&pile.sp.sil);
  sil_NodeData_t *d = &sil_GetNodeByReference(&pile.sp.sil, nr)->data;

  d->addr = *addr;
  d->StartedSessionCount = 0;
  d->ssnr = ssl_gnric();

  _sp_AddToSessionStartIfNeeded(nr);
}

#include "ssc_func.h"
#include "s_func.h"

void cb_SessionTimeOut(EV_t *listener, EV_timer_t *evt){
  uint64_t ct = EV_nowi(listener);

  SessionTimeOutList_NodeReference_t nr = SessionTimeOutList_GetNodeFirst(&pile.sp.SessionTimeOutList);
  while(!SessionTimeOutList_inre(nr, pile.sp.SessionTimeOutList.dst)){
    SessionTimeOutList_Node_t *n = SessionTimeOutList_GetNodeByReference(&pile.sp.SessionTimeOutList, nr);
    SessionTimeOutList_NodeReference_t next = n->NextNodeReference;
    s_t *s = n->data;
    print("%llu\n", ct - s->LastAnswerTime);
    if(ct - s->LastAnswerTime >= (uint64_t)20000000000){
      print("timeout came\n");
      s_Close(s);
    }
    nr = next;
  }
}

void sp_Open(){
  sil_Open(&pile.sp.sil);

  ssl_Open(&pile.sp.ssl);

  SessionTimeOutList_Open(&pile.sp.SessionTimeOutList);

  EV_timer_init(&pile.sp.Timer_SessionTimeOut, 10, cb_SessionTimeOut);
  EV_timer_start(&pile.listener, &pile.sp.Timer_SessionTimeOut);

  SocketMap_Open(&pile.sp.SocketMap);
}
void sp_Close(){
  SocketMap_Close(&pile.sp.SocketMap);

  EV_timer_stop(&pile.listener, &pile.sp.Timer_SessionTimeOut);

  SessionTimeOutList_Close(&pile.sp.SessionTimeOutList);

  ssl_Close(&pile.sp.ssl);

  sil_Close(&pile.sp.sil);
}

typedef enum{
  PacketType_Connectionless,
  PacketType_Split,
  PacketType_Connected
}PacketType_t;

void SendConnect(s_t *s){
  VEC_t Data;
  VEC_init(&Data, 1, A_resize);

  VEC_print(&Data, "%c%c%c%cconnect 48 %.*s ", 0xff, 0xff, 0xff, 0xff, (uintptr_t)s->ChallengeSize, s->Challenge);

  ReadFromFile("cinfo", &Data);

  sil_NodeData_t *sind = &sil_GetNodeByReference(&pile.sp.sil, s->sinr)->data;

  net_sendto(&pile.sock, Data.ptr, Data.Current, &sind->addr);

  VEC_free(&Data);
}

sint32_t ProcessGetChallenge(s_t *s, uint32_t Section, const uint8_t *Data, uintptr_t Size){
  if(Section == 1){
    if(Size > sizeof(s->Challenge)){
      return -1;
    }
    s->ChallengeSize = Size;
    MEM_copy(Data, s->Challenge, s->ChallengeSize);

    SendConnect(s);

    s->ss = ss_Connect_0;
  }

  return 0;
}

#define ConnectedHeadSize 8

void SetSequences(ssc_t *ssc, void *Data, uint8_t Mode){
  ((uint32_t *)Data)[0] = ssc->srcSequence | (uint32_t)Mode << 30;
  ((uint32_t *)Data)[1] = ssc->dstSequence;

  ssc->srcSequence++;
}
void SetSequencesVEC(ssc_t *ssc, VEC_t *Data, uint8_t Mode){
  VEC_handle0(Data, ConnectedHeadSize);
  SetSequences(ssc, Data->ptr, Mode);
}

void WriteToConnection(s_t *, void *, uintptr_t);

void cb_ACKNotSent(EV_t *listener, EV_timer_t *evt){
  ssc_t *c = OFFSETLESS(evt, ssc_t, lr.NoACKTimer);
  s_t *s = OFFSETLESS(c, s_t, c);

  uintptr_t Size = c->lr.Size;
  c->lr.Size = (uintptr_t)-1;
  uint8_t Data[0x500];
  MEM_copy(c->lr.Data, Data, Size);
  
  EV_timer_stop(&pile.listener, &c->lr.NoACKTimer);

  SetSequences(c, Data, *(uint32_t *)Data >> 30);

  WriteToConnection(s, Data, Size);

  sil_NodeData_t *sind = &sil_GetNodeByReference(&pile.sp.sil, s->sinr)->data;
  print("packet loss at %08lx%04lx\n", sind->addr.ip, sind->addr.port);
}

void WriteToConnection(s_t *s, void *Data, uintptr_t Size){
  ssc_t *c = &s->c;

  if(c->lr.Size != (uintptr_t)-1){
    /* internal error, we should not send two packet */
    print("err %x\n", c->lr.Size);
    PR_abort();
  }

  uint32_t Sequence = *(uint32_t *)Data & 0x3fffffff;
  uint8_t ms = (uint8_t)Sequence; /* munge sequence */

  c->lr.Size = Size;
  MEM_copy(Data, c->lr.Data, c->lr.Size);

  COM_Munge2(&((uint8_t *)Data)[ConnectedHeadSize], Size - ConnectedHeadSize, ms);

  sil_NodeData_t *sind = &sil_GetNodeByReference(&pile.sp.sil, s->sinr)->data;

  net_sendto(&pile.sock, Data, Size, &sind->addr);

  EV_timer_init(&c->lr.NoACKTimer, 0.2, cb_ACKNotSent);
  EV_timer_start(&pile.listener, &c->lr.NoACKTimer);
}

void SendACK(s_t *s){
  ssc_t *ssc = &s->c;

  gt_begin:;

  if(ssc->lr.Size != (uintptr_t)-1){
    return;
  }

  if(ssc->ClientFragment.Exist == true){
    if(ssc->ClientFragment.DataOffset == ssc->ClientFragment.Data.Current){
      ssc->ClientFragment.Exist = false;
      if(ssc->ClientFragment.cb != NULL){
        ssc->ClientFragment.cb(s);
      }
      goto gt_begin;
    }

    uintptr_t DataIndex = 0;
    uint8_t Data[ConnectedHeadSize + 1 + sizeof(frag_head_t) + 1 + 1024];

    SetSequences(ssc, &Data[DataIndex], 3);
    DataIndex += ConnectedHeadSize;

    *(uint8_t *)&Data[DataIndex] = 1;
    DataIndex += sizeof(uint8_t);

    uintptr_t fds = ssc->ClientFragment.Data.Current - ssc->ClientFragment.DataOffset; /* fragment data size */
    if(fds > 1024){
      fds = 1024;
    }

    frag_head_t *fh = (frag_head_t *)&Data[DataIndex];
    fh->To = ssc->ClientFragment.Data.Current / 1024 + !!(ssc->ClientFragment.Data.Current % 1024);
    fh->At = ssc->ClientFragment.DataOffset / 1024 + 1;
    fh->StartPosition = 0;
    fh->Size = fds;
    DataIndex += sizeof(frag_head_t);

    *(uint8_t *)&Data[DataIndex] = 0;
    DataIndex += sizeof(uint8_t);

    MEM_copy(&ssc->ClientFragment.Data.ptr[ssc->ClientFragment.DataOffset], &Data[DataIndex], fds);
    DataIndex += fds;

    ssc->ClientFragment.DataOffset += fds;

    WriteToConnection(s, Data, DataIndex);
  }
  else{
    uint8_t Data[ConnectedHeadSize + 8];
    SetSequences(ssc, Data, 0);
    ((uint64_t *)Data)[1] = 0x0101010101010101;

    WriteToConnection(s, Data, sizeof(Data));
  }
}

void SendConnection(s_t *s){
  ssc_t *ssc = &s->c;

  uint8_t arr0[] = {clc_id_stringcmd, 'n', 'e', 'w', 0x00, 0x01, 0x01, 0x01};
  uint8_t arr1[] = {clc_id_stringcmd, 's', 'e', 'n', 'd', 'r', 'e', 's', 0x00};

  VEC_t Data;
  VEC_init(&Data, 1, A_resize);

  SetSequencesVEC(ssc, &Data, 2);
  if(ssc->ConnectionState == 0){
    VEC_print(&Data, "%.*s", sizeof(arr0), arr0);
  }
  else if(ssc->ConnectionState == 1){
    VEC_print(&Data, "%.*s", sizeof(arr1), arr1);
  }
  else if(ssc->ConnectionState == 2){
    uint32_t worldmapCRC = ssc->worldmapCRC;
    COM_Munge2(&worldmapCRC, sizeof(uint32_t), (-1 - ssc->SpawnCount) & 0xFF);

    VEC_print(&Data, "%cspawn %lu %ld%c", clc_id_stringcmd, ssc->SpawnCount, worldmapCRC, 0);
  }
  ssc->ConnectionState++;

  WriteToConnection(s, Data.ptr, Data.Current);

  VEC_free(&Data);

  print("connect sequence: %lx %lx\n", ssc->ConnectionState, ssc->dstSequence);
}

void cb_ConsistencySent(s_t *s){
  SendConnection(s);
}

sint32_t ProcessServerData(s_t *s, uint8_t *Data, uintptr_t Size, bool Recursived){
  ssc_t *ssc = &s->c;

  while(Size){
    uint8_t DataType = Data[0];
    Data = &Data[1];
    Size -= 1;

    if(DataType == svc_id_nop){
      /* time after time */
    }
    else if(DataType == svc_id_print){
      uintptr_t PrintSize = 0;
      for(; PrintSize < Size; PrintSize++){
        if(Data[PrintSize] == 0){
          break;
        }
      }
      if(Size == 0 || Data[PrintSize] != 0){
        return -1;
      }

      Data += PrintSize + 1;
      Size -= PrintSize + 1;
    }
    else if(DataType == svc_id_serverinfo){
      svc_data_serverinfo si;
      if(MSG_ReadBytes(&Data, &Size, &si, sizeof(si))){return -1;}

      ssc->MaxClients = si.MaxClients;
      ssc->PlayerNumber = si.PlayerNumber;

      ssc->worldmapCRC = si.Munge3_worldmapCRC;
      COM_UnMunge3(&ssc->worldmapCRC, sizeof(uint32_t), (-1 - ssc->PlayerNumber) & 0xff);

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}

      uint8_t isVAC2Secure;
      if(MSG_ReadBytes(&Data, &Size, &isVAC2Secure, sizeof(isVAC2Secure))){return -1;}
    }
    else if(DataType == svc_id_sendextrainfo){
      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0) {return -1;}

      uint8_t AllowCheats;
      if(MSG_ReadBytes(&Data, &Size, &AllowCheats, sizeof(AllowCheats))){return -1;}
    }
    else if(DataType == svc_id_deltadescription){
      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0){return -1;}

      DeltaType_t *DeltaType = FindDeltaType(str.Data, str.Size);
      if(DeltaType == NULL){
        print("invalid delta name %.*s\n", str.Size, str.Data);
        return -1;
      }

      uintptr_t BitIndex = 0;

      uint16_t FieldCount = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &FieldCount, 16) != 0){return -1;}

      if(FieldCount != DeltaType->FieldAmount){
        return -1;
      }

      for(uint32_t ifield = 0; ifield < FieldCount; ifield++){
        if(ParseDelta(&DeltaType_MetaDescription, Data, Size, &BitIndex)){return -1;}
      }

      Data += BitIndex / 8 + !!(BitIndex % 8);
      Size -= BitIndex / 8 + !!(BitIndex % 8);
    }
    else if(DataType == svc_id_newmovevars){
      svc_data_newmovevars mv;
      if(MSG_ReadBytes(&Data, &Size, &mv, sizeof(mv))){return -1;}

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0) {return -1;}

      if(s->c.ConnectionState == 1){
        SendConnection(s);
      }
    }
    else if(DataType == svc_id_cdtrack){
      uint8_t CDTrack[2];
      if(MSG_ReadBytes(&Data, &Size, CDTrack, sizeof(CDTrack))){return -1;}
    }
    else if(DataType == svc_id_setview){
      uint16_t PlayerNumber;
      if(MSG_ReadBytes(&Data, &Size, &PlayerNumber, sizeof(PlayerNumber))){return -1;}
    }
    else if(DataType == svc_id_newusermsg){
      svc_data_newusermsg msg;
      if(MSG_ReadBytes(&Data, &Size, &msg, sizeof(msg))){return -1;}

      umsg_Write(&ssc->UserMessages, &msg);
    }
    else if(DataType == svc_id_stufftext){
      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0) {return -1;}
    }
    else if(DataType == svc_id_updateuserinfo){
      uint8_t ClientID;
      if(MSG_ReadBytes(&Data, &Size, &ClientID, sizeof(ClientID))){return -1;}

      uint32_t UserID;
      if(MSG_ReadBytes(&Data, &Size, &UserID, sizeof(UserID))){return -1;}

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0) {return -1;}

      uint8_t md5[16];
      if(MSG_ReadBytes(&Data, &Size, md5, sizeof(md5))){return -1;}
    }
    else if(DataType == svc_id_resourcerequest){
      if(MSG_ReadBytes(&Data, &Size, &ssc->SpawnCount, sizeof(ssc->SpawnCount))){return -1;}

      uint32_t filler;
      if(MSG_ReadBytes(&Data, &Size, &filler, sizeof(filler))){return -1;}
    }
    else if(DataType == svc_id_resourcelocation){
      String_t str;
      if(MSG_ReadString(&Data, &Size, &str) != 0) {return -1;}
    }
    else if(DataType == svc_id_resourcelist){
      uint8_t DoWeGetConsistencyList = 0;

      {
        uintptr_t BitIndex = 0;

        if(MSG_ReadBits(Data, &BitIndex, Size, &ssc->Resources.Current, 12) != 0){return -1;}

        VEC_handle(&ssc->Resources);

        for(uint32_t inr = 0; inr < ssc->Resources.Current; inr++){
          Resource_t *r = &((Resource_t *)ssc->Resources.ptr)[inr];

          uint8_t Type = 0;
          if(MSG_ReadBits(Data, &BitIndex, Size, &Type, 4) != 0){return -1;}

          if(MSG_ReadBitString(Data, &BitIndex, Size, &r->Name) != 0){return -1;}

          uint32_t ResourceIndex = 0;
          if(MSG_ReadBits(Data, &BitIndex, Size, &ResourceIndex, 12) != 0){return -1;}

          uint32_t DownloadSize = 0;
          if(MSG_ReadBits(Data, &BitIndex, Size, &DownloadSize, 24) != 0){return -1;}

          r->Flag = 0;
          if(MSG_ReadBits(Data, &BitIndex, Size, &r->Flag, 3) != 0){return -1;}

          MEM_set(0, r->md5, 16);
          if(r->Flag & RES_CUSTOM){
            if(MSG_ReadBits(Data, &BitIndex, Size, r->md5, 16 * 8) != 0){return -1;}
          }

          uint8_t Is_rguc_reserved = 0;
          if(MSG_ReadBits(Data, &BitIndex, Size, &Is_rguc_reserved, 1) != 0){return -1;}

          MEM_set(0, &r->reserved, 32);
          if(Is_rguc_reserved == 1){
            if(MSG_ReadBits(Data, &BitIndex, Size, &r->reserved, 32 * 8) != 0){return -1;}
          }

          r->NeedConsistency = false;
        }

        if(MSG_ReadBits(Data, &BitIndex, Size, &DoWeGetConsistencyList, 1) != 0){return -1;}

        if(DoWeGetConsistencyList == 1){
          uint32_t LastIndex = 0;

          while(1){
            uint8_t DoWeHaveFile = 0;
            if(MSG_ReadBits(Data, &BitIndex, Size, &DoWeHaveFile, 1) != 0){return -1;}

            if(DoWeHaveFile == 0){
              break;
            }

            uint8_t IndexOrDiff = 0;
            if(MSG_ReadBits(Data, &BitIndex, Size, &IndexOrDiff, 1) != 0){return -1;}

            if(IndexOrDiff == 0){
              LastIndex = 0;
              if(MSG_ReadBits(Data, &BitIndex, Size, &LastIndex, 10) != 0){return -1;}
            }
            else{
              uint32_t diff = 0;
              if(MSG_ReadBits(Data, &BitIndex, Size, &diff, 5) != 0){return -1;}
              LastIndex += diff;
            }

            if(LastIndex >= ssc->Resources.Current){
              return -1;
            }

            Resource_t *r = &((Resource_t *)ssc->Resources.ptr)[LastIndex];

            r->NeedConsistency = true;

            if(!(r->Flag & RES_CUSTOM)) do{
              {
                bool BadResourceName = false;
                uint8_t DotStreak = 0;
                for(uint32_t i = 0; i < r->Name.Size; i++){
                  if(r->Name.Data[i] == '.'){
                    DotStreak++;
                    if(DotStreak == 2){
                      print("dead0 %.*s\n", r->Name.Size, r->Name.Data);
                      BadResourceName = true;
                      break;
                    }
                  }
                  else if(!(
                    r->Name.Data[i] == '/' ||
                    r->Name.Data[i] == '_' ||
                    (r->Name.Data[i] >= '0' & r->Name.Data[i] <= '9') ||
                    (r->Name.Data[i] >= 'A' & r->Name.Data[i] <= 'Z') ||
                    (r->Name.Data[i] >= 'a' & r->Name.Data[i] <= 'z')
                  )){
                    print("dead1 %.*s\n", r->Name.Size, r->Name.Data);
                    BadResourceName = true;
                    break;
                  }
                  else{
                    DotStreak = 0;
                  }
                }
                if(BadResourceName){
                  break;
                }
              }

              const char *base_dir = "/home/arch/.steam/steam/steamapps/common/Half-Life/";
              const char *sub_dir[2] = {
                "ag/",
                "valve/"
              };

              VEC_t Path;
              VEC_init(&Path, 1, A_resize);

              bool ItsSound = false;
              for(uint32_t i = r->Name.Size; i--;){
                if(r->Name.Data[i] != '.'){
                  continue;
                }
                i++;
                if(
                  r->Name.Size - i == 3 &&
                  (
                    r->Name.Data[i + 0] == 'w' &&
                    r->Name.Data[i + 1] == 'a' &&
                    r->Name.Data[i + 2] == 'v'
                  )
                ){
                  ItsSound = true;
                }
                break;
              }

              uint32_t isub = 0;
              for(; isub < sizeof(sub_dir) / sizeof(sub_dir[0]); isub++){
                Path.Current = 0;

                VEC_print(&Path, "%s", base_dir);
                VEC_print(&Path, "%s", sub_dir[isub]);

                if(ItsSound){
                  VEC_print(&Path, "sound/");
                }

                VEC_print(&Path, "%.*s", r->Name.Size, r->Name.Data);

                VEC_print(&Path, "%c", 0);

                if(FileToMD5(Path.ptr, r->md5) == 0){
                  break;
                }
              }
              if(isub == sizeof(sub_dir) / sizeof(sub_dir[0])){
                /* print("file consistency failed with \"%.*s\". ignoring.\n", r->Name.Size, r->Name.Data); */
              }

              VEC_free(&Path);
            }while(0);
          }
        }

        Data += BitIndex / 8 + !!(BitIndex % 8);
        Size -= BitIndex / 8 + !!(BitIndex % 8);
      }

      if(ssc->ConnectionState != 2){
        return -1;
      }

      ClientFragment_Reinit(&ssc->ClientFragment, cb_ConsistencySent);

      uintptr_t BitIndex = 0;

      VEC_handle0(&ssc->ClientFragment.Data, 3);
      ssc->ClientFragment.Data.ptr[0] = clc_id_fileconsistency;
      uintptr_t bwi = 3; /* bit write index */

      uint8_t one = 1;
      uint8_t zero = 0;

      for(uint32_t ir = 0; ir < ssc->Resources.Current; ir++){
        Resource_t *r = &((Resource_t *)ssc->Resources.ptr)[ir];

        if(!r->NeedConsistency){
          continue;
        }

        {
          uintptr_t ps = BitIndex + 1 + 12 + 192; /* possible size */
          ssc->ClientFragment.Data.Current = bwi + ps / 8 + !!(ps % 8);
          VEC_handle(&ssc->ClientFragment.Data);
        }

        MSG_WriteBits(&one, 1, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);
        MSG_WriteBits(&ir, 12, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);

        uint32_t i = 0;
        for(; i < 32; i++){
          if(r->reserved[i] != 0){
            break;
          }
        }

        if(i == 32){
          MSG_WriteBits(r->md5, 32, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);
        }
        else{
          MSG_WriteBits(&r->reserved[1], 12 * 8, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);
          MSG_WriteBits(&r->reserved[13], 12 * 8, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);
        }
      }

      MSG_WriteBits(&zero, 1, &ssc->ClientFragment.Data.ptr[bwi], &BitIndex, (uintptr_t)-1);

      uintptr_t s = BitIndex / 8 + !!(BitIndex % 8);
      ssc->ClientFragment.Data.Current = 3 + s;
      COM_Munge(&ssc->ClientFragment.Data.ptr[bwi], s, ssc->SpawnCount);

      *(uint16_t *)&ssc->ClientFragment.Data.ptr[1] = s;
    }
    else if(DataType == svc_id_temp_entity){
      uint8_t filler;
      if(MSG_ReadBytes(&Data, &Size, &filler, sizeof(filler))){return -1;}

      uint16_t origin[3];
      if(MSG_ReadBytes(&Data, &Size, &origin[0], sizeof(origin[0]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &origin[1], sizeof(origin[1]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &origin[2], sizeof(origin[2]))){return -1;}

      uint16_t DecalIndex;
      if(MSG_ReadBytes(&Data, &Size, &DecalIndex, sizeof(DecalIndex))){return -1;}

      uint16_t EntityIndex;
      if(MSG_ReadBytes(&Data, &Size, &EntityIndex, sizeof(EntityIndex))){return -1;}

      if(EntityIndex){
        uint16_t ModelIndex;
        if(MSG_ReadBytes(&Data, &Size, &ModelIndex, sizeof(ModelIndex))){return -1;}
      }
    }
    else if(DataType == svc_id_spawnstaticsound){
      uint16_t pos[3];
      if(MSG_ReadBytes(&Data, &Size, &pos[0], sizeof(pos[0]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &pos[1], sizeof(pos[1]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &pos[2], sizeof(pos[2]))){return -1;}

      uint16_t SoundNumber;
      if(MSG_ReadBytes(&Data, &Size, &SoundNumber, sizeof(SoundNumber))){return -1;}

      uint8_t Volume;
      if(MSG_ReadBytes(&Data, &Size, &Volume, sizeof(Volume))){return -1;}

      uint8_t Attenuation;
      if(MSG_ReadBytes(&Data, &Size, &Attenuation, sizeof(Attenuation))){return -1;}

      uint16_t Entity;
      if(MSG_ReadBytes(&Data, &Size, &Entity, sizeof(Entity))){return -1;}

      uint8_t Pitch;
      if(MSG_ReadBytes(&Data, &Size, &Pitch, sizeof(Pitch))){return -1;}

      uint8_t Flag;
      if(MSG_ReadBytes(&Data, &Size, &Flag, sizeof(Flag))){return -1;}
    }
    else if(DataType == svc_id_sendcvarvalue2){
      uint32_t RequestID;
      if(MSG_ReadBytes(&Data, &Size, &RequestID, sizeof(RequestID))){return -1;}

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}

      /* TODO we need to respond */
    }
    else if(DataType == svc_id_spawnbaseline){
      uintptr_t BitIndex = 0;

      while(1){
        uint16_t EntityNumber = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &EntityNumber, 11) != 0){return -1;}

        if(EntityNumber == ((uint16_t)1 << 11) - 1){
          BitIndex += 5;
          break;
        }

        uint8_t EntityType = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &EntityType, 2) != 0){return -1;}

        DeltaType_t *DeltaType;

        if(EntityType & ENTITY_NORMAL){
          if(IsEntityIndexPlayer(ssc->MaxClients, EntityNumber) == true){
            DeltaType = &DeltaType_EntityStatePlayer;
          }
          else{
            DeltaType = &DeltaType_EntityState;
          }
        }
        else{
          DeltaType = &DeltaType_CustomEntityState;
        }

        if(ParseDelta(DeltaType, Data, Size, &BitIndex)){return -1;}
      }

      uint8_t BaselineCount = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &BaselineCount, 6) != 0){return -1;}

      for(uint16_t ei = 0; ei < BaselineCount; ei++){
        if(ParseDelta(&DeltaType_EntityState, Data, Size, &BitIndex)){return -1;}
      }

      Data += BitIndex / 8 + !!(BitIndex % 8);
      Size -= BitIndex / 8 + !!(BitIndex % 8);
    }
    else if(DataType == svc_id_time){
      f32_t Time;
      if(MSG_ReadBytes(&Data, &Size, &Time, sizeof(Time))){return -1;}
    }
    else if(DataType == svc_id_lightstyle){
      uint8_t Index;
      if(MSG_ReadBytes(&Data, &Size, &Index, sizeof(Index))){return -1;}

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}
    }
    else if(DataType == svc_id_setangle){
      uint16_t Angle[3];
      if(MSG_ReadBytes(&Data, &Size, &Angle[0], sizeof(Angle[0]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &Angle[1], sizeof(Angle[1]))){return -1;}
      if(MSG_ReadBytes(&Data, &Size, &Angle[2], sizeof(Angle[2]))){return -1;}
    }
    else if(DataType == svc_id_clientdata){
      uintptr_t BitIndex = 0;

      uint8_t HaveDeltaSequence = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &HaveDeltaSequence, 1) != 0){return -1;}

      if(HaveDeltaSequence == 1){
        uint8_t DeltaSequence = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &DeltaSequence, 8) != 0){return -1;}
      }

      if(ParseDelta(&DeltaType_ClientData, Data, Size, &BitIndex)){return -1;}

      while(1){
        uint8_t HaveDelta = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &HaveDelta, 1) != 0){return -1;}

        if(HaveDelta == 0){
          break;
        }

        uint8_t Index = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &Index, 6) != 0){return -1;}

        if(ParseDelta(&DeltaType_WeaponData, Data, Size, &BitIndex)){return -1;}
      }

      Data += BitIndex / 8 + !!(BitIndex % 8);
      Size -= BitIndex / 8 + !!(BitIndex % 8);
    }
    else if(DataType == svc_id_signonnum){
      uint8_t Value;
      if(MSG_ReadBytes(&Data, &Size, &Value, sizeof(Value))){return -1;}
    }
    else if(DataType == svc_id_voiceinit){
      String_t str;
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}

      uint8_t filler;
      if(MSG_ReadBytes(&Data, &Size, &filler, sizeof(filler))){return -1;}
    }
    else if(DataType == svc_id_sound){
      uintptr_t BitIndex = 0;

      uint16_t FieldMask = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &FieldMask, 9) != 0){return -1;}

      if(FieldMask & SND_FL_VOLUME){
        uint8_t Volume = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &Volume, 8) != 0){return -1;}
      }
      if(FieldMask & SND_FL_ATTENUATION){
        uint8_t Attenuation = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &Attenuation, 8) != 0){return -1;}
      }

      uint8_t Channel = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &Channel, 3) != 0){return -1;}

      uint16_t Entity = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &Entity, MAX_EDICT_BITS) != 0){return -1;}

      uint16_t SoundNumber = 0;
      if(MSG_ReadBits(Data, &BitIndex, Size, &SoundNumber, (FieldMask & SND_FL_LARGE_INDEX) ? 0x10 : 0x08) != 0){return -1;}

      f32_t origin[3];
      if(MSG_ReadBitVec3Coord(Data, &BitIndex, Size, &origin[0], &origin[1], &origin[2])){return -1;}

      if(FieldMask & SND_FL_PITCH){
        uint8_t Pitch = 0;
        if(MSG_ReadBits(Data, &BitIndex, Size, &Pitch, 8) != 0){return -1;}
      }

      Data += BitIndex / 8 + !!(BitIndex % 8);
      Size -= BitIndex / 8 + !!(BitIndex % 8);
    }
    else if(DataType == svc_id_customization){
      uint8_t Index;
      if(MSG_ReadBytes(&Data, &Size, &Index, sizeof(Index))){return -1;}

      uint8_t Type;
      if(MSG_ReadBytes(&Data, &Size, &Type, sizeof(Type))){return -1;}

      String_t str;
      if(MSG_ReadString(&Data, &Size, &str)){return -1;}

      uint16_t ResourceIndex;
      if(MSG_ReadBytes(&Data, &Size, &ResourceIndex, sizeof(ResourceIndex))){return -1;}

      uint32_t DownloadSize;
      if(MSG_ReadBytes(&Data, &Size, &DownloadSize, sizeof(DownloadSize))){return -1;}

      uint8_t Flag;
      if(MSG_ReadBytes(&Data, &Size, &Flag, sizeof(Flag))){return -1;}

      if(Flag & RES_CUSTOM){
        uint8_t md5[16];
        if(MSG_ReadBytes(&Data, &Size, md5, sizeof(md5))){return -1;}
      }
    }
    else if(DataType == svc_id_choke){
      /* *cough cough* */
    }
    else if(
      (DataType == 'B' && Size >= 3) &&
      Data[0] == 'Z' && Data[1] == '2' && Data[2] == 0
    ){
      if(Recursived == true){
        /* server sent us bz2 bomb lol */
        return -1;
      }

      Data += 3;
      Size -= 3;

      uint8_t udata[0x8000];
      unsigned int udata_size = sizeof(udata);
      int bzr = BZ2_bzBuffToBuffDecompress((char *)udata, &udata_size, (char *)Data, Size, 0, 0);
      if(bzr != 0){
        return -1;
      }

      sint32_t r = ProcessServerData(s, udata, udata_size, true);
      if(r != 0){
        return r;
      }

      Data += Size;
      Size -= Size;
    }
    else if(DataType >= svc_id_UserMessageStart){
      umsg_t *umsg = umsg_Read(&ssc->UserMessages, DataType);
      if(umsg == NULL){
        print("umsg_Read %lx %lx\n", DataType, ssc->UserMessages.Current);
        PrintHexData(Data, Size);
        return -1;
      }

      uint8_t ReadSize = umsg->Size;
      if(ReadSize == (uint8_t)-1){
        if(MSG_ReadBytes(&Data, &Size, &ReadSize, sizeof(ReadSize))){return -1;}
      }

      uint8_t MessageData[0xff];
      if(MSG_ReadBytes(&Data, &Size, &MessageData, ReadSize)){return -1;}
    }
    else{
      Data--;
      Size++;

      print("Unknown data at ProcessServerData\n");
      PrintHexData(Data, Size);
      return -1;
    }
  }
  return 0;
}

void cb_Read(EV_t *listener, EV_event_t *e, uint32_t flag){
  uint8_t _Data[0x1000];

  NET_addr_t From;
  IO_ssize_t size = net_recvfrom(&pile.sock, _Data, sizeof(_Data), &From);
  if(size < 0){
    PR_abort();
  }

  uint8_t *Data = _Data;

  if(size < 4){
    return;
  }

  s_t *s;
  {
    s_t **_s;
    SocketMapInput_t i;
    i.SocketIndex = 0;
    i.addr = From;
    _s = SocketMap_GetOutputPointerSafe(&pile.sp.SocketMap, &i);
    if(_s == NULL){
      return;
    }
    s = *_s;
  }

  uint32_t srcSequence;
  bool srcCommand;
  bool IsFragment;

  PacketType_t PacketType;
  if(*(uint32_t *)Data == (uint32_t)-1){
    PacketType = PacketType_Connectionless;
  }
  else if(*(uint32_t *)Data == (uint32_t)-2){
    PacketType = PacketType_Split;
  }
  else{
    srcSequence = *(uint32_t *)Data & 0x3fffffff;
    srcCommand = (*(uint32_t *)Data & 0x80000000) != 0;
    IsFragment = (*(uint32_t *)Data & 0x40000000) != 0;
    PacketType = PacketType_Connected;
  }

  Data += 4;
  size -= 4;

  uint32_t dstSequence;

  if(PacketType == PacketType_Connected){
    if(size < 4){
      goto gt_DropSession;
    }
    dstSequence = *(uint32_t *)Data & 0x3fffffff;
    Data += 4;
    size -= 4;

    COM_UnMunge2(Data, size, srcSequence & 0xff);
  }

  if(PacketType == PacketType_Connectionless){
    if(s->ss == ss_GetChallenge){
      uint32_t Section = 0;
      uint32_t SectionStart = 0;
      for(uint32_t i = 0; i < size; i++){
        if(Data[i] == ' '){
          if(ProcessGetChallenge(s, Section++, &Data[SectionStart], i - SectionStart) != 0){
            goto gt_DropSession;
          }
          SectionStart = i + 1;
        }
      }
      if(ProcessGetChallenge(s, Section++, &Data[SectionStart], size - SectionStart) != 0){
        goto gt_DropSession;
      }
    }
    else if(s->ss == ss_Connect_0){
      print("connect answer:\n%.*s\n", size, Data);

      /* TODO check it */
      s->ss = ss_Connected;

      ssc_Open(&s->c);

      SendConnection(s);
    }
    else{
      goto gt_DropSession;
    }
  }
  else if(PacketType == PacketType_Split){
    goto gt_DropSession;
  }
  else if(PacketType == PacketType_Connected){
    if(s->ss != ss_Connected){
      goto gt_DropSession;
    }

    if((s->c.dstSequence & 0x7fffffff) + 1 > srcSequence){
      /* lets act like we didnt see this packet */
      return;
    }

    if(dstSequence == s->c.srcSequence - 1 && s->c.lr.Size != (uintptr_t)-1){
      s->c.lr.Size = (uintptr_t)-1;
      EV_timer_stop(&pile.listener, &s->c.lr.NoACKTimer);
    }

    s->c.dstSequence = srcSequence;
    if(srcCommand){
      s->c.dstSequence ^= 0x80000000;
    }

    if(IsFragment == true){
      frag_head_t fhead[MaxFragmentStreams];
      uint8_t fexist[MaxFragmentStreams] = {0};

      for(uint32_t fid = 0; fid < MaxFragmentStreams; fid++){
        if(size < 1){
          goto gt_DropSession;
        }
        fexist[fid] = *Data;
        Data += 1;
        size -= 1;

        if(fexist[fid] == 1){
          if(size < sizeof(frag_head_t)){
            goto gt_DropSession;
          }
          fhead[fid] = *(frag_head_t *)Data;
          Data = &Data[sizeof(frag_head_t)];
          size -= sizeof(frag_head_t);

          if(fhead[fid].StartPosition != 0){
            /* need to be implemented */
            goto gt_DropSession;
          }
        }
        else if(fexist[fid] != 0){
          goto gt_DropSession;
        }
      }

      for(uint32_t fid = 0; fid < MaxFragmentStreams; fid++){
        if(fexist[fid] == 0){
          continue;
        }

        if(fid > 0){
          /* needs to be implemented */
          goto gt_DropSession;
        }

        if(fhead[fid].Size > size){
          print("fhead[fid].Size > size %lx %x\n", fhead[fid].Size, size);
          goto gt_DropSession;
        }

        if(s->c.ServerFragment.Current == 0){
          if(fhead[fid].At != 1){
            goto gt_DropSession;
          }
        }
        else{
          if(fhead[fid].At == 1){
            goto gt_DropSession;
          }
        }

        uintptr_t oc = s->c.ServerFragment.Current; /* old current */
        VEC_handle0(&s->c.ServerFragment, fhead[fid].Size);
        MEM_copy(Data, &((uint8_t *)s->c.ServerFragment.ptr)[oc], fhead[fid].Size);

        Data += fhead[fid].Size;
        size -= fhead[fid].Size;

        if(fhead[fid].At == fhead[fid].To){
          if(ProcessServerData(s, s->c.ServerFragment.ptr, s->c.ServerFragment.Current, false) != 0){
            goto gt_DropSession;
          }
          s->c.ServerFragment.Current = 0;
        }
      }

      if(size != 0){
        if(Data[0] != 0x02){
          goto gt_DropSession;
        }
        Data += 1;
        size -= 1;
      }
    }

    if(size != 0){
      if(ProcessServerData(s, Data, size, false) != 0){
        goto gt_DropSession;
      }
    }

    SendACK(s);
  }

  return;

  gt_DropSession:;

  s_Close(s);
}

void cb_InputEvent(EV_t *listener, EV_event_t *e, uint32_t flag){
  uint8_t Input[0x1000];

  IO_fd_t fd;
  IO_fd_set(&fd, STDIN_FILENO);
  IO_ssize_t size = IO_read(&fd, Input, sizeof(Input));
  if(size < 0){
    PR_abort();
  }

  uint32_t el = 0;
  for(; el < size; el++){
    if(Input[el] == '\n'){
      break;
    }
  }

  if(el == size){
    return;
  }

  /* ~and the silence never ends~ */
}

int main(){
  EV_open(&pile.listener);

  sp_Open();

  {
    sint32_t err = NET_socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP, &pile.sock);
    if(err != 0){
      PR_abort();
    }
  }

  {
    NET_addr_t addr;
    addr.ip = 0x900d7a11;
    addr.port = 27015;
    sp_AddServer(&addr);
  }

  {
    IO_fd_t fd;
    IO_fd_set(&fd, STDIN_FILENO);
    EV_event_init_fd(&pile.InputEvent, &fd, cb_InputEvent, EV_READ);
    EV_event_start(&pile.listener, &pile.InputEvent);
  }

  EV_event_init_socket(&pile.Read, &pile.sock, cb_Read, EV_READ);
  EV_event_start(&pile.listener, &pile.Read);

  EV_start(&pile.listener);

  return 0;
}
