typedef enum{
  svc_id_bad = 0x00,
  svc_id_nop = 0x01,
  svc_id_disconnect = 0x02,
  svc_id_event = 0x03,
  svc_id_version = 0x04,
  svc_id_setview = 0x05,
  svc_id_sound = 0x06,
  svc_id_time = 0x07,
  svc_id_print = 0x08,
  svc_id_stufftext = 0x09,
  svc_id_setangle = 0x0a,
  svc_id_serverinfo = 0x0b,
  svc_id_lightstyle = 0x0c,
  svc_id_updateuserinfo = 0x0d,
  svc_id_deltadescription = 0x0e,
  svc_id_clientdata = 0x0f,
  svc_id_stopsound = 0x10,
  svc_id_pings = 0x11,
  svc_id_particle = 0x12,
  svc_id_damage = 0x13,
  svc_id_spawnstatic = 0x14,
  svc_id_event_reliable = 0x15,
  svc_id_spawnbaseline = 0x16,
  svc_id_temp_entity = 0x17,
  svc_id_setpause = 0x18,
  svc_id_signonnum = 0x19,
  svc_id_centerprint = 0x1a,
  svc_id_killedmonster = 0x1b,
  svc_id_foundsecret = 0x1c,
  svc_id_spawnstaticsound = 0x1d,
  svc_id_intermission = 0x1e,
  svc_id_finale = 0x1f,
  svc_id_cdtrack = 0x20,
  svc_id_restore = 0x21,
  svc_id_cutscene = 0x22,
  svc_id_weaponanim = 0x23,
  svc_id_decalname = 0x24,
  svc_id_roomtype = 0x25,
  svc_id_addangle = 0x26,
  svc_id_newusermsg = 0x27,
  svc_id_packetentities = 0x28,
  svc_id_deltapacketentities = 0x29,
  svc_id_choke = 0x2a,
  svc_id_resourcelist = 0x2b,
  svc_id_newmovevars = 0x2c,
  svc_id_resourcerequest = 0x2d,
  svc_id_customization = 0x2e,
  svc_id_crosshairangle = 0x2f,
  svc_id_soundfade = 0x30,
  svc_id_filetxferfailed = 0x31,
  svc_id_hltv = 0x32,
  svc_id_director = 0x33,
  svc_id_voiceinit = 0x34,
  svc_id_voicedata = 0x35,
  svc_id_sendextrainfo = 0x36,
  svc_id_timescale = 0x37,
  svc_id_resourcelocation = 0x38,
  svc_id_sendcvarvalue = 0x39,
  svc_id_sendcvarvalue2 = 0x3a,
  svc_id_exec = 0x3b,
  svc_id_UserMessageStart = 0x40
}svc_id_t;

#pragma pack(push, 1)
  typedef struct{
    uint32_t ProtocolVersion;
    uint32_t SpawnCount;
    uint32_t Munge3_worldmapCRC; /* (-1 - playernum) & 0xFF */
    uint8_t md5_ClientDLL[0x10];
    uint8_t MaxClients;
    uint8_t PlayerNumber;
    uint8_t Unknown0;
  }svc_data_serverinfo;

  typedef struct{
    f32_t gravity;
    f32_t stopspeed;
    f32_t maxspeed;
    f32_t spectatormaxspeed;
    f32_t accelerate;
    f32_t airaccelerate;
    f32_t wateraccelerate;
    f32_t friction;
    f32_t edgefriction;
    f32_t waterfriction;
    f32_t entgravity;
    f32_t bounce;
    f32_t stepsize;
    f32_t maxvelocity;
    f32_t zmax;
    f32_t waveHeight;
    uint8_t footsteps;
    f32_t rollangle;
    f32_t rollspeed;
    f32_t skycolor_r;
    f32_t skycolor_g;
    f32_t skycolor_b;
    f32_t skyvec_x;
    f32_t skyvec_y;
    f32_t skyvec_z;
  }svc_data_newmovevars;

  typedef struct{
    uint8_t i;
    uint8_t Size;
    uint8_t Data[16];
  }svc_data_newusermsg;
#pragma pack(pop)