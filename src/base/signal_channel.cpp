/*

    $Id: signal_channel.cpp,v 1.9 2009-02-22 12:36:46 cle1109 Exp $
    Copyright (C) Thomas Brunner  2006,2007 
    		  Christoph Eibel 2007,2008, 
		  Clemens Brunner 2006,2007,2008  
    		  Alois Schloegl  2008
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

// signal_channel.cpp

#include "signal_channel.h"

#include <QMutexLocker>

namespace BioSig_
{

// constructor
SignalChannel::SignalChannel(uint32 number, const QString& label,
                             uint32 samples_per_record,
                             const QString& physical_dim,
                             uint32 physdimcode, 
                             float64 physical_minimum,
                             float64 physical_maximum,
                             float64 digital_minimum, float64 digital_maximum,
                             Type data_type, uint32 data_offset,
                             const QString filter_label, float64 lowpass,
                             float64 highpass, bool notch)
: number_(number),
  label_(label),
  samples_per_record_(samples_per_record),
  physical_dim_(physical_dim),
  physical_maximum_(physical_maximum),
  digital_maximum_(digital_maximum),
  physical_minimum_(physical_minimum),
  digital_minimum_(digital_minimum),
  data_type_(data_type),
  data_offset_(data_offset),
  filter_label_(filter_label),
  lowpass_(lowpass),
  highpass_(highpass),
  notch_(notch)
{
    scale_ = (physical_maximum - physical_minimum_) /
             (digital_maximum - digital_minimum);
    offset_ = physical_minimum - digital_minimum * scale_;
}

// get number
uint32 SignalChannel::getNumber() const
{
    return number_;
}

// get label
const QString& SignalChannel::getLabel() const
{
    return label_;
}

// get samples per record
uint32 SignalChannel::getSamplesPerRecord() const
{
    return samples_per_record_;
}

// get scale
float64 SignalChannel::getScale() const
{
    return scale_;
}

// get offset
float64 SignalChannel::getOffset() const
{
    return offset_;
}

// get filter label
const QString& SignalChannel::getFilterLabel() const
{
    return filter_label_;
}

// get lowpass
float64 SignalChannel::getLowpass() const
{
    return lowpass_;
}

// get highpass
float64 SignalChannel::getHighpass() const
{
    return highpass_;
}

// get notch
bool SignalChannel::getNotch() const
{
    return notch_;
}

// get physical dim
const QString& SignalChannel::getPhysicalDim() const
{
    return physical_dim_;
}

// get physical max
float64 SignalChannel::getPhysicalMaximum() const
{
    return physical_maximum_;
}

// get digital maximum
float64 SignalChannel::getDigitalMaximum() const
{
    return digital_maximum_;
}

// get physical minumum
float64 SignalChannel::getPhysicalMinimum() const
{
    return physical_minimum_;
}

// get digital minumum
float64 SignalChannel::getDigitalMinimum() const
{
    return digital_minimum_;
}

// get data type
uint32 SignalChannel::getDataType() const
{
    return data_type_;
}

// get data offset
uint32 SignalChannel::getDataOffset() const
{
    return data_offset_;
}

// size of data_type in bits
uint32 SignalChannel::typeBitSize() const
{
    if (data_type_ > UBITN)
    {
        return (data_type_ - UBITN);
    }
    if (data_type_ > BITN)
    {
        return (data_type_ - BITN);
    }
    switch (data_type_)
    {
        case CHAR:
        case INT8:
        case UINT8:
            return 8;
        case INT16:
        case UINT16:
            return 16;
        case INT32:
        case UINT32:
        case FLOAT32:
            return 32;
        case INT64:
        case FLOAT64:
            return 64;
        default:
            return 0;
    }
    return 0;
}

// type string
QString SignalChannel::typeString() const
{
    if (data_type_ > UBITN)
    {
        return QString("ubit%1").arg(data_type_ - UBITN);
    }
    if (data_type_ > BITN)
    {
        return QString("bit%1").arg(data_type_ - BITN);
    }
    switch (data_type_)
    {
        case CHAR:
            return "char";
        case INT8:
            return "int8";
        case UINT8:
            return "uint8";
        case INT16:
            return "int16";
        case UINT16:
            return "uint16";
        case INT32:
            return "int32";
        case UINT32:
            return "uint32";
        case FLOAT32:
            return "float32";
        case INT64:
            return "int64";
        case FLOAT64:
            return "float64";
        default:
            return "?";
    }
    return "?";
}

} // namespace BioSig_
