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
