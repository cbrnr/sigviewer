// Copyright 2013 Clemens Brunner, Thomas Brunner, Christoph Eibel,
// Alois Schlögl, Oliver Terbu.

// This file is part of SigViewer.
//
// SigViewer is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// SigViewer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SigViewer. If not, see <http://www.gnu.org/licenses/>.


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

} // namespace SigViewer_
