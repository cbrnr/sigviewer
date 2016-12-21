// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "math_utils.h"

#include <QDebug>
#include <cmath>

namespace sigviewer
{

namespace MathUtils_
{

//-----------------------------------------------------------------------------
float64 round125(float64 value)
{
    float64 sign = value < 0 ? -1 : 1;
    value = value * sign;
    float64 tmp = pow (10.0, floor (log10(value)));
    if (tmp < value)
        tmp *= 2.0;
    if (tmp < value)
        tmp *= 1.5;
    if (tmp < value)
        tmp *= (4.0 / 3.0);
    if (tmp < value)
        tmp *= (5.0 / 4.0);
    if (tmp < value)
        tmp *= (7.5 / 5.0);
    return sign * tmp;
}

//-----------------------------------------------------------------------------
int sampleRateToDecimalPrecision (float64 sample_rate)
{
    int precision = 0;
    for (; sample_rate > 10; sample_rate /= 10)
        precision++;
    return precision;
}

}

}
