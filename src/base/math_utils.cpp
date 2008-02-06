// math_utils.cpp

#include "math_utils.h"

#include <math.h>

namespace BioSig_
{

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
