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

#include <QDebug>
#include <cmath>

namespace SigViewer_
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
int sampleRateToDecimalPrecision (float sample_rate)
{
    int precision = 0;
    for (; sample_rate > 10; sample_rate /= 10)
        precision++;
    return precision;
}

}

} // namespace SigViewer_
