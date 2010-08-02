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

#include "sigviewer_user_types.h"

namespace SigViewer_
{

namespace MathUtils_
{

// round to 1, 2 or 5
float64 round125(float64 value);

//-----------------------------------------------------------------------------
/// @return number of decimals needed to display time intervals correctly
int sampleRateToDecimalPrecision (float sample_rate);

}

} // namespace SigViewer_

#endif
