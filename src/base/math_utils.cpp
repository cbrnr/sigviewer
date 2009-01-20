/*

    $Id: math_utils.cpp,v 1.2 2009-01-20 10:00:35 schloegl Exp $
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

// math_utils.cpp

#include "math_utils.h"

#include <math.h>

namespace BioSig_
{

#ifdef OBSOLETE 
	/* this part is obsolete */

// greatest common divisor
uint32 gcd(uint32 a, uint32 b)
{
    uint32 m = a;
    uint32 n = b;
    while (n > 0)
    {
        uint32 z = n;
        n = m % n;
        m = z;
    }
    return m;
}

// least common multiple
uint32 lcm(uint32 a, uint32 b)
{
    return (uint32)((uint64)a * b / gcd(a, b));
}

// float to rational
// tested up to 10000/10000
// should work up to 2^18/2^18
void float2rational(float64 value, uint32& nominator, uint32& denominator)
{
    uint32 fraction[40];
    int32 fraction_size = 0;
    do
    {
        value = 1.0 / value;
        fraction[fraction_size++] = (uint32)value;
        value -= (uint32)value;
    }
    while (value > 1.0 / 0x4000 && fraction_size < 40);

    nominator = 1;
    denominator = fraction[fraction_size - 1];
    for (int32 index = fraction_size - 2; index >= 0; index--)
    {
        uint32 tmp = denominator;
        denominator = denominator * fraction[index] + nominator;
        nominator = tmp;
    }
}
#endif 

// round to 1, 2 or 6
float64 round125(float64 value)
{
    float64 sign = value < 0 ? -1 : 1;
    value = value * sign;
    float64 tmp = pow(10.0, floor(log10(value)));
    if (tmp < value)
    {
        tmp *= 2.0;
    }
    if (tmp < value)
    {
        tmp *= 2.5;
    }
    if (tmp < value)
    {
        tmp *= 2.0;
    }
    return sign * tmp;
}

} // namespace BioSig_
