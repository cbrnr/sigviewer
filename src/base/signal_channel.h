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


#ifndef SIGNAL_CHANNEL_H
#define SIGNAL_CHANNEL_H

#include "sigviewer_user_types.h"
#include "biosig.h"

#include <QString>
#include <QMutex>

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// @class SignalChannel
/// @brief data about a signal channel
class SignalChannel
{
public:
    //-------------------------------------------------------------------------
    SignalChannel(unsigned ch, const HDRTYPE* hdr);
    SignalChannel(unsigned number, CHANNEL_TYPE C);  /* obsolete, deprecated */

    //-------------------------------------------------------------------------
    QString typeString() const;
    const QString& getLabel() const;
    float64 getLowpass() const;
    float64 getHighpass() const;
    bool getNotch() const;
    const QString& getPhysicalDim() const;
    float64 getPhysicalMaximum() const;
    float64 getDigitalMaximum() const;
    float64 getPhysicalMinimum() const;
    float64 getDigitalMinimum() const;
    float64 getSampleRate() const;

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
    
    //uint32 number_;
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
    float64 samplerate_;
    bool notch_;
};

} // namespace SigViewer_

#endif
