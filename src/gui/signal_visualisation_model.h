#ifndef SIGNAL_VISUALISATION_MODEL_H
#define SIGNAL_VISUALISATION_MODEL_H

#include "../base/user_types.h"

#include <QObject>

#include <set>

namespace BioSig_
{

enum ZoomDimension
{
    ZOOM_VERTICAL,
    ZOOM_HORIZONTAL,
    ZOOM_BOTH
};

//-----------------------------------------------------------------------------
/// SignalVisualisationModel
///
/// base class for all
class SignalVisualisationModel : public QObject
{
    Q_OBJECT
public:

    virtual ~SignalVisualisationModel () {}

    virtual std::set<ChannelID> getShownChannels () const = 0;
    virtual bool setShownChannels (std::set<ChannelID> const& shown_channels) = 0;

    virtual void zoom (ZoomDimension dimension, float factor) = 0;

    void setPixelPerSample (float32 pixel_per_sample)
    {
        pixel_per_sample_ = pixel_per_sample;
        emit pixelPerSampleChanged (pixel_per_sample_, sample_rate_);
    }

    float32 getPixelPerSample () {return pixel_per_sample_;}
    float32 getSampleRate () {return sample_rate_;}


    virtual void updateLayout () = 0;

signals:
    void pixelPerSampleChanged (float32 pixel_per_sample, float32 sample_rate);

protected:
    SignalVisualisationModel (float32 sample_rate)
        : pixel_per_sample_ (1),
          sample_rate_ (sample_rate)
    {}

private:
    float32 pixel_per_sample_;
    float32 sample_rate_;
};


} // namespace BioSig_

#endif // SIGNAL_VISUALISATION_MODEL_H
