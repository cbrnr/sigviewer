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


#include "gdf_basic_header.h"

namespace SigViewer_
{

//-------------------------------------------------------------------------------------------------
GDFBasicHeader::GDFBasicHeader (QString const& file_path,
                                gdf::GDFHeaderAccess const& header_access) : BasicHeader (file_path),
    header_access_ (header_access)
{
    setDownSamplingFactor (1);
    setSampleRate (header_access_.getSignalHeader_readonly(0).get_samplerate());
    for (unsigned index = 0; index < header_access_.getNumSignals(); index++)
    {
        QSharedPointer<SignalChannel> channel (new SignalChannel (index, QString::number(index)));
        addChannel (index, channel);
    }
}

//---------------------------------------------------------------------------------------------
uint32 GDFBasicHeader::getNumberOfSamples () const
{
    return header_access_.getSignalHeader_readonly(0).get_samples_per_record() *
            header_access_.getMainHeader_readonly().get_num_datarecords() /
            getDownSamplingFactor ();
}



}
