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


#ifndef BIOSIG_BASIC_HEADER_H
#define BIOSIG_BASIC_HEADER_H

#include "file_handling/basic_header.h"
#include "biosig.h"

namespace sigviewer
{

class BiosigBasicHeader : public BasicHeader
{
public:
    //-------------------------------------------------------------------------
    BiosigBasicHeader (HDRTYPE* raw_header, QString const& file_path);

    //-------------------------------------------------------------------------
    virtual size_t getNumberOfSamples () const;

    //-------------------------------------------------------------------------
    virtual QMap<unsigned, QString> getNamesOfUserSpecificEvents () const;

private:
    //-------------------------------------------------------------------------
    void readChannelsInfo (HDRTYPE const* raw_header);

    //-------------------------------------------------------------------------
    void readPatientInfo (HDRTYPE const* raw_header);

    //-------------------------------------------------------------------------
    void readRecordingInfo (HDRTYPE const* raw_header);

    unsigned number_samples_;
    QMap<unsigned, QString> user_defined_event_map_;
    QMap<unsigned, QSharedPointer<SignalChannel> > channels_;
};

} // namespace SigViewer_

#endif // BIOSIG_BASIC_HEADER_H
