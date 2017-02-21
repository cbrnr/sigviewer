// Copyright (c) 2016 The SigViewer Development Team
// Licensed under the GNU General Public License (GPL)
// https://www.gnu.org/licenses/gpl


#include "processed_signal_channel_manager.h"
#include <QDebug>

namespace sigviewer
{

//-------------------------------------------------------------------------
ProcessedSignalChannelManager::ProcessedSignalChannelManager(float64 sample_rate, unsigned length, QObject* parent)
    : QObject (parent),
      sample_rate_ (sample_rate),
      length_ (length)
{
    // nothing to do here
}

//-------------------------------------------------------------------------
void ProcessedSignalChannelManager::addChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                 QString const& label, QString const& y_unit_string)
{
    channels_[id] = data_block;
    channel_labels_[id] = label;
    y_unit_strings_[id] = y_unit_string;
}

//-------------------------------------------------------------------------
ChannelID ProcessedSignalChannelManager::addExtraChannel (ChannelID id, QSharedPointer<DataBlock const> data_block,
                                                     QString const& label, QString const& y_unit_string)
{
    id += 0.1f;
    channels_[id] = data_block;
    channel_labels_[id] = label;
    y_unit_strings_[id] = y_unit_string;
    return id;
}

//-------------------------------------------------------------------------
std::set<ChannelID> ProcessedSignalChannelManager::getChannels () const
{
    std::set<ChannelID> channels;
    foreach (ChannelID id, channels_.keys())
        channels.insert(id);
    return channels;
}


//-------------------------------------------------------------------------
uint32 ProcessedSignalChannelManager::getNumberChannels () const
{
    return channels_.size ();
}

//-------------------------------------------------------------------------
QString ProcessedSignalChannelManager::getChannelLabel (ChannelID id) const
{
    return channel_labels_[id];
}

//-------------------------------------------------------------------------
QString ProcessedSignalChannelManager::getChannelYUnitString (ChannelID id) const
{
    return y_unit_strings_[id];
}

//-------------------------------------------------------------------------
QSharedPointer<DataBlock const> ProcessedSignalChannelManager::getData (ChannelID id,
                                                 unsigned start_pos,
                                                 unsigned length) const
{
    return channels_[id]->createSubBlock (start_pos, length);
}

//-------------------------------------------------------------------------
float64 ProcessedSignalChannelManager::getDurationInSec() const
{
    return static_cast<float32>(length_) / sample_rate_;
}

//-------------------------------------------------------------------------
size_t ProcessedSignalChannelManager::getNumberSamples() const
{
    return length_;
}

//-------------------------------------------------------------------------
float64 ProcessedSignalChannelManager::getSampleRate() const
{
    return sample_rate_;
}

//!Inherited, should not be called.----------------------------------------
QString ProcessedSignalChannelManager::getChannelLabel(ChannelID id, int streamNumber) const
{
    qDebug() << "id: " << id << "stream: " << streamNumber;
    return "";
}

}
