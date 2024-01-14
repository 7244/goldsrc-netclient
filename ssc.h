typedef struct{ /* ss_Connected */
  uint32_t srcSequence;
  uint32_t dstSequence;

  uint8_t ConnectionState;

  struct{
    uint8_t Data[0x500];
    uintptr_t Size;
    EV_timer_t NoACKTimer;
  }lr; /* loss recover */

  ClientFragment_t ClientFragment;

  VEC_t ServerFragment; /* 1 */

  uint32_t SpawnCount;

  uint32_t worldmapCRC;
  uint8_t MaxClients;
  uint8_t PlayerNumber;

  VEC_t Resources; /* Resource_t */

  VEC_t UserMessages; /* umsg_t */
}ssc_t;
