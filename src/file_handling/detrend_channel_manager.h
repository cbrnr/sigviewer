// © SigViewer developers
//
// License: GPL-3.0

#ifndef DETREND_CHANNEL_MANAGER_H
#define DETREND_CHANNEL_MANAGER_H

#include <QMap>
#include <QMutex>

#include "channel_manager.h"

namespace sigviewer {

//-----------------------------------------------------------------------------
/// DetrendChannelManager
///
/// A ChannelManager decorator that removes the global mean offset (DC
/// component) from every channel and optionally applies a zero-phase
/// forward-backward FIR high-pass filter designed with a Blackman window.
///
/// The filtered data for every channel is computed lazily on first access and
/// then cached so that repeated rendering calls are cheap.
class DetrendChannelManager : public ChannelManager {
   public:
    //-------------------------------------------------------------------------
    /// @param source    Underlying channel manager.  This class does NOT take
    ///                  ownership – the caller must ensure that the source
    ///                  outlives this object.
    /// @param hp_cutoff High-pass cutoff frequency in Hz.  Pass 0.0 to skip
    ///                  the filter and only remove the mean.
    explicit DetrendChannelManager(ChannelManager* source, double hp_cutoff = 0.0);

    virtual ~DetrendChannelManager() {}

    double hpCutoff() const { return hp_cutoff_; }

    // ---- ChannelManager interface ------------------------------------------

    virtual std::set<ChannelID> getChannels() const override;
    virtual uint32 getNumberChannels() const override;
    virtual QString getChannelLabel(ChannelID id) const override;
    virtual QString getChannelLabel(ChannelID id, int streamNumber) const override;
    virtual QString getChannelYUnitString(ChannelID id) const override;

    virtual QSharedPointer<DataBlock const> getData(ChannelID id,
        unsigned start_pos,
        unsigned length) const override;

    virtual float64 getDurationInSec() const override;
    virtual size_t getNumberSamples() const override;
    virtual float64 getSampleRate() const override;

    // ---- Downsampled data -- forward to source so the pre-built pyramid -----

    virtual void addDownsampledMinMaxVersion(ChannelID id,
        QSharedPointer<DataBlock const> min,
        QSharedPointer<DataBlock const> max,
        unsigned factor) override;

    virtual unsigned getNearestDownsamplingFactor(ChannelID id, unsigned factor) const override;

    virtual QSharedPointer<DataBlock const> getDownsampledMin(ChannelID id, unsigned factor) const override;

    virtual QSharedPointer<DataBlock const> getDownsampledMax(ChannelID id, unsigned factor) const override;

    // ---- Min/Max -- computed once during processedChannel() ----------------
    // Overriding the single-channel accessors bypasses the inherited
    // initMinMax() scan (O(N×K)), replacing it with an O(1) map lookup.

    virtual float64 getMinValue(ChannelID id) const override;
    virtual float64 getMaxValue(ChannelID id) const override;

    //-------------------------------------------------------------------------
    /// Pre-compute the processed channel from already-read raw data.
    /// Called from the parallel file-open phase to avoid a second disk read.
    /// Thread-safe: multiple channels can be processed concurrently.
    void precomputeFromRawData(ChannelID id, QSharedPointer<DataBlock const> raw) const;

   private:
    //-------------------------------------------------------------------------
    /// Returns (or computes and caches) the fully processed data for a channel.
    QSharedPointer<QVector<float32>> processedChannel(ChannelID id) const;

    //-------------------------------------------------------------------------
    /// Build the Blackman-windowed sinc high-pass FIR kernel.
    /// Returns an empty vector when hp_cutoff_ == 0.
    std::vector<double> buildFirKernel() const;

    //-------------------------------------------------------------------------
    /// Apply zero-phase filtering via FFT: multiply the signal spectrum by
    /// |H(f)|^2 (the squared magnitude of the FIR kernel's frequency response).
    /// O(N log N) – much faster than the time-domain forward+backward pass.
    static void applyZeroPhaseFFT(const std::vector<double>& kernel, std::vector<double>& data);

    ChannelManager* source_;
    double hp_cutoff_;

    mutable QMutex cache_mutex_;
    mutable QMap<ChannelID, QSharedPointer<QVector<float32>>> cache_;
    mutable QMap<ChannelID, float64> channel_min_cache_;
    mutable QMap<ChannelID, float64> channel_max_cache_;
};

}  // namespace sigviewer

#endif  // DETREND_CHANNEL_MANAGER_H
