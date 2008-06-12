/*

    $Id: signal_channel.h,v 1.6 2008-06-12 19:18:47 schloegl Exp $
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

// signal_channel.h

#ifndef SIGNAL_CHANNEL_H
#define SIGNAL_CHANNEL_H

#include "user_types.h"

#include <QString>
#include <QMutex>

namespace BioSig_
{

// SignalChannel
class SignalChannel
{
public:

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
        FLOAT32 = 16,
        FLOAT64 = 17,

        // add N to code
        BITN    = 255,
        UBITN   = 511
    };

    SignalChannel(uint32 number, const QString& label,
                  uint32 samples_per_record, const QString& physical_dim,
                  uint32 physdimcode, 
                  float64 physical_minimum, float64 physical_maximum,
                  float64 digital_minimum, float64 digital_maximum,
                  Type data_type, uint32 data_offset,
                  const QString filter_label, float64 lowpass,
                  float64 highpass, bool notch);

    uint32 typeBitSize() const;
    QString typeString() const;
    uint32 getNumber() const;
    const QString& getLabel() const;
    uint32 getSamplesPerRecord() const;
    float64 getScale() const;
    float64 getOffset() const;
    const QString& getFilterLabel() const;
    float64 getLowpass() const;
    float64 getHighpass() const;
    bool getNotch() const;
    const QString& getPhysicalDim() const;
    float64 getPhysicalMaximum() const;
    float64 getDigitalMaximum() const;
    float64 getPhysicalMinimum() const;
    float64 getDigitalMinimum() const;
    uint32 getDataType() const;
    uint32 getDataOffset() const;

private:
    // not allowed
    SignalChannel();

    mutable QMutex mutex_;
    
    uint32 number_;
    QString label_;
    uint32 samples_per_record_;
    QString physical_dim_;
    float64 physical_maximum_;
    float64 digital_maximum_;
    float64 physical_minimum_;
    float64 digital_minimum_;
    Type data_type_;
    uint32 data_offset_;
    float64 scale_;
    float64 offset_;
    QString filter_label_;
    float64 lowpass_;
    float64 highpass_;
    bool notch_;
};

} // namespace BioSig_

#endif
