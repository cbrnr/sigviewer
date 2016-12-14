#ifndef SIGVIEWER_USER_TYPES_H
#define SIGVIEWER_USER_TYPES_H

#include <stddef.h>

namespace SigViewer_
{

enum ScaleMode
{
        MAX_TO_MAX,
        MIN_TO_MAX
};

// standard types
// compiler specific types
#ifdef _MSC_VER
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif

}

#ifdef __GNUG__
#include <inttypes.h>
#endif

namespace SigViewer_
{

typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef float 	 float32;
typedef double   float64;
typedef int64_t  int64;
typedef uint64_t uint64;

typedef int32 EventID;
typedef uint16 EventType;
typedef int ChannelID;

const ChannelID UNDEFINED_CHANNEL = -1;
const EventID UNDEFINED_EVENT_ID = -1;
const EventType UNDEFINED_EVENT_TYPE = -1;

}

#endif // SIGVIEWER_USER_TYPES_H
