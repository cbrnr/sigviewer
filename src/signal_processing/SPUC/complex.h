// 
// author="Tony Kirke" *
// Copyright(c) 1993-1996 Tony Kirke
/*
 * SPUC - Signal processing using C++ - A DSP library
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#ifndef CMPLX_D
#define CMPLX_D
#include <cstdlib>
#include "spuc.h"
#include <cmath>
namespace SPUC {
//! \brief Spuc template complex class.
//!	Basically the same as STL complex class but allows easier customization.
template <class T> class complex
{
  public:
  T re;
  T im;

  complex()
  {
    re = 0;
    im = 0;
  }
  void*  operator new (size_t) {
	  T* re = new T;
	  T* im = new T;
  }
  complex(T r, T i) :re(r), im(i) {}
  complex(T r) : re(r), im(0) {}
//  template <> complex(const complex<long>& y) : re(y.re), im(y.im) {;}

  inline T real() { return(re);};
  inline T imag() { return(im);};

  friend T real( complex<T> y ) { return(y.re);}
  friend T imag( complex<T> y ) { return(y.im);}

  inline T operator=(T r)
  {
    re = r;
    im = 0;
    return r;
  }

  inline complex  operator =(const complex<T>& y)   {   
	  re = (T)y.re; im = (T)y.im; return *this; 
  } 
  inline complex  operator *=(const complex<T>& y) {
	  T r = re*y.re - im*y.im;
	  im  = re*y.im + im*y.re;
	  re = r;
	  return *this;
  }
  inline complex  operator +=(const complex<T>& y) {
	  re += y.re;
	  im += y.im;
	  return *this;
  }
  inline complex  operator -=(const complex<T>& y) {
	  re -= y.re;
	  im -= y.im;
	  return *this;
  }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  operator const complex<long> () const {	  return(complex<long>((long)re,(long)im));  }
#endif
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <class T1, class T2> inline complex<T1> operator *(complex<T1> r, complex<T2> l)
{
  complex<T1> x;
  x.re = ((r.re*l.re) - (r.im*l.im));
  x.im = (r.re*l.im + r.im*l.re);
  return x;  
  };
//!
template <class T1, class T2> inline complex<T1> operator +(complex<T1> r, complex<T2> l)
{
  complex<T1> x;
  x.re = r.re + l.re;
  x.im = r.im + l.im;
  return x;  
};
//!
template <class T> inline complex<T> operator +(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re + l;
  x.im = r.im;
  return x;  
};
//!
template <class T> inline complex<T> operator +(T r, complex<T> l)
{
  complex<T> x;
  x.re = r + l.re;
  x.im = l.im;
  return x;  
};

//!
template <class T1, class T2> inline complex<T1> operator -(complex<T1> r, complex<T2> l)
{
  complex<T1> x;
  x.re = r.re - l.re;
  x.im = r.im - l.im;
  return x;  
};
//!
template <class T> inline complex<T> operator -(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re - l;
  x.im = r.im;
  return x;  
};
//!
template <class T> inline complex<T> operator -(T r, complex<T> l)
{
  complex<T> x;
  x.re = r - l.re;
  x.im = -l.im;
  return x;  
};
//!
template <class T> inline complex<T> operator &(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re & l;
  x.im = r.im & l;
  return x;  
};
//!
template <class T> inline complex<T> operator &(T r, complex<T> l)
{
  complex<T> x;
  x.re = r & l.re;
  x.im = r & l.im;
  return x;  
};
template <class T> inline complex<T> operator %(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re % l;
  x.im = r.im % l;
  return x;  
};
//!
template <class T> inline complex<T> operator ^(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re ^ l;
  x.im = r.im ^ l;
  return x;  
};
//!
template <class T> inline complex<T> operator ^(T r, complex<T> l)
{
  complex<T> x;
  x.re = r ^ l.re;
  x.im = r ^ l.im;
  return x;  
};
template <class T> inline complex<T> operator |(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re | l;
  x.im = r.im | l;
  return x;  
};
//!
template <class T> inline complex<T> operator |(T r, complex<T> l)
{
  complex<T> x;
  x.re = r | l.re;
  x.im = r | l.im;
  return x;  
};

//! Left shift
template <class T> inline complex<T>  operator <<(complex<T> r, const long shift) 
{
	  complex<long> res = complex<long>(r.re << shift, r.im << shift);
	  return(res);
}
//! Right shift
template <class T> inline complex<T>  operator >>(complex<T> r, const long shift) 
{
	  complex<long> res = complex<long>(r.re >> shift, r.im >> shift);
	  return(res);
}
//!
template <class T> inline complex<T> operator -(complex<T> r)
{
  complex<T> x;
  x.re = -r.re;
  x.im = -r.im;
  return x;  
};




//!
inline complex<long> operator *( double r, complex<long> l)
{
	return(complex<long>((long)floor((double)(r*l.re+0.5)),
						 (long)floor((double)(r*l.im+0.5))));

};
//!
inline complex<double> operator *( long r, complex<double> l)
{
  complex<double> x;
  x.re = (r*l.re);
  x.im = (r*l.im);
  return x;   
};
//!
inline complex<double> operator *( double r, complex<double> l)
{
  complex<double> x;
  x.re = (r*l.re);
  x.im = (r*l.im);
  return x;   
};

//!
template <class T> inline complex<T> operator *(complex<T> r, T l)
{
  complex<T> x;
  x.re = r.re*l;
  x.im = r.im*l;
  return x;  
};

//!
template <class T> complex<T> operator /(complex<T> l, T r)
{
  complex<T> x(0,0);
  if (r != 0)
  {
    x.re = l.re/r;
    x.im = l.im/r;
  }
  //! else is an error condition!
  return x;  
};

//!
template <class T1, class T2> complex<T1> operator /(complex<T1> r, complex<T2> l)
{
  complex<T1> x;
  T2 den;
  den = magsq(l);
  x = (r * conj(l))/den;
  return x;  
};

//!
template <class T1, class T2> inline bool operator ==(complex<T1> r, complex<T2> l)
{
  return ((r.re == l.re) && (r.im == l.im));  
};

//!
template <class T1, class T2> inline bool operator !=(complex<T1> r, complex<T2> l)
{
  return ((r.re != l.re) || (r.im != l.im));  
};

//! Unit magnitude polar to rectangular conversion
inline complex<double> expj(double x)
{
  complex<double> y;
  y.re = cos(x);
  y.im = sin(x);
  return y;
}
//! Polar to rectangular conversion
inline complex<double> polar(double amp,double arg)
{
  complex<double> y;
  y.re = amp*cos(arg);
  y.im = amp*sin(arg);
  return y;
}

//! Complex value (0,1) 
template <class T> inline complex<T> complexj(void)
{
  return(complex<T>(0,1));
}

template <class T> inline T re(complex<T> x)
{
  T y;
  y = x.re;
  return y;
};

template <class T> inline T im(complex<T> x)
{
  T y;
  y = x.im;
  return y;
};

//! Conjugate
template <class T> inline complex<T> conj(complex<T> x)
{
  complex<T> y;
  y.re = x.re;
  y.im = -x.im;
  return y;
}
//! Magnitude Squared of complex vector
template <class T> inline T magsq(complex<T> x)
{
  T y;
  y = (x.re*x.re + x.im*x.im);
  return y;
};
//! Normalized vector (magnitude = 1)
template <class T> inline complex<double> norm(complex<T> x)
{
  T y;
  y = ::sqrt(x.re*x.re + x.im*x.im);
  return (complex<double>(x.re/y,x.im/y));
};


template <class T> inline T approx_mag(complex<T> x)
{
  return(MAX(abs(x.re),abs(x.im)) + MIN(abs(x.re),abs(x.im))/4);
};

template <class T> inline complex<T> maxvalue(complex<T> x1)
{
  return(complex<T>(MAX(x1.re,x1.im)));
};

template <class T> inline complex<T> minimum(complex<T> x1, complex<T> x2)
{
  return(complex<T>(MIN(x1.re,x2.re),MIN(x1.im,x2.im)));
}
template <class T> inline complex<T> maximum(complex<T> x1, complex<T> x2)
{
  return(complex<T>(MAX(x1.re,x2.re),MAX(x1.im,x2.im)));
};
//! Return phase angle (radians) of complex number
template <class T> inline double arg(const complex<T> x)
{
  double TMPANG;
  if (real(x) == 0) {
     if (imag(x) < 0) return(3.0*PI/2.0);
     else return(PI/2.0);
  } else {
        TMPANG=atan((double)imag(x)/(double)real(x));
        if (real(x) < 0) TMPANG -= PI;
        if (TMPANG < 0) TMPANG += TWOPI;
  }
  return(TMPANG);
}
//! Convert to complex<double>
template <class T> inline complex<double> rational(complex<T> l) {
  return(complex<double>((double)l.re,(double)l.im));
}

//! Round by bits, to near integer - return(complex<long>)
inline complex<long> round(complex<long> in, long bits)
{
  double scale = 1.0/(double)(1 << bits);
  return(complex<long>((long)floor((double)(scale*in.re)+0.5),
		     (long)floor((double)(scale*in.im)+0.5)));
}
//! Saturate to specific number of bits (signed)
inline complex<long> saturate(complex<long> in, long bits)
{
  complex<long> out;
  long low_mask = ((1<<(bits-1)) - 1);
  if (labs(in.re) > low_mask) out.re = (in.re>0) ? low_mask : ~low_mask;
  else out.re = in.re;
  if (labs(in.im) > low_mask) out.im = (in.im>0) ? low_mask : ~low_mask;
  else out.im = in.im;
  return(out);
}
template <class T> complex<T> signbit(complex<T> in)
{
  return(complex<T>(SGN(in.re),SGN(in.im)));
}

//template <class T> T real(complex<T>& y) {  return(y.re);}
//template <class T> T imag(complex<T>& y) {  return(y.im);}

template <class T> inline complex<double> reciprocal(complex<T> x)
{
  T y;
  y = (x.re*x.re + x.im*x.im);
  return (complex<double>(x.re/y,-x.im/y));
};
//! squaring function
template <class T> inline complex<T> sqr(complex<T> x)
{
  return (x*x);
};
//! square root function (same as sqrt in spuc_math.h)
template <class T> inline complex<double> csqrt(complex<T> x)
{
  double mag = sqrt(sqrt(magsq(x)));
  double ang = 0.5*arg(x); // ambiguity
  return(polar(mag,ang));
};
//! complex exponential
inline complex<double> exp(complex<double> x)
  { 
	  return (::exp(x.real()) * expj(x.imag()));
  }
//! same name as math library
inline double hypot(complex<double> z) { 
	double _hypot(double x, double y);
	return _hypot(z.imag(), z.real()); }
#endif
} // namespace SPUC 
#endif
