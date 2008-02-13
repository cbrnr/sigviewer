// math_utils.h

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "user_types.h"
#include "signal_channel.h"

namespace BioSig_
{

// greatest common divisor
uint32 gcd(uint32 a, uint32 b);

// least common multiple 
uint32 lcm(uint32 a, uint32 b);

// float to rational
void float2rational(float64 value, uint32& nominator, uint32& denominator);

// round to 1, 2 or 5
float64 round125(float64 value);

// convert data
template<typename DestType>
inline void convertData(int8* src, DestType* dest, SignalChannel& sig,
                        uint32 samples, bool do_scale = true)
{
    DestType scale = do_scale ? sig.getScale() : 1;
    DestType offset = do_scale ? sig.getOffset() : 1;
    uint8* buffer = (uint8*)src + sig.getDataOffset();
    if (sig.getDataType() > SignalChannel::UBITN)
    {
        // TODO : signal record-size no multiple of 8 bits
        uint32 bits = sig.getDataType() - SignalChannel::UBITN;
        copyInterpolatedUNBitArray(dest, buffer, bits, scale, offset, samples);
    }
    else if (sig.getDataType() > SignalChannel::BITN)
    {
        // TODO : signal record-size no multiple of 8 bits
        uint32 bits = sig.getDataType() - SignalChannel::UBITN;
        copyInterpolatedSNBitArray(dest, buffer, bits, scale, offset, samples);
    }
    else switch(sig.getDataType())
    {
        case SignalChannel::INT8:
            copyInterpolatedArray(dest, (int8*)buffer, scale, offset, samples);
            break;
        case SignalChannel::UINT8:
            copyInterpolatedArray(dest, (uint8*)buffer, scale, offset, samples);
            break;
        case SignalChannel::INT16:
            copyInterpolatedArray(dest, (int16*)buffer, scale, offset, samples);
            break;  
        case SignalChannel::UINT16:
            copyInterpolatedArray(dest, (uint16*)buffer, scale, offset,samples);
            break;  
        case SignalChannel::INT32:
            copyInterpolatedArray(dest, (int32*)buffer, scale, offset, samples);
            break;  
        case SignalChannel::UINT32:
            copyInterpolatedArray(dest, (uint32*)buffer, scale, offset,samples);
            break;  
        case SignalChannel::INT64:
            copyInterpolatedArray(dest, (int64*)buffer, scale, offset, samples);
            break;  
        case SignalChannel::FLOAT32:
            copyInterpolatedArray(dest, (float32*)buffer, scale,offset,samples);
            break;  
        case SignalChannel::FLOAT64:
            copyInterpolatedArray(dest, (float64*)buffer, scale,offset,samples);
            break;
    }
}

// compare floats
template<typename FloatType>
inline bool floatEqual(FloatType a, FloatType b)
{
    FloatType abs_sum = a > 0.0 ? a : -a;
    abs_sum += b > 0.0 ? b : -b;
    if (abs_sum == 0.0)
    {
        return true;
    }
    return (a - b) * (a - b) / (abs_sum * abs_sum) < 1E-30;
}

// copy interpolated array values
template<typename DestType, typename SrcType>
inline void copyInterpolatedArray(DestType* dest, SrcType* src,
                                  DestType scale, DestType offset,
                                  uint32 array_size)
{
    DestType* last_dest = &dest[array_size - 1];
    while(dest <= last_dest)
    {
        *(dest++) = *(src++) * scale + offset;
    }
}

// copy array values
template<typename DestType, typename SrcType>
inline void copyArray(DestType* dest, SrcType* src, uint32 array_size)
{
    DestType* last_dest = &dest[array_size - 1];
    while(dest <= last_dest)
    {
        *(dest++) = (DestType)*(src++);
    }
}

// copy interpolared unsigned N-bit array values
// bits < 64!!
template<typename DestType>
inline void copyInterpolatedUNBitArray(DestType* dest, uint8* src, uint32 bits,
                                      DestType scale, DestType offset,
                                      uint32 array_size)
{
    uint64 queue = 0;
    uint8 bits_in_queue = 0;
    DestType* last_dest = &dest[array_size - 1];
    while(dest <= last_dest)
    {
        while (bits_in_queue < bits)
        {
            queue = (queue << 8) | *(src++);
            bits_in_queue += 8;
        }
        uint64 value = queue >> (bits_in_queue - bits);
        queue -= value << (bits_in_queue - bits);
        bits_in_queue -= bits;
        *(dest++) = value * scale + offset;
    }
}

// copy interpolared signed N-bit array values
// bits < 64!!
template<typename DestType>
inline void copyInterpolatedSNBitArray(DestType* dest, uint8* src, uint32 bits,
                                       DestType scale, DestType offset,
                                       uint32 array_size)
{
    uint64 sign_mask = 1 << (bits - 1);
    uint64 queue = 0;
    uint8 bits_in_queue = 0;
    DestType* last_dest = &dest[array_size - 1];
    while(dest <= last_dest)
    {   
        while (bits_in_queue < bits)
        {
            queue = (queue << 8) | *(src++);
            bits_in_queue += 8;
        }
        int64 value = queue >> (bits_in_queue - bits);
        queue -= value << (bits_in_queue - bits);
        bits_in_queue -= bits;
        if (value & sign_mask)
        {
            value = (value - sign_mask) | 0x8000000000000000LL;
        }
        *(dest++) = value * scale + offset;
    }
}

} // namespace BioSig_

#endif
