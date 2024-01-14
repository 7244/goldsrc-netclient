/* session info list */
#define BLL_set_prefix sil
#define bll_set_type_node uint16_t
#define BLL_set_declare_NodeReference 1
#define BLL_set_declare_rest 0
#define BLL_set_Language 0
/* TODO use some witch constant */
#define BLL_set_UseUninitialisedValues 0
#include <WITCH/BLL/BLL.h>
/* session start list */
#define BLL_set_prefix ssl
#define bll_set_type_node uint16_t
#define BLL_set_declare_NodeReference 1
#define BLL_set_declare_rest 0
#define BLL_set_Language 0
/* TODO use some witch constant */
#define BLL_set_UseUninitialisedValues 0
#include <WITCH/BLL/BLL.h>

/* session info list */
#define BLL_set_prefix sil
#define bll_set_type_node uint16_t
#define BLL_set_declare_NodeReference 0
#define BLL_set_declare_rest 1
#define BLL_set_NodeData \
  NET_addr_t addr; \
  uint32_t StartedSessionCount; \
  ssl_NodeReference_t ssnr;
#define BLL_set_Language 0
/* TODO use some witch constant */
#define BLL_set_UseUninitialisedValues 0
#include <WITCH/BLL/BLL.h>
/* session start list */
#define BLL_set_prefix ssl
#define bll_set_type_node uint16_t
#define BLL_set_declare_NodeReference 0
#define BLL_set_declare_rest 1
#define BLL_set_NodeData \
  sil_NodeReference_t sinr;
#define BLL_set_Language 0
/* TODO use some witch constant */
#define BLL_set_UseUninitialisedValues 0
#include <WITCH/BLL/BLL.h>

#define BLL_set_prefix SessionTimeOutList
#define bll_set_type_node uint16_t
#define BLL_set_NodeDataType s_t *
#define BLL_set_Language 0
/* TODO use some witch constant */
#define BLL_set_UseUninitialisedValues 0
#include <WITCH/BLL/BLL.h>

struct s_t{
  uint8_t SocketIndex;
  sil_NodeReference_t sinr;

  ss_t ss;

  SessionTimeOutList_NodeReference_t timeoutnr;
  uint64_t LastAnswerTime;

  uint8_t Challenge[10];
  uint8_t ChallengeSize;

  ssc_t c;
};

#pragma pack(push, 1)
  typedef struct{
    uint8_t SocketIndex;
    NET_addr_t addr;
  }SocketMapInput_t;
#pragma pack(pop)

#define MAP_set_Prefix SocketMap
#define MAP_set_InputType SocketMapInput_t
#define MAP_set_OutputType s_t *
#include <WITCH/MAP/MAP.h>
