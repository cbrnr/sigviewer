/*

    $Id: signal_channel.cpp,v 1.11 2009-03-10 09:46:31 schloegl Exp $
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

// signal_channel.cpp

#include "signal_channel.h"

#include <QMutexLocker>

namespace BioSig_
{

// constructor
SignalChannel::SignalChannel(uint32 number, 
                             uint32 samples_per_record,
                             CHANNEL_STRUCT C)
: number_(number),
  label_(QString(C.Label)),
  samples_per_record_(samples_per_record),
  physical_maximum_(C.PhysMax),
  digital_maximum_(C.DigMax),
  physical_minimum_(C.PhysMin),
  digital_minimum_(C.DigMin),
  data_type_(C.GDFTYP),
  data_offset_(1/8),
  filter_label_("filter"),
  lowpass_(C.LowPass),
  highpass_(C.HighPass),
  notch_(C.Notch)
{
    scale_ = (C.PhysMax - C.PhysMin) /
             (C.DigMax - C.DigMin);
    offset_ = C.PhysMin - C.DigMin * scale_;
    
    char p[MAX_LENGTH_PHYSDIM+1];
    PhysDim(C.PhysDimCode,p);	
    physical_dim_ = QString(p);

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
uint16 SignalChannel::getDataType() const
{
    return data_type_;
}

// get data offset
uint32 SignalChannel::getDataOffset() const
{
    return data_offset_;
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
        case INT64:
            return "int64";
        case UINT64:
            return "uint64";
        case FLOAT32:
            return "float32";
        case FLOAT64:
            return "float64";
        case FLOAT128:
            return "float128";
        default:
            return "?";
    }
    return "?";
}

} // namespace BioSig_
