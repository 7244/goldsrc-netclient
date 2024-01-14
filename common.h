#include <WITCH/WITCH.h>

#include <WITCH/PR/PR.h>

#include <WITCH/EV/EV.h>

#include <WITCH/IO/IO.h>
#include <WITCH/IO/print.h>

void print(const char *format, ...){
  IO_fd_t fd_stdout;
  IO_fd_set(&fd_stdout, FD_OUT);
  va_list argv;
  va_start(argv, format);
  IO_vprint(&fd_stdout, format, argv);
  va_end(argv);
}

#include <WITCH/RAND/RAND.h>

//#define set_PacketLoss 0.0

IO_ssize_t net_recvfrom(NET_socket_t *sock, void *Data, uintptr_t Size, NET_addr_t *from){
  IO_ssize_t s = NET_recvfrom(sock, Data, Size, from);
  if(s < 0){
    print("net_recvfrom s %d\n", s);
    PR_abort();
  }

  #ifdef set_PacketLoss
    f32_t v = (f32_t)RAND_soft_16() / 0xffff;
    if(v < set_PacketLoss){
      print("loss came\n");
      return 0;
    }
  #endif

  return s;
}

void net_sendto(NET_socket_t *sock, const void *Data, uintptr_t Size, NET_addr_t *to){
  /*
  #ifdef set_PacketLoss
    f32_t v = (f32_t)RAND_soft_16() / 0xffff;
    if(v < set_PacketLoss){
      print("loss came\n");
      return;
    }
  #endif
  */

  IO_ssize_t s = NET_sendto(sock, Data, Size, to);
  if(s != Size){
    print("net_sendto s %d\n", s);
    PR_abort();
  }
}

void PrintHexData(const void *Data, uintptr_t Size){
  for(uint32_t i = 0; i < Size; i++){
    print("%02lx", ((uint8_t *)Data)[i]);
  }
  print("\n");
}

#include <WITCH/NET/NET.h>

#include <WITCH/VEC/VEC.h>
#include <WITCH/VEC/print.h>

#include <WITCH/STR/psu.h>
#include <WITCH/STR/pss.h>
#include <WITCH/STR/psh.h>

#include <WITCH/FS/FS.h>

void SaveToFile(const char *FileName, const void *Data, uintptr_t DataSize){
  FS_file_t file;
  sint32_t err = FS_file_open(FileName, &file, O_CREAT | O_WRONLY);
  if(err != 0){
    print("SaveToFile failed with %lx\n", err);
    PR_abort();
  }

  for(uintptr_t i = 0; i != DataSize;){
    sintptr_t s = FS_file_write(&file, Data, DataSize);
    if(s < 0){
      print("FS_file_write err %x\n", s);
      PR_abort();
    }
    i += s;
  }

  FS_file_close(&file);
}
void ReadFromFile(const char *FileName, VEC_t *Data){
  FS_file_t file;
  sint32_t err = FS_file_open(FileName, &file, O_RDONLY);
  if(err != 0){
    print("SaveToFile failed with %lx\n", err);
    PR_abort();
  }

  IO_fd_t filefd;
  FS_file_getfd(&file, &filefd);

  IO_stat_t s;
  err = IO_fstat(&filefd, &s);
  if(err != 0){
    PR_abort();
  }

  IO_off_t FileSize = IO_stat_GetSizeInBytes(&s);

  uintptr_t CurrentDataCurrent = Data->Current;
  VEC_handle0(Data, FileSize);

  sintptr_t frs = FS_file_read(&file, &((uint8_t *)Data->ptr)[CurrentDataCurrent], FileSize);
  if(frs != FileSize){
    PR_abort();
  }

  FS_file_close(&file);
}

#include <openssl/evp.h>

sint32_t FileToMD5(const void *FileName, uint8_t *md5){
  FS_file_t file;

  sint32_t err = FS_file_open(FileName, &file, O_RDONLY);
  if(err != 0){
    return -1;
  }

  EVP_MD_CTX *mdctx;
  mdctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);

  while(1){
    uint8_t Data[0x2000];
    sintptr_t frs = FS_file_read(&file, Data, sizeof(Data));
    if(frs < 0){
      PR_abort();
    }
    if(frs == 0){
      break;
    }

    EVP_DigestUpdate(mdctx, Data, frs);

    if(frs != sizeof(Data)){
      break;
    }
  }

  unsigned int md5_digest_len = 16;
  EVP_DigestFinal_ex(mdctx, md5, &md5_digest_len);
  EVP_MD_CTX_free(mdctx);

  FS_file_close(&file);

  return 0;
}

#include <bzlib.h>
