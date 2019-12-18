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


#ifndef NOTCH_FILTER8_H
#define NOTCH_FILTER8_H

//---------------------------------------------------------------------------------------

// #include "base/filter.h"

#include <vector>
#include <boost/circular_buffer.hpp>

namespace SigViewer_
{

//---------------------------------------------------------------------------------------
class NotchFilter8 // : public Filter
{
  public:
    NotchFilter8(double fs, double freq);

    virtual void filter (double* in, double* out, int in_out_length) const
    {
        filterImpl (in, out, in_out_length);
    }

private:


            template<typename T>
            void filterImpl (T const& in, T& out, int in_out_size) const
            {
                double temp = 0;
                double sum = 0;
                boost::circular_buffer<double> buffer(8);
                for(unsigned int m = 0; m < 8; m++)
                  buffer.push_front(0);

                for(int n = 0; n < in_out_size; n++)
                {
                  sum = 0;
                  for(unsigned int m = 0; m < buffer.size(); m++)
                    sum += buffer[m] * -a_[m +1];

                  temp = in[n] + sum;

                  sum = temp * b_[0];
                  for(unsigned int m = 0; m < buffer.size(); m++)
                    sum += buffer[m] * b_[m +1];

                  out[n] = sum;

                  buffer.push_front(temp);
                }
            }

    static const unsigned int FILTER_ORDER;
    static const double POLE_DISTANCE;
    std::vector<double> a_;
    std::vector<double> b_;
    double d_;

    double fs_;
    double freq_;
};

}

//---------------------------------------------------------------------------------------

#endif // NOTCH_FILTER8_H
