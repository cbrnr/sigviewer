#include "channel_manager.h"

#include <QProgressDialog>
#include <limits>

namespace BioSig_
{

//-------------------------------------------------------------------------
float64 ChannelManager::getMinValue (std::set<ChannelID> const& channels) const
{
    if (!min_max_initialized_)
        initMinMax();
    float64 min = std::numeric_limits<float64>::max();
    foreach (ChannelID channel, channels)
        min = std::min (min, min_values_[channel]);
    return min;
}

//-------------------------------------------------------------------------
float64 ChannelManager::getMaxValue (std::set<ChannelID> const& channels) const
{
    if (!min_max_initialized_)
        initMinMax();

    float64 max = -std::numeric_limits<float64>::max();
    foreach (ChannelID channel, channels)
        max = std::max (max, max_values_[channel]);
    return max;
}

//-------------------------------------------------------------------------
float64 ChannelManager::getMinValue (ChannelID channel_id) const
{
    if (!min_max_initialized_)
        initMinMax();

    if (min_values_.count (channel_id))
        return min_values_[channel_id];
    else
        return std::numeric_limits<float64>::min();
}

//-------------------------------------------------------------------------
float64 ChannelManager::getMaxValue (ChannelID channel_id) const
{
    if (!min_max_initialized_)
        initMinMax();

    if (max_values_.count (channel_id))
        return max_values_[channel_id];
    else
        return std::numeric_limits<float64>::max();
}

//-------------------------------------------------------------------------
void ChannelManager::initMinMax () const
{
    QProgressDialog progress_dialog;
    progress_dialog.setMaximum (getNumberChannels() * 2);
    progress_dialog.setLabelText (QObject::tr("Searching Min Max"));
    progress_dialog.show();
    foreach (ChannelID id, getChannels())
    {
        QSharedPointer<DataBlock const> data = getData (id, 0, getNumberSamples ());
        max_values_[id] = data->getMax ();
        progress_dialog.setValue(progress_dialog.value() + 1);
        min_values_[id] = data->getMin ();
        progress_dialog.setValue(progress_dialog.value() + 1);
    }
    progress_dialog.setValue(progress_dialog.maximum());
    min_max_initialized_ = true;
}

}
