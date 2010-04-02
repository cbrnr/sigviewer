/*

    $Id: user_types.h,v 1.3 2009-02-10 13:19:57 schloegl Exp $
    Copyright (C) Thomas Brunner  2005,2006
		  Clemens Brunner 2006
    		  Alois Schloegl  2009
    		  
    This file is part of the "SigViewer" repository
    at http://biosig.sf.net/

    This program is free software; you can redistribute it and/or
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


// user_types.h

#ifndef USER_TYPES_H
#define USER_TYPES_H

// von settings dialog ausgelagert!
enum ScaleMode
{
        MAX_TO_MAX,
        MIN_TO_MAX
};

// operating system dependent
extern const char* DIR_SEPARATOR;
extern const char* HOME_DIRECTORY;

// standard types
// compiler specific types
#ifdef _MSC_VER
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif

#ifdef __GNUG__
#include <inttypes.h>
#endif

typedef int8_t   int8;
typedef uint8_t  uint8;
typedef int16_t  int16;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef float 	 float32;
typedef double   float64;
typedef int64_t  int64;
typedef uint64_t uint64;

typedef int32 EventID;
typedef uint16 EventType;
typedef int32 ChannelID;

#endif
