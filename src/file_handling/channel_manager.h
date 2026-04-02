// © SigViewer developers
//
// License: GPL-3.0

#ifndef CHANNEL_MANAGER_INTERFACE_H
#define CHANNEL_MANAGER_INTERFACE_H

#include <set>

#include "base/data_block.h"

namespace sigviewer {

/// ChannelManager
///
/// abstract base class for any channel handling
class ChannelManager {
   public:
    /// destructor
    virtual ~ChannelManager() {}

    virtual std::set<ChannelID> getChannels() const = 0;

    virtual uint32 getNumberChannels() const = 0;

    virtual QString getChannelLabel(ChannelID id) const = 0;

    virtual QString getChannelLabel(ChannelID id, int streamNumber) const = 0;  // streamNumber is for XDF only

    virtual QString getChannelYUnitString(ChannelID id) const = 0;

    /// @param channel_id the id of the channel
    /// @param start_pos starting sample
    /// @param length length given in samples
    virtual QSharedPointer<DataBlock const> getData(ChannelID id,
        unsigned start_pos,
        unsigned length) const = 0;

    virtual float64 getDurationInSec() const = 0;

    virtual size_t getNumberSamples() const = 0;

    virtual float64 getSampleRate() const = 0;

    virtual void addDownsampledMinMaxVersion(ChannelID id,
        QSharedPointer<DataBlock const> min,
        QSharedPointer<DataBlock const> max,
        unsigned factor);

    virtual unsigned getNearestDownsamplingFactor(ChannelID id, unsigned factor) const;

    virtual QSharedPointer<DataBlock const> getDownsampledMin(ChannelID id, unsigned factor) const;

    virtual QSharedPointer<DataBlock const> getDownsampledMax(ChannelID id, unsigned factor) const;

    /// Clears the cached min/max values so they are recomputed on the next
    /// access. Call this when the underlying data source changes.
    void invalidateMinMaxCache();

    /// Pre-populate one channel's min/max entry.  Call markMinMaxInitialized()
    /// once all channels have been set to skip the lazy per-channel file scan.
    void setChannelMinMax(ChannelID id, float64 min_val, float64 max_val);

    /// Mark the min/max cache as fully built so initMinMax() is never called.
    void markMinMaxInitialized();

    float64 getValueRange(std::set<ChannelID> const& channels) const;

    virtual float64 getMinValue(std::set<ChannelID> const& channels) const;

    virtual float64 getMaxValue(std::set<ChannelID> const& channels) const;

    virtual float64 getMinValue(ChannelID channel_id) const;

    virtual float64 getMaxValue(ChannelID channel_id) const;

    void setXAxisUnitLabel(QString const& label) { x_axis_unit_label_ = label; }

    QString getXAxisUnitLabel() const { return x_axis_unit_label_; }

   protected:
    ChannelManager() : min_max_initialized_(false) {}

   private:
    void initMinMax() const;

    mutable bool min_max_initialized_;
    mutable std::map<ChannelID, float64> max_values_;
    mutable std::map<ChannelID, float64> min_values_;
    mutable std::map<ChannelID, float64> offsets_;

    QString x_axis_unit_label_;

    QMap<ChannelID, QMap<unsigned, QSharedPointer<DataBlock const> > > downsampled_max_map_;  // [channel][factor] -> maximum downsampled_data
    QMap<ChannelID, QMap<unsigned, QSharedPointer<DataBlock const> > > downsampled_min_map_;  // [channel][factor] -> minimum downsampled_data
};

}  // namespace sigviewer

#endif  // CHANNEL_MANAGER_INTERFACE_H
