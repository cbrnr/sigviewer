#ifndef SIGNAL_VIEW_SETTINGS_H
#define SIGNAL_VIEW_SETTINGS_H

#include "base/sigviewer_user_types.h"
#include "file_handling/channel_manager.h"

#include <QObject>
#include <QMap>

#include <set>

namespace SigViewer_
{

//-----------------------------------------------------------------------------
/// SignalViewSettings
///
/// provides methods for changing the visualisation of a signal
class SignalViewSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float pixelsPerSample READ getPixelsPerSample WRITE setPixelsPerSample)
    Q_PROPERTY(float channelOverlapping READ getChannelOverlapping WRITE setChannelOverlapping)
    Q_PROPERTY(int channelHeight READ getChannelHeight WRITE setChannelHeight)
public:
    //-------------------------------------------------------------------------
    SignalViewSettings (ChannelManager const& channel_manager);

    //-------------------------------------------------------------------------
    ChannelManager const& getChannelManager () const {return channel_manager_;}

    //-------------------------------------------------------------------------
    float getPixelsPerSample () const {return pixels_per_sample_;}

    //-------------------------------------------------------------------------
    float getChannelOverlapping () const {return channel_overlapping_;}

    //-------------------------------------------------------------------------
    int getChannelHeight () const {return channel_heigth_in_pixels_;}

    //-------------------------------------------------------------------------
    float getSampleRate () const {return channel_manager_.getSampleRate();}

    //-------------------------------------------------------------------------
    int getGridFragmentation (Qt::Orientation orientation) const {return grid_fragmentation_[orientation];}

    //-------------------------------------------------------------------------
    /// @return a set of ids of the shown channels
    //std::set<ChannelID> getShownChannels () const {return shown_channels_;}
public slots:
    //-------------------------------------------------------------------------
    void setPixelsPerSample (float pixel_per_sample);

    //-------------------------------------------------------------------------
    void setChannelOverlapping (float channel_overlapping);

    //-------------------------------------------------------------------------
    void setChannelHeight (int channel_heigth_in_pixels);

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
    void channelOverlappingChanged ();
    void channelHeightChanged ();
    void channelHeightChanged (unsigned channel_height_in_pixel);
    void gridFragmentationChanged ();

private:
    //std::set<ChannelID> shown_channels_;
    float pixels_per_sample_;
    float channel_overlapping_;
    int channel_heigth_in_pixels_;
    QMap<Qt::Orientation, int> grid_fragmentation_;
    ChannelManager const& channel_manager_;
};

}

#endif // SIGNAL_VIEW_SETTINGS_H
