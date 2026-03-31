// © SigViewer developers
//
// License: GPL-3.0

#ifndef CHANNEL_MANAGER_PROXY_H
#define CHANNEL_MANAGER_PROXY_H

#include "channel_manager.h"

namespace sigviewer
{

//-----------------------------------------------------------------------------
/// ChannelManagerProxy
///
/// A ChannelManager that delegates every call to a swappable target.
/// Owned by FileContext so that the SignalBrowserModel can hold a stable
/// const-reference even while the underlying data source (raw vs. filtered)
/// is toggled at run time.
class ChannelManagerProxy : public ChannelManager
{
public:
    //-------------------------------------------------------------------------
    explicit ChannelManagerProxy (ChannelManager* target);

    //-------------------------------------------------------------------------
    /// Switch the target and invalidate any cached min/max state so that the
    /// next access recomputes from the new source.
    void setTarget (ChannelManager* target);

    //-------------------------------------------------------------------------
    ChannelManager* getTarget () const { return target_; }

    // ---- ChannelManager pure-virtual interface ------------------------------

    virtual std::set<ChannelID> getChannels () const override;
    virtual uint32 getNumberChannels () const override;
    virtual QString getChannelLabel (ChannelID id) const override;
    virtual QString getChannelLabel (ChannelID id, int streamNumber) const override;
    virtual QString getChannelYUnitString (ChannelID id) const override;
    virtual QSharedPointer<DataBlock const> getData (ChannelID id,
                                                     unsigned start_pos,
                                                     unsigned length) const override;
    virtual float64 getDurationInSec () const override;
    virtual size_t getNumberSamples () const override;
    virtual float64 getSampleRate () const override;

    // ---- Downsampled data -- forward to target so the pre-built pyramid -----
    // ---- from file-open is still available after the proxy is interposed ----

    virtual void addDownsampledMinMaxVersion (ChannelID id,
                                              QSharedPointer<DataBlock const> min,
                                              QSharedPointer<DataBlock const> max,
                                              unsigned factor) override;

    virtual unsigned getNearestDownsamplingFactor (ChannelID id,
                                                   unsigned factor) const override;

    virtual QSharedPointer<DataBlock const> getDownsampledMin (ChannelID id,
                                                               unsigned factor) const override;

    virtual QSharedPointer<DataBlock const> getDownsampledMax (ChannelID id,
                                                               unsigned factor) const override;

    // ---- Min/Max – delegate to target so its pre-built cache is reused ------
    // ---- and the proxy never needs to scan full channel data itself.    ------

    virtual float64 getMinValue (ChannelID id) const override;
    virtual float64 getMaxValue (ChannelID id) const override;
    virtual float64 getMinValue (std::set<ChannelID> const& channels) const override;
    virtual float64 getMaxValue (std::set<ChannelID> const& channels) const override;

private:
    ChannelManager* target_;
};

} // namespace sigviewer

#endif // CHANNEL_MANAGER_PROXY_H
