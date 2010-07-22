#ifndef SIGNAL_VIEW_SETTINGS_H
#define SIGNAL_VIEW_SETTINGS_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"

#include <QObject>
#include <QMap>

#include <set>

namespace BioSig_
{

//-----------------------------------------------------------------------------
/// SignalViewSettings
///
/// provides methods for changing the visualisation of a signal
class SignalViewSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float pixelsPerSample READ getPixelsPerSample WRITE setPixelsPerSample)

public:
    //-------------------------------------------------------------------------
    SignalViewSettings (QSharedPointer<ChannelManager> channel_manager);

    //-------------------------------------------------------------------------
    float getPixelsPerSample () const {return pixels_per_sample_;}

    //-------------------------------------------------------------------------
    float getSampleRate () const {return channel_manager_->getSampleRate();}

    //-------------------------------------------------------------------------
    int getGridFragmentation (Qt::Orientation orientation) const {return grid_fragmentation_[orientation];}

    //-------------------------------------------------------------------------
    /// @return a set of ids of the shown channels
    //std::set<ChannelID> getShownChannels () const {return shown_channels_;}
public slots:
    //-------------------------------------------------------------------------
    void setPixelsPerSample (float pixel_per_sample);

    //-------------------------------------------------------------------------
    void setGridFragmentation (Qt::Orientation orientation, int fragmentation);

    //-------------------------------------------------------------------------
    /// @param channels a set of ids of channels that should be visible
    //void setShownChannels (std::set<ChannelID> const& channels) {shown_channels_ = channels;
    //    shownChannelsChanged (shown_channels_);}

    //-------------------------------------------------------------------------
    //virtual QSharedPointer<ChannelManager const> getChannelManager () const = 0;


signals:
    void pixelsPerSampleChanged ();
    void gridFragmentationChanged ();

private:
    //std::set<ChannelID> shown_channels_;
    float pixels_per_sample_;
    QMap<Qt::Orientation, int> grid_fragmentation_;
    QSharedPointer<ChannelManager> channel_manager_;
};

}

#endif // SIGNAL_VIEW_SETTINGS_H
