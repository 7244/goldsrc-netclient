#define ENTITY_NORMAL (1<<0)
#define ENTITY_BEAM   (1<<1)

#define MAX_EDICT_BITS 11

bool IsEntityIndexPlayer(uint8_t MaxClients, uint16_t EntityIndex){
  if(EntityIndex >= 1 && EntityIndex <= MaxClients){
    return true;
  }
  return false;
}
