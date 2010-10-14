// 
// Copyright(c) 1993-1996 Tony Kirke
// author="Tony Kirke" *
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
#ifndef IIR_2ND
#define IIR_2ND
namespace SPUC {
/*! 
  \addtogroup iir IIR filters
*/
//! \brief Template for 2nd Order IIR filter<p>
//! \ingroup iir
//!
//!  Template for 2nd Order IIR filter<p>
//!   The filter is assumed to be a low pass 2nd order digital filter 
//!   of the form \f$ G(z) =  \frac{b0*z^2 + b1*z+b2}{(z^2+a1*z+a2)} \f$
//! \image html iir_2nd.gif
//! \image latex iir_2nd.eps
template <class Numeric> class iir_2nd
{
    protected:   
    	Numeric b0,b1,b2;                    
    	Numeric a1,a2;
        Numeric in[3];
        Numeric out[3]; 
        
    public:
        iir_2nd(Numeric B0, Numeric B1, Numeric B2, Numeric A1, Numeric A2) : b0(B0), b1(B1), b2(B2), a1(A1), a2(A2) {
        	in[0] = in[1] = in[2] = out[2] = out[1] = out[0] = 0 ; }
        iir_2nd(Numeric A1=0, Numeric A2=0) : b0(1), b1(2), b2(1), a1(A1), a2(A2) {
        	in[0] = in[1] = in[2] = out[2] = out[1] = out[0] = 0 ; } 
        void reset() {
        	in[0] = in[1] = in[2] = out[2] = out[1] = out[0] = 0 ; } 
		void set_a(Numeric A1, Numeric A2) { a1=A1; a2=A2;}
		void set_b(Numeric A1, Numeric A2) { b1=A1; b2=A2;}
		void set_coeff(Numeric A1, Numeric A2) { a1=A1; a2=A2;}

//! Constructor reading coefficients from a file.
iir_2nd(const char* file)
{
	FILE *iirf = fopen(file,"r"); 
	fscanf(iirf,"%lf %lf %lf %lf %lf",&b0,&b1,&b2,&a1,&a2);
	fclose(iirf);
   	in[0] = in[1] = in[2] = out[2] = out[1] = out[0] = 0;
}             
//! Print out coefficients
void print() {
    printf("IIR Coefficients B0 = %lf, B1 =  %lf, B2 = %lf",b0,b1,b2);
    printf(" A0 = 1, A1 = %lf, A2 = %lf\n",a1,a2);
}
//! Input new sample and calculate output
Numeric clock(Numeric input) {
	// Shift inputs by one time sample and place new sample into array
	in[0] = in[1];
	in[1] = in[2];
	in[2] = input;
	// Shift previous outputs and calculate new output */
	out[0] = out[1];
	out[1] = out[2];
	out[2] = b0*in[2] + b1*in[1] + b2*in[0] - a1*out[1] - a2*out[0];
	return(out[2]);
}
};                                               
} // namespace SPUC 
#endif
