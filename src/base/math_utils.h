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
int sampleRateToDecimalPrecision (float64 sample_rate);

}

} // namespace SigViewer_

#endif
