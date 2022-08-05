
// ESP8266 & ESP32 version

typedef unsigned char  _u8;
typedef unsigned short _u16;
typedef unsigned int   _u32;
typedef unsigned long long _u64;
typedef _u8           *_u8p;
typedef _u16          *_u16p;
typedef _u32          *_u32p;
typedef _u64          *_u64p;

typedef char           _i8;
typedef short          _i16;
typedef int            _i32;
typedef long long      _i64;
typedef _i8           *_i8p;
typedef _i16          *_i16p;
typedef _i32          *_i32p;
typedef _i64          *_i64p;

#define _SZ(a) (sizeof a)
#define _NE(a) (_SZ(a) / _SZ(*(a))) // number of elements

#define SWP_16(a) \
    ((_u32)(a) << 8 & 0xff00 | \
     (_u32)(a) >> 8 & 0x00ff)

#define SWP_24(a) \
    ((a)[0] << 16 | \
     (a)[1] <<  8 | \
     (a)[2] <<  0)

#define SWP_32(a) \
    ((_u32)(a) << 24 & 0xff000000 | \
     (_u32)(a) <<  8 & 0x00ff0000 | \
     (_u32)(a) >>  8 & 0x0000ff00 | \
     (_u32)(a) >> 24 & 0x000000ff)

#define SWP_64(a) \
    ((_u64)(a) << 56 & 0xff00000000000000 | \
     (_u64)(a) << 40 & 0x00ff000000000000 | \
     (_u64)(a) << 24 & 0x0000ff0000000000 | \
     (_u64)(a) <<  8 & 0x000000ff00000000 | \
     (_u64)(a) >>  8 & 0x00000000ff000000 | \
     (_u64)(a) >> 24 & 0x0000000000ff0000 | \
     (_u64)(a) >> 40 & 0x000000000000ff00 | \
     (_u64)(a) >> 56 & 0x00000000000000ff)

