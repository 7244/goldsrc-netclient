#define RES_FATALIFMISSING (1<<0)
#define RES_WASMISSING     (1<<1)
#define RES_CUSTOM         (1<<2)

#define RES_REQUESTED	   (1<<3)	// Already requested a download of this one
#define RES_PRECACHED	   (1<<4)	// Already precached
#define RES_ALWAYS       (1<<5)	// download always even if available on client
#define RES_UNK_6        (1<<6) // TODO: what is it?
#define RES_CHECKFILE	   (1<<7)	// check file on client

typedef struct{
  BitString_t Name;
  uint8_t Flag;
  uint8_t md5[16];
  uint8_t reserved[32];
  bool NeedConsistency;
}Resource_t;
