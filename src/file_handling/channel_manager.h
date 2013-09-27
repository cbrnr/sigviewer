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


#ifndef CHANNEL_MANAGER_INTERFACE_H
#define CHANNEL_MANAGER_INTERFACE_H

#include "base/sigviewer_user_types.h"
#include "base/data_block.h"

#include <QSharedPointer>
#include <set>
#include <map>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// ChannelManager
///
/// abstract base class for any channel handling
class ChannelManager
{
public:
    //-------------------------------------------------------------------------
    /// destructor
    virtual ~ChannelManager () {}

    //-------------------------------------------------------------------------
    virtual std::set<ChannelID> getChannels () const = 0;

    //-------------------------------------------------------------------------
    virtual uint32 getNumberChannels () const = 0;

    //-------------------------------------------------------------------------
    virtual QString getChannelLabel (ChannelID id) const = 0;

    //-------------------------------------------------------------------------
    virtual QString getChannelYUnitString (ChannelID id) const = 0;

    //-------------------------------------------------------------------------
    /// @param channel_id the id of the channel
    /// @param start_pos starting sample
    /// @param length length given in samples
    virtual QSharedPointer<DataBlock const> getData (ChannelID id,
                                                     unsigned start_pos,
                                                     unsigned length) const = 0;

    //-------------------------------------------------------------------------
    virtual float64 getDurationInSec () const = 0;

    //-------------------------------------------------------------------------
    virtual size_t getNumberSamples () const = 0;

    //-------------------------------------------------------------------------
    virtual float64 getSampleRate () const = 0;

    //-------------------------------------------------------------------------
    void addDownsampledMinMaxVersion (ChannelID id, QSharedPointer<DataBlock const> min,
                                      QSharedPointer<DataBlock const> max, unsigned factor);

    //-------------------------------------------------------------------------
    unsigned getNearestDownsamplingFactor (ChannelID id, unsigned factor) const;

    //-------------------------------------------------------------------------
    QSharedPointer<DataBlock const> getDownsampledMin (ChannelID id, unsigned factor) const;

    //-------------------------------------------------------------------------
    QSharedPointer<DataBlock const> getDownsampledMax (ChannelID id, unsigned factor) const;

    //-------------------------------------------------------------------------
    float64 getValueRange (std::set<ChannelID> const& channels) const;

    //-------------------------------------------------------------------------
    float64 getMinValue (std::set<ChannelID> const& channels) const;

    //-------------------------------------------------------------------------
    float64 getMaxValue (std::set<ChannelID> const& channels) const;

    //-------------------------------------------------------------------------
    float64 getMinValue (ChannelID channel_id) const;

    //-------------------------------------------------------------------------
    float64 getMaxValue (ChannelID channel_id) const;

    //-------------------------------------------------------------------------
    void setXAxisUnitLabel (QString const& label) {x_axis_unit_label_ = label;}

    //-------------------------------------------------------------------------
    QString getXAxisUnitLabel () const {return x_axis_unit_label_;}

protected:
    ChannelManager () : min_max_initialized_ (false) {}

private:
    //-------------------------------------------------------------------------
    void initMinMax () const;

    mutable bool min_max_initialized_;
    mutable std::map<ChannelID, float64> max_values_;
    mutable std::map<ChannelID, float64> min_values_;

    QString x_axis_unit_label_;

    QMap<ChannelID, QMap<unsigned, QSharedPointer<DataBlock const> > > downsampled_max_map_; // [channel][factor] -> maximum downsampled_data
    QMap<ChannelID, QMap<unsigned, QSharedPointer<DataBlock const> > > downsampled_min_map_; // [channel][factor] -> minimum downsampled_data

};

} // namespace SigViewer_

#endif // CHANNEL_MANAGER_INTERFACE_H
