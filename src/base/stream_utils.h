// stream_utils.h

#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

#include "user_types.h"

#include <QIODevice>

namespace BioSig_
{

// read value from stream
template<typename DestType>
inline uint32 readStreamValue(DestType& dest, QIODevice& src_device)
{
    src_device.read((char*)&dest, sizeof(DestType));
#ifdef _BIG_ENDIAN
    swapBytes(dest);
#endif
    return sizeof(DestType);
}

// read values from stream
template<typename DestType>
inline uint32 readStreamValues(DestType* dest, QIODevice& src_device,
                               uint32 dest_size)
{
    src_device.read((char*)dest, dest_size);
#ifdef _BIG_ENDIAN
    DestType* last_dest = &dest[dest_size / sizeof(DestType) - 1];
    while (dest <= last_dest)
    {
        swapBytes(*(dest++));
    }
#endif
    return sizeof(DestType) * dest_size;
}

// read chars from stream
inline uint32 readStreamChars(char* dest, QIODevice& src_device,
                               uint32 length)
{
    src_device.read(dest, length);
    uint32 no_whitespace = length - 2;
    while (dest[no_whitespace] == ' ' || dest[no_whitespace] == '\t')
    {
        no_whitespace--;
    }
    dest[no_whitespace + 1] = '\0';
    return length;  
}

// write value to stream
template<typename SrcType>
inline uint32 writeStreamValue(QIODevice& dest_device, SrcType src)
{
#ifdef _BIG_ENDIAN
    swapBytes(src);
#endif
    dest_device.write((char*)&src, sizeof(SrcType));
    return sizeof(SrcType);
}

// write values to stream
template<typename SrcType>
inline uint32 writeStreamValues(QIODevice& dest_device, SrcType* src,
                                uint32 src_size)
{
#ifdef _BIG_ENDIAN
    SrcType* last_src = &src[src_size / sizeof(SrcType) - 1];
    while (src <= last_src)
    {
        SrcType tmp = *(src++);
        swapBytes(tmp);
        dest_device.write((char*)tmp, sizeof(SrcType));
    }
#else
    dest_device.write((char*)src, src_size);
#endif
    return sizeof(SrcType) * src_size;
}

// write chars to stream
inline uint32 writeStreamChars(QIODevice& dest_device, char* src,
                               uint32 length)
{
    uint32 done = 0;
    while(done < length && *src != '\0')
    {
        dest_device.write(src++, 1);
        done++;
    }
    while(done < length)
    {
        dest_device.write(" ", 1);
        done++;
    }
    return length;
}

} // namespace BioSig_

#endif
