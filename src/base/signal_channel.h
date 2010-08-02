/*

    $Id: signal_channel.h,v 1.10 2009-03-10 09:38:23 schloegl Exp $
    Copyright (C) Thomas Brunner  2006,2007 
    Copyright (C) Christoph Eibel 2007,2008, 
    Copyright (C) Clemens Brunner 2006,2007,2008  
    Copyright (C) Alois Schloegl  2008,2009
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

// signal_channel.h

#ifndef SIGNAL_CHANNEL_H
#define SIGNAL_CHANNEL_H

#include "sigviewer_user_types.h"

#include <QString>
#include <QMutex>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// @class SignalChannel
/// @brief data about a signal channel
class SignalChannel
{
public:
    //-------------------------------------------------------------------------
    SignalChannel (unsigned number,
                   QString const& label,
                   QString const& phys_y_dimension_label = "");

    //-------------------------------------------------------------------------
    QString typeString() const;
    uint32 getNumber() const;
    const QString& getLabel() const;
    float64 getLowpass() const;
    float64 getHighpass() const;
    bool getNotch() const;
    const QString& getPhysicalDim() const;
    float64 getPhysicalMaximum() const;
    float64 getDigitalMaximum() const;
    float64 getPhysicalMinimum() const;
    float64 getDigitalMinimum() const;

private:
    // from GDF format
    enum Type
    {
        CHAR    = 0,
        INT8    = 1,
        UINT8   = 2,
        INT16   = 3,
        UINT16  = 4,
        INT32   = 5,
        UINT32  = 6,
        INT64   = 7,
        UINT64   = 8,
        FLOAT32 = 16,
        FLOAT64 = 17,
        FLOAT128 = 18,

        // add N to code
        BITN    = 255,
        UBITN   = 511
    };

    mutable QMutex mutex_;
    
    uint32 number_;
    QString label_;
    QString phys_y_dimension_label_;
    uint16  physical_dimcode_;
    float64 physical_maximum_;
    float64 digital_maximum_;
    float64 physical_minimum_;
    float64 digital_minimum_;
    uint16_t data_type_;
    float64 lowpass_;
    float64 highpass_;
    bool notch_;
};

} // namespace SigViewer_

#endif
