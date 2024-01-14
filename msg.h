typedef struct{
  uint8_t *Data;
  uintptr_t Size;
}String_t;
typedef struct{
  uint8_t Data[64];
  uintptr_t Size;
}BitString_t;

sint32_t MSG_ReadString(uint8_t **Data, uintptr_t *Size, String_t *String){
  String->Data = *Data;
  String->Size = 0;

  while(String->Size != *Size){
    if((*Data)[String->Size] == 0){
      *Data += String->Size + 1;
      *Size -= String->Size + 1;

      return 0;
    }

    String->Size++;
  }

  *Data += *Size;
  *Size = 0;

  return -1;
}

sint32_t MSG_ReadBytes(
  uint8_t **Source, uintptr_t *SourceSize,
  void *Destination, uintptr_t DestinationSize
){
  if(*SourceSize < DestinationSize){
    return -1;
  }

  for(uintptr_t i = 0; i < DestinationSize; i++){
    ((uint8_t *)Destination)[i] = (*Source)[i];
  }

  *Source += DestinationSize;
  *SourceSize -= DestinationSize;

  return 0;
}

sint32_t MSG_ReadBits(
  uint8_t *Source, uintptr_t *SourceIndex, uintptr_t SourceSize,
  void *Destination, uintptr_t DestinationSize
){
  if(SourceSize * 8 - *SourceIndex < DestinationSize){
    return -1;
  }

  uintptr_t DestinationIndex = 0;

  while(DestinationSize){
    uint8_t m = *SourceIndex % 8;
    uint8_t l = 8 - m;
    if(l > DestinationSize){
      l = DestinationSize;
    }
    DestinationSize -= l;

    uint8_t n = Source[*SourceIndex / 8];
    *SourceIndex += l;

    n >>= m;
    n &= (uint8_t)(((uint16_t)1 << l) - 1);

    while(l){
      uint8_t dm = DestinationIndex % 8;
      uint8_t dl = 8 - dm;
      if(dl > l){
        dl = l;
      }

      ((uint8_t *)Destination)[DestinationIndex / 8] |= n << dm;

      l -= dl;
      n >>= dl;
      DestinationIndex += dl;
    }
  }

  return 0;
}

sint32_t MSG_ReadBitString(
  uint8_t *Source, uintptr_t *SourceIndex, uintptr_t SourceSize,
  BitString_t *BitString
){
  BitString->Size = 0;

  while(1){
    if(BitString->Size == sizeof(BitString->Data)){
      PR_abort();
    }

    BitString->Data[BitString->Size] = 0;
    if(MSG_ReadBits(Source, SourceIndex, SourceSize, &BitString->Data[BitString->Size], 8) != 0){
      return -1;
    }

    if(BitString->Data[BitString->Size] == 0){
      break;
    }

    BitString->Size++;
  }

  return 0;
}

sint32_t MSG_ReadBitCoord(
  uint8_t *Source, uintptr_t *SourceIndex, uintptr_t SourceSize,
  f32_t *f
){
  uint16_t intval = 0;
  if(MSG_ReadBits(Source, SourceIndex, SourceSize, &intval, 1)){return -1;}
  uint8_t fractval = 0;
  if(MSG_ReadBits(Source, SourceIndex, SourceSize, &fractval, 1)){return -1;}

  if(intval || fractval){
    uint8_t signbit = 0;
    if(MSG_ReadBits(Source, SourceIndex, SourceSize, &signbit, 1)){return -1;}

    if(intval){
      intval = 0;
      if(MSG_ReadBits(Source, SourceIndex, SourceSize, &intval, 12)){return -1;}
    }

    if(fractval){
      fractval = 0;
      if(MSG_ReadBits(Source, SourceIndex, SourceSize, &fractval, 3)){return -1;}
    }

    *f = (f32_t)(fractval / 8.0 + intval);

    if(signbit){
      *f = -*f;
    }
  }

  return 0;
}

sint32_t MSG_ReadBitVec3Coord(
  uint8_t *Source, uintptr_t *SourceIndex, uintptr_t SourceSize,
  f32_t *x, f32_t *y, f32_t *z
){
  *x = 0;
  *y = 0;
  *z = 0;

  uint8_t xflag = 0;
  uint8_t yflag = 0;
  uint8_t zflag = 0;
  if(MSG_ReadBits(Source, SourceIndex, SourceSize, &xflag, 1)){return -1;}
  if(MSG_ReadBits(Source, SourceIndex, SourceSize, &yflag, 1)){return -1;}
  if(MSG_ReadBits(Source, SourceIndex, SourceSize, &zflag, 1)){return -1;}

  if(xflag){
    if(MSG_ReadBitCoord(Source, SourceIndex, SourceSize, x)){return -1;}
  }
  if(yflag){
    if(MSG_ReadBitCoord(Source, SourceIndex, SourceSize, y)){return -1;}
  }
  if(zflag){
    if(MSG_ReadBitCoord(Source, SourceIndex, SourceSize, z)){return -1;}
  }

  return 0;
}

sint32_t MSG_WriteBits(
  void *Source, uintptr_t SourceSize,
  uint8_t *Destination, uintptr_t *DestinationIndex, uintptr_t DestinationSize
){
  if(SourceSize > DestinationSize * 8 - *DestinationIndex){
    return -1;
  }

  uintptr_t SourceIndex = 0;

  while(SourceSize){
    uint8_t m = SourceIndex % 8;
    uint8_t l = 8 - m;
    if(l > SourceSize){
      l = SourceSize;
    }
    SourceSize -= l;

    uint8_t n = ((uint8_t *)Source)[SourceIndex / 8];
    SourceIndex += l;

    n >>= m;
    n &= (uint8_t)(((uint16_t)1 << l) - 1);

    while(l){
      uint8_t dm = *DestinationIndex % 8;
      uint8_t dl = 8 - dm;
      if(dl > l){
        dl = l;
      }

      if(dm == 0){
        Destination[*DestinationIndex / 8] = 0;
      }

      Destination[*DestinationIndex / 8] |= n << dm;

      l -= dl;
      n >>= dl;
      *DestinationIndex += dl;
    }
  }

  return 0;
}
