// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "sigviewer_user_types.h"

namespace sigviewer
{

namespace MathUtils_
{

// round to 1, 2 or 5
float64 round125(float64 value);

//-----------------------------------------------------------------------------
/// @return number of decimals needed to display time intervals correctly
int sampleRateToDecimalPrecision (float64 sample_rate);

}

}

#endif
