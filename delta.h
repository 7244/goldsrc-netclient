typedef uint32_t DeltaType_FieldFlag_t;
#define DeltaType_FieldFlag_BYTE ((DeltaType_FieldFlag_t)1 << 0)
#define DeltaType_FieldFlag_SHORT ((DeltaType_FieldFlag_t)1 << 1)
#define DeltaType_FieldFlag_FLOAT ((DeltaType_FieldFlag_t)1 << 2)
#define DeltaType_FieldFlag_INTEGER ((DeltaType_FieldFlag_t)1 << 3)
#define DeltaType_FieldFlag_ANGLE ((DeltaType_FieldFlag_t)1 << 4)
#define DeltaType_FieldFlag_TIMEWINDOW_8 ((DeltaType_FieldFlag_t)1 << 5)
#define DeltaType_FieldFlag_TIMEWINDOW_BIG	((DeltaType_FieldFlag_t)1 << 6)
#define DeltaType_FieldFlag_STRING ((DeltaType_FieldFlag_t)1 << 7)
#define DeltaType_FieldFlag_SIGNED ((DeltaType_FieldFlag_t)1 << 31)

typedef struct{
  const char *FieldName;
  DeltaType_FieldFlag_t FieldFlag;
  uint8_t Bits;
  f32_t Multipler;
}DeltaType_Field_t;

typedef struct{
  const char *DeltaName;
  uint8_t FieldAmount;
  DeltaType_Field_t Fields[64];
}DeltaType_t;

DeltaType_t DeltaType_Event = {
  .DeltaName = "event_t",
  .FieldAmount = 0x0e,
  .Fields = {
    {"entindex", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"bparam1", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"bparam2", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"origin[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"origin[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"origin[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"fparam1", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 20, 100.0},
    {"fparam2", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 20, 100.0},
    {"iparam1", DeltaType_FieldFlag_INTEGER | DeltaType_FieldFlag_SIGNED, 16, 1.0},
    {"iparam2", DeltaType_FieldFlag_INTEGER | DeltaType_FieldFlag_SIGNED, 16, 1.0},
    {"angles[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"angles[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"angles[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"ducking", DeltaType_FieldFlag_INTEGER, 1, 1.0}
  }
};
DeltaType_t DeltaType_WeaponData = {
  .DeltaName = "weapon_data_t",
  .FieldAmount = 0x14,
  .Fields = {
    {"m_flTimeWeaponIdle", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_flNextPrimaryAttack", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_flNextReload", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_fNextAimBonus", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_flNextSecondaryAttack", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_iClip", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"m_flPumpTime", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"m_fInSpecialReload", DeltaType_FieldFlag_INTEGER, 2, 1.0},
    {"m_fReloadTime", DeltaType_FieldFlag_FLOAT, 16, 100.0},
    {"m_fInReload", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"m_fAimedDamage", DeltaType_FieldFlag_FLOAT, 6, 0.1},
    {"m_fInZoom", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"m_iWeaponState", DeltaType_FieldFlag_INTEGER, 2, 1.0},
    {"m_iId", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"iuser1", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"iuser2", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"iuser3", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"fuser1", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 22, 1000.0},
    {"fuser2", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 22, 128.0},
    {"fuser3", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 22, 128.0}
  }
};
DeltaType_t DeltaType_UserCMD = {
  .DeltaName = "usercmd_t",
  .FieldAmount = 0x0f,
  .Fields = {
    {"lerp_msec", DeltaType_FieldFlag_SHORT, 9, 1.0},
    {"msec", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"viewangles[1]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"viewangles[0]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"buttons", DeltaType_FieldFlag_SHORT, 16, 1.0},
    {"forwardmove", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 12, 1.0},
    {"lightlevel", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"sidemove", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 12, 1.0},
    {"upmove", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 12, 1.0},
    {"impulse", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"viewangles[2]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"impact_index", DeltaType_FieldFlag_INTEGER, 6, 1.0},
    {"impact_position[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"impact_position[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"impact_position[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0}
  }
};
DeltaType_t DeltaType_CustomEntityState = {
  .DeltaName = "custom_entity_state_t",
  .FieldAmount = 0x13,
  .Fields = {
    {"rendermode", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"origin[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"origin[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"origin[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"angles[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"angles[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"angles[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 17, 8.0},
    {"sequence", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"skin", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"modelindex", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"scale", DeltaType_FieldFlag_FLOAT, 8, 1.0},
    {"body", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"rendercolor.r", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.g", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.b", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"renderfx", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"renderamt", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"frame", DeltaType_FieldFlag_FLOAT, 8, 1.0},
    {"animtime", DeltaType_FieldFlag_FLOAT, 8, 1.0}
  }
};
DeltaType_t DeltaType_EntityStatePlayer = {
  .DeltaName = "entity_state_player_t",
  .FieldAmount = 0x31,
  .Fields = {
    {"animtime", DeltaType_FieldFlag_TIMEWINDOW_8, 8, 1.0},
    {"frame", DeltaType_FieldFlag_FLOAT, 8, 1.0},
    {"origin[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 18, 32.0},
    {"angles[0]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"angles[1]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"origin[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 18, 32.0},
    {"origin[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 18, 32.0},
    {"gaitsequence", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"sequence", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"modelindex", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"movetype", DeltaType_FieldFlag_INTEGER, 4, 1.0},
    {"solid", DeltaType_FieldFlag_SHORT, 3, 1.0},
    {"mins[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"mins[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"mins[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"weaponmodel", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"team", DeltaType_FieldFlag_INTEGER, 4, 1.0},
    {"playerclass", DeltaType_FieldFlag_INTEGER, 4, 1.0},
    {"owner", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"effects", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"angles[2]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"colormap", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"framerate", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 8, 16.0},
    {"skin", DeltaType_FieldFlag_SHORT | DeltaType_FieldFlag_SIGNED, 9, 1.0},
    {"controller[0]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[1]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[2]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[3]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"blending[0]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"blending[1]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"body", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"rendermode", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"renderamt", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"renderfx", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"scale", DeltaType_FieldFlag_FLOAT, 16, 256.0},
    {"rendercolor.r", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.g", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.b", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"friction", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"usehull", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"gravity", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 32.0},
    {"aiment", DeltaType_FieldFlag_INTEGER, 11, 1.0},
    {"basevelocity[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"basevelocity[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"basevelocity[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"spectator", DeltaType_FieldFlag_INTEGER, 1, 1.0}
  }
};
DeltaType_t DeltaType_EntityState = {
  .DeltaName = "entity_state_t",
  .FieldAmount = 0x34,
  .Fields = {
    {"animtime", DeltaType_FieldFlag_TIMEWINDOW_8, 8, 1.0},
    {"frame", DeltaType_FieldFlag_FLOAT, 10, 4.0},
    {"origin[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"angles[0]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"angles[1]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"origin[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"origin[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"sequence", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"modelindex", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"movetype", DeltaType_FieldFlag_INTEGER, 4, 1.0},
    {"solid", DeltaType_FieldFlag_SHORT, 3, 1.0},
    {"mins[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"mins[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"mins[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"maxs[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"endpos[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"endpos[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"endpos[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"startpos[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"startpos[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"startpos[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 13, 1.0},
    {"impacttime", DeltaType_FieldFlag_TIMEWINDOW_BIG, 13, 100.0},
    {"starttime", DeltaType_FieldFlag_TIMEWINDOW_BIG, 13, 100.0},
    {"weaponmodel", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"owner", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"effects", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"eflags", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"angles[2]", DeltaType_FieldFlag_ANGLE, 16, 1.0},
    {"colormap", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"framerate", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 8, 16.0},
    {"skin", DeltaType_FieldFlag_SHORT | DeltaType_FieldFlag_SIGNED, 9, 1.0},
    {"controller[0]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[1]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[2]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"controller[3]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"blending[0]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"blending[1]", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"body", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"rendermode", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"renderamt", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"renderfx", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"scale", DeltaType_FieldFlag_FLOAT, 16, 256.0},
    {"rendercolor.r", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.g", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"rendercolor.b", DeltaType_FieldFlag_BYTE, 8, 1.0},
    {"aiment", DeltaType_FieldFlag_INTEGER, 11, 1.0},
    {"basevelocity[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"basevelocity[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"basevelocity[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"playerclass", DeltaType_FieldFlag_INTEGER, 1, 1.0}
  }
};
DeltaType_t DeltaType_ClientData = {
  .DeltaName = "clientdata_t",
  .FieldAmount = 0x32,
  .Fields = {
    {"flTimeStepSound", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"origin[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 21, 128.0},
    {"origin[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 21, 128.0},
    {"velocity[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"velocity[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"m_flNextAttack", DeltaType_FieldFlag_FLOAT | DeltaType_FieldFlag_SIGNED, 22, 1000.0},
    {"origin[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 21, 128.0},
    {"velocity[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"ammo_nails", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"ammo_shells", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"ammo_cells", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"ammo_rockets", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"m_iId", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"punchangle[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"flags", DeltaType_FieldFlag_INTEGER, 32, 1.0},
    {"weaponanim", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"health", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"maxspeed", DeltaType_FieldFlag_FLOAT, 16, 10.0},
    {"flDuckTime", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"view_ofs[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 4.0},
    {"punchangle[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"punchangle[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 16, 8.0},
    {"viewmodel", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"weapons", DeltaType_FieldFlag_INTEGER, 32, 1.0},
    {"pushmsec", DeltaType_FieldFlag_INTEGER, 11, 1.0},
    {"deadflag", DeltaType_FieldFlag_INTEGER, 3, 1.0},
    {"fov", DeltaType_FieldFlag_FLOAT, 8, 1.0},
    {"physinfo", DeltaType_FieldFlag_STRING, 1, 1.0},
    {"bInDuck", DeltaType_FieldFlag_INTEGER, 1, 1.0},
    {"flSwimTime", DeltaType_FieldFlag_INTEGER, 10, 1.0},
    {"waterjumptime", DeltaType_FieldFlag_INTEGER, 15, 1.0},
    {"waterlevel", DeltaType_FieldFlag_INTEGER, 2, 1.0},
    {"iuser1", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"iuser2", DeltaType_FieldFlag_INTEGER, 5, 1.0},
    {"vuser1[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser1[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser1[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser2[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser2[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser2[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser3[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser3[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser3[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser4[0]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser4[1]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"vuser4[2]", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 1.0},
    {"fuser1", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 22, 128.0},
    {"fuser2", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 128.0},
    {"fuser3", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 10, 128.0},
    {"fuser4", DeltaType_FieldFlag_SIGNED | DeltaType_FieldFlag_FLOAT, 2, 128.0}
  }
};

DeltaType_t *DeltaTypes[] = {
  &DeltaType_Event,
  &DeltaType_WeaponData,
  &DeltaType_UserCMD,
  &DeltaType_CustomEntityState,
  &DeltaType_EntityStatePlayer,
  &DeltaType_EntityState,
  &DeltaType_ClientData
};

DeltaType_t DeltaType_MetaDescription = {
  .FieldAmount = 0x07,
  .Fields = {
    {"fieldType", DeltaType_FieldFlag_INTEGER, 32, 1.0},
    {"fieldName", DeltaType_FieldFlag_STRING, 1,  1.0,},
    {"fieldOffset", DeltaType_FieldFlag_INTEGER, 16, 1.0},
    {"fieldSize", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"significant_bits", DeltaType_FieldFlag_INTEGER, 8, 1.0},
    {"premultiply", DeltaType_FieldFlag_FLOAT, 32, 4000.0},
    {"postmultiply", DeltaType_FieldFlag_FLOAT, 32, 4000.0}
  }
};

typedef struct{
  int sendcount;
  int receivedcount;
}delta_stats_t;

typedef struct{
  int fieldType;
  char fieldName[32];
  int fieldOffset;

  short int fieldSize;
  int significant_bits;
  float premultiply;
  float postmultiply;
  short int flags;
  delta_stats_t stats;
}delta_description_t;

typedef struct{
  char *fieldName;
  size_t fieldOffset;
}delta_definition_t;

typedef struct{
  int flags;

  int entindex;

  float origin[3];
  float angles[3];
  float velocity[3];

  int ducking;

  float fparam1;
  float fparam2;

  int iparam1;
  int iparam2;

  int bparam1;
  int bparam2;
}DeltaType_event_t;

DeltaType_t *FindDeltaType(const void *Name, uintptr_t NameSize){
  for(uint32_t i = 0; i < sizeof(DeltaTypes) / sizeof(DeltaTypes[0]); i++){
    if(NameSize == MEM_cstreu(DeltaTypes[i]->DeltaName) && STR_ncmp(Name, DeltaTypes[i]->DeltaName, NameSize) == 0){
      return DeltaTypes[i];
    }
  }
  return NULL;
}

sint32_t ParseDelta(DeltaType_t *DeltaType, uint8_t *Data, uintptr_t Size, uintptr_t *BitIndex){
  uint8_t ByteCount = 0;
  if(MSG_ReadBits(Data, BitIndex, Size, &ByteCount, 3) != 0){return -1;}

  if(ByteCount > DeltaType->FieldAmount / 8 + !!(DeltaType->FieldAmount % 8)){
    return -1;
  }

  uint8_t FDR_MARK_BitArray[8] = {0};

  for(uint32_t i = 0; i < ByteCount; i++){
    if(MSG_ReadBits(Data, BitIndex, Size, &FDR_MARK_BitArray[i], 8) != 0){return -1;}
  }

  uint32_t ToIterate = ByteCount * 8;
  if(ToIterate > DeltaType->FieldAmount){
    ToIterate = DeltaType->FieldAmount;
  }

  for(uint32_t i = 0; i < ToIterate; i++){
    uint32_t FieldID = i;

    if(*(uint64_t *)FDR_MARK_BitArray & 1){
      if(DeltaType->Fields[FieldID].FieldFlag & DeltaType_FieldFlag_STRING){
        uint8_t BitString[32];
        MEM_set(0, BitString, sizeof(BitString));
        uint32_t iBitString = 0;

        while(1){
          if(iBitString == sizeof(BitString)){
            return -1;
          }

          if(MSG_ReadBits(Data, BitIndex, Size, &BitString[iBitString], 8) != 0){return -1;}
          if(BitString[iBitString] == 0){
            break;
          }
          iBitString++;
        }
      }
      else{
        uint32_t filler = 0;
        if(MSG_ReadBits(Data, BitIndex, Size, &filler, DeltaType->Fields[FieldID].Bits) != 0){return -1;}
      }
    }

    *(uint64_t *)FDR_MARK_BitArray >>= 1;
  }

  return 0;
}
