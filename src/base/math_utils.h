/*

    $Id: math_utils.h,v 1.3 2009-01-20 10:00:35 schloegl Exp $
    Copyright (C) Thomas Brunner  2005,2006
		  Clemens Brunner 2005
    		  Alois Schloegl  2009
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    SigViewer is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/



// math_utils.h

#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "user_types.h"

namespace BioSig_
{

// round to 1, 2 or 5
float64 round125(float64 value);


#ifdef OBSOLETE 
	/* this part is obsolete */

// greatest common divisor
uint32 gcd(uint32 a, uint32 b);

// least common multiple 
uint32 lcm(uint32 a, uint32 b);

// float to rational
void float2rational(float64 value, uint32& nominator, uint32& denominator);

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
#endif // OBSOLETE


} // namespace BioSig_

#endif
