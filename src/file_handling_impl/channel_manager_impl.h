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


#ifndef CHANNEL_MANAGER_IMPL_H
#define CHANNEL_MANAGER_IMPL_H

#include "file_handling/channel_manager.h"
#include "file_handling/file_signal_reader.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// ChannelManagerImpl
///
/// implementation of ChannelManager
///
class ChannelManagerImpl : public ChannelManager
{
public:
    //---------------------------------------------------------------------------------------------
    /// @param file_signal_reader the ChannelManagerImpl takes ownership of the given FileSignalReader
    ///                           and destroys it if the ChannelManagerImpl is destructed
    ChannelManagerImpl (FileSignalReader* file_signal_reader);

    //---------------------------------------------------------------------------------------------
    virtual ~ChannelManagerImpl ();

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getChannels () const;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels () const;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel (ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QString getChannelYUnitString (ChannelID id) const;

    //-------------------------------------------------------------------------
    virtual QSharedPointer<DataBlock const> getData (ChannelID id,
                                                     unsigned start_pos,
                                                     unsigned length) const;

    //-------------------------------------------------------------------------
    virtual float64 getDurationInSec() const;

    //-------------------------------------------------------------------------
    virtual size_t getNumberSamples() const;

    //-------------------------------------------------------------------------
    virtual float64 getSampleRate() const;

private:
    FileSignalReader* reader_;
};

}

#endif // CHANNEL_MANAGER_IMPL_H
