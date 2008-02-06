// user_types.cpp

#include "user_types.h"

#include <stdlib.h>

#ifdef _WIN32
//const char* DIR_SEPARATOR = "\\";
const char* DIR_SEPARATOR = "/";
const char* HOME_DIRECTORY = getenv("USERPROFILE");
#else
const char* DIR_SEPARATOR = "/";
const char* HOME_DIRECTORY = getenv("HOME");
#endif

#ifdef _BIG_ENDIAN

// change byte order int8
void swapBytes(int8 &)
{
    // nothing
}

// change byte order uint8
void swapBytes(uint8 &)
{
    // nothing
}

// change byte order int16
void swapBytes(int16 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = tmp;
}

// change byte order uint16
void swapBytes(uint16 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = tmp;
}

// change byte order int32
void swapBytes(int32 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}

// change byte order uint32
void swapBytes(uint32 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}

// change byte order int64
void swapBytes(int64 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[7];
    bytes[7] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[6];
    bytes[6] = tmp;
    tmp = bytes[2];
    bytes[2] = bytes[5];
    bytes[5] = tmp;
    tmp = bytes[3];
    bytes[3] = bytes[4];
    bytes[4] = tmp;
}

// change byte order uint64
void swapBytes(uint64 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[7];
    bytes[7] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[6];
    bytes[6] = tmp;
    tmp = bytes[2];
    bytes[2] = bytes[5];
    bytes[5] = tmp;
    tmp = bytes[3];
    bytes[3] = bytes[4];
    bytes[4] = tmp;
}

// change byte order float32
void swapBytes(float32 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = tmp;
}

// change byte order float64
void swapBytes(float64 &value)
{
    char tmp;
    char* bytes = (char*)&value;
    tmp = bytes[0];
    bytes[0] = bytes[7];
    bytes[7] = tmp;
    tmp = bytes[1];
    bytes[1] = bytes[6];
    bytes[6] = tmp;
    tmp = bytes[2];
    bytes[2] = bytes[5];
    bytes[5] = tmp;
    tmp = bytes[3];
    bytes[3] = bytes[4];
    bytes[4] = tmp;
}

#endif // _BIG_ENDIAN
