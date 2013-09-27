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


#include "notch_filter8.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <iomanip>

#include <complex>
#include <stdexcept>

namespace SigViewer_
{


//---------------------------------------------------------------------------------------

using std::vector;

const unsigned int NotchFilter8::FILTER_ORDER = 8;
const double NotchFilter8::POLE_DISTANCE = 0.995;

//---------------------------------------------------------------------------------------

NotchFilter8::NotchFilter8(double fs, double freq)
  :a_(FILTER_ORDER +1,0), b_(FILTER_ORDER+1,0), d_(POLE_DISTANCE),
   fs_(fs), freq_(freq)
{
  if(freq >= fs/2)
    throw(std::invalid_argument("Filter freq must be lower than fs/2!"));

  if(!freq)
    throw(std::invalid_argument("Filter freq must not be 0!"));

  if(!fs)
    throw(std::invalid_argument("Fs must not be 0!"));

  double w = (2 * M_PI * freq)/fs;

  std::complex<double> e1(0,w);
  std::complex<double> e2(0,-w);

  std::complex<double> v_comp = std::exp(e1) + std::exp(e2);

  if(v_comp.imag())
    throw(std::runtime_error("Coefficient factor has imaginary part -- something went wrong!"));

  double v( v_comp.real() );


  b_[0] = 1;
  b_[1] = -4* v;
  b_[2] = 4 + 6 * pow(v,2);
  b_[3] = -12 * v -4 * pow(v,3);
  b_[4] = 6 + 12 * pow(v,2) + pow(v,4);
  b_[5] = b_[3];
  b_[6] = b_[2];
  b_[7] = b_[1];
  b_[8] = b_[0];

  a_[0] = b_[0];
  a_[1] = b_[1] * d_;
  a_[2] = b_[2] * pow(d_, 2);
  a_[3] = b_[3] * pow(d_, 3);
  a_[4] = b_[4] * pow(d_, 4);
  a_[5] = b_[5] * pow(d_, 5);
  a_[6] = b_[6] * pow(d_, 6);
  a_[7] = b_[7] * pow(d_, 7);
  a_[8] = b_[8] * pow(d_, 8);
}

//---------------------------------------------------------------------------------------
//template<typename T>
//void NotchFilter8::filter (T& in, T& out, int in_out_size)
//{

//}

//---------------------------------------------------------------------------------------



}














