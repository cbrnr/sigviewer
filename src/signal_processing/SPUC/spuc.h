/*! \mainpage 
 * SPUC - Signal processing using C++ - A DSP library
 * <p>Copyright (C) 1993-2005 Tony Kirke.
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

 <H1>Signal Processing using C++ (SPUC)</H1>
 <P><B>Motivation:Why use C++ for DSP Simulation/Modeling?</B>
	 <P>Todays IC and system designers typically use either C or costly 
	 2<SUP>nd</SUP> party tools to simulate/model Digital Signal Processing 
	 algorithms. While the latter are well suited for modeling "hardwired" DSP blocks 
	 with rather simplistic dataflows, they are very inefficient for modeling the control logic that is often required in todays DSP applications (such as ADSL or V.34 modems). 
	 while C is well suited to control logic type structures (if,else, case, etc.), it is not highly desireable for manipulation of complex 
	 or user defined data types (such as fixed width integers).
	 </P>
	 <P><I>C++ bridges this gap and is the underlying language of choice 
	 for the cumbersome GUI based tools.</I></P>
	 <HR>
	 
	 <P><B>Objective</B></P>
	 <P>The objective of SPUC is to provide the Communications Systems Designer or DSP Algorithm designer with simple, efficient and reusable DSP building block objects. Thus allowing a transition from System design to implementation in either programmable DSP chips or hardwired DSP logic. While Matlab is perhaps the most useful available tool for this purpose, it can be quite slow for simulation and it favors a matrix/block based approach rather than the sample by sample simulations that are often most useful for communications systems design. Also Matlab is generally awkward or inefficient when dealing with several interactive feedback loops where C/C++ is perhaps the most useful environment. For bit-accurate simulations (for VLSI design) C/C++ generally outperforms and is easier to manipulate than Matlab or other GUI-based tools.</P>
	 <P>This Class Library</P>
	 <ul> <li> 1) basic building blocks such as complex data types, Fixed-bit width integer classes, pure-delay blocks, etc.
	 <li> 2) Basic DSP building blocks such as FIR, IIR, Allpass, Running Average, Lagrange interpolation filters, NCO, Cordic rotator.
	 <li> 3) Several communications functions such as timing, phase and frequency discriminators for BPSK/QPSK signals.
	 <li> 4) Other miscellaneous DSP/Communications related functions/classes.
	 <li> 5) Ability to design several types of FIR and IIR filters
	 <li> 6) Various adaptive equalizer classes
	 <li> 7) This library now includes code from IT 3.7.0. 
	 Code was modified to work together with SPUC and replace Vector and Matrix classes from TNT.
     <li> 8) Capitalized and uppercase class names are classes not originally in SPUC
	 </ul>


	 The classes are designed so that they can be used in a simple straight forward manner. For example, a FIR would be initialized with its tap weights and then simply a member function would be called every time a sample is input or an output is required.
	 <p>For downloading files go to<p>
	 <P><A HREF="http://sourceforge.net/projects/spuc/"><B>Project Page at Sourceforge</B></A>

	 <P>Before discussing why C++ is better than C for DSP programming. Let's look at some of the pros and cons
	 for C++ vs. the alternatives.

	 <P><B>Pros</B>
	 <ul><li>  The common alternatives are very costly (Alta's SPW, Synopsys Cossap etc.)
	 <li>    Long learning times/training not required for C++ programmers.
	 <li>   For complex simulations, C++ is more efficient and yields shorter run times.
	 <li>   Ideal for modeling a combination of control logic and hardwired DSP blocks.
	 <li>   Code entry can be faster than GUI based tools.
	 <li>  Looks closer to Verilog/VHDL than alternative tools.
	 <li>  C/C++ has a large user base.
	 <li>  Lots of freely available C/C++ code.
	 <li>  Infinitely customizable.
	 <li>  Portable to different platforms.
	 <li>  Can take advantage of Windows GUIs if desired.
	 </ul>

	 <P><B>Cons</B>
	 <ul><li>    No standardization!
	 <li>    Lack of documentation.
	 <li>    Not user friendly/ No GUI.
	 <li>    Potential portability problems.
	 <li>    No technical support.
	 </ul>


	 <BR> Please see links such as those below for why to use C++ over C in general.
	 <BR><A HREF="http://www.cantrip.org/cpp.html">Nathan Myers C++ Articles</A>
	 <BR><A HREF="http://math.nist.gov/pozo/c++class/">C++ Programming for Scientists Course (PDF) from NIST</A>
	 <BR><A HREF="http://math.nist.gov/pozo/">Various C++ Math classes from Roldan Pozo at NIST</A>
	 <P><B>Advantages of C++ over C</B>
	 <UL>
	 <LI>Object Oriented Language</LI>
	 <LI>Abstract Data typing</LI>
	 <LI>Inheritance</LI>
	 <LI>Polymorphism</LI>
	 <LI>Comments do not extend over more than one line</LI>
	 <LI>Variables can be declared anywhere.</LI>
	 <LI>Dynamic memory allocation is simpler.</LI>
	 <LI>Function Overloading</LI>
	 </UL>
	 
	 <P><B>C++ allows templates and operator overloading.</B>
	 <BR>For example, we can define a complex data type based on templates. Then this class can be used whether we need floating point (double), integer, or user defined fixed bit width data types.This not only requires less code to document/debug, but also through operator overloading we can make the code much easier to read and potentially make the look and feel very close to a Hardware Description Language (HDL)such as Verilog or VHDL. Also this style makes it much easier to change your programs data types (from floating point -> integer -> fixed bit width for example), without having to change every line of code.
	 <P><B>C++ allows for much better interfaces between classes or DSP blocks.</B>
	 <BR>Because C++ supports initialization through constructors
	 and there can be a variety of member functions, data can be handled in
	 a much smoother manner than C. With data hiding, the code becomes much
	 simpler to read, allowing a high level look at the code.
	 
	 <P><B>Inheritance</B>
	 <BR>Inheritance helps you to re-use code.You can derive new types or classes from an old one and make changes only where you need them. This promotes code re-use. 
	 <P><B>Classes</B>
	 </center>
	 <UL><LI>Hide complexity.</LI>
	 <LI>Create a more convenient object oriented interfaces</LI>
	 <LI>Can create more than one instance of a structure.</LI>
	 <LI>Similar to C structure but can include functions.</LI>
	 <LI>Data abstraction and hiding allows changes to be localized to one section.</LI>
	 <LI>Private, Protected, Public members</LI></UL>
	 
	 <P><B>For Hardware Modeling</B>
	 <BR>C++ can be made to replicate hardware much easier than C.
	 Functions and variables can be localised to each block (hiding complexity) in the same way that Verilog /VHDL does.
	 Easier to have a hierarchical structure.
	 Classes can be instantiated in same manner as in a HDL (C makes this difficult).
	 New chip designs can inherit subblocks or a large section from an old design. Only the new or changed blocks need to be coded. Also the interfaces to the chip can remain the same if desired. When classes are designed in this manner it is quite easy to see the difference between objects.
	 
	 
*/
//!  SPUC - Signal processing using C++ - A DSP library
//! includes code from other sources including IT++ 3.7.0
//! \namespace SPUC
namespace SPUC {
//! \file
//! \brief Various Defined Constants and Macros
//! \author Tony Kirke,  Copyright(c) 2001 
#ifndef MYINCLUDE
#define MYINCLUDE
#define MAXINT	32767
#define MXLONG 4294967295
#define BIG	72057594037927936.		// Maximum precision of DOUBLE */
#define HUGED	1.701411733192644270e38 	// Largest DOUBLE */
#define LOGHUGE 39				// 10^39 --> Largest power */
#define LN2	0.69314718055994530941
#define ILN2	1.4426950408889634 // 1/LN2
#define LN10	2.30258509299404568401
#define E	2.7182818284590452353602874
#define SQRT2	1.41421356237309504880
#define HALFSQRT2	.70710678118654752440
#ifndef PI
#define PI	3.141592653589793238462643
#endif
//#define pi PI
#define QUARTPI 0.78539816339744830962
#define HALFPI	1.57079632679489661923
#define TWOPI	6.28318530717958647692
#define RADPDEG 0.01745329251994329576
#define RTD  57.295781  // radian to decimal conversion factor 

// macros 

#define SQR(x)	((x) * (x))
#define SGN(x)	((x) < 0 ? -1 : 1)
#define XSWAP(a,b) a ^= b, b ^= a, a ^= b
#define SWAP(a,b,c) c = a, a = b, b = c
#ifndef MAX
#define MAX(x,y)	((x) >= (y) ? (x) : (y))
#endif
#ifndef MIN
#define MIN(x,y)	((x) <= (y) ? (x) : (y))
#endif
#ifndef ABS
#define ABS(x) ( (x < 0) ? (-x) : (x))
#endif
//#define abs(x)		((x)<0? -(x):(x))	// Integer Absolute value */
//#define fabs(x) 	((x)<0.? -(x):(x))	// Floating absolute value */
#define MODULO(x,y)	((x)%(y))		//  Integer modulo */
#define LOGE(x) 	(log(x))		// Natural log */
#define LN(x)		(log(x))		// Natural log */
#define MASK_LOW(n)	((1<<n) - 1)
#define MASK_BIT(n) (1<<n)
#define MASK_NEG_HI(n)	(-1<<(32-n))
#define CLIP(A,B,C) (A>B) ? B : ((A<C) ? C : A)
typedef long natural;
#endif
// End of spuc.h 
}
